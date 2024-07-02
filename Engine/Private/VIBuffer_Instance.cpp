#include "VIBuffer_Instance.h"
#include "ComputeShader.h"
#include "GameInstance.h"
CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& rhs)
	: CVIBuffer{ rhs }
{
	//m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance);
	//m_pContext->CopyResource(m_pVBInstance, rhs.m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		INSTANCE_DESC* pDesc = static_cast<INSTANCE_DESC*>(pArg);

		m_InstanceDesc = pDesc;
		
	}

	return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{

	ID3D11Buffer* pVertices[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint pStrideVertices[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint pStartVertices[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertices, pStrideVertices, pStartVertices);
	m_pContext->IASetIndexBuffer(m_pIB, m_GIFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_Primitive_Topology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_InstanceDesc->iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Compute_Render()
{
	CComputeShader::COMPUTE_DESC ComputeDesc{};	
	ComputeDesc.OutputBuffer = mOutputUAV;
	ComputeDesc.NumElement = m_InstanceDesc->iNumInstance;

	m_pComputeShader->Render(&ComputeDesc);

	m_pContext->Flush();




	COMPUTEMATRIX* pCopyBlends = new COMPUTEMATRIX[m_InstanceDesc->iNumInstance];
	ZeroMemory(pCopyBlends, sizeof(VTXMATRIX) * m_InstanceDesc->iNumInstance);

	D3D11_BUFFER_DESC	InstanceBufferDesc = {};

	InstanceBufferDesc.ByteWidth = sizeof(COMPUTEMATRIX) * m_InstanceDesc->iNumInstance;
	InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	InstanceBufferDesc.MiscFlags = 0;
	InstanceBufferDesc.StructureByteStride = sizeof(COMPUTEMATRIX);

	ID3D11Buffer* Instance = { nullptr };


	m_InitialData.pSysMem = pCopyBlends;

	if (FAILED(m_pDevice->CreateBuffer(&InstanceBufferDesc, &m_InitialData, &Instance)))
		return E_FAIL;

	Safe_Delete_Array(pCopyBlends);	

	m_pContext->CopyResource(Instance, m_pOutBuffer);




	//이곳에서 m_pBlendSortBuffer이 버퍼를 m_pVBInstance 버퍼로 바꿔준
	D3D11_MAPPED_SUBRESOURCE		SubResourceOutput{};
	D3D11_MAPPED_SUBRESOURCE		SubResourceRender{};

	m_pContext->Map(Instance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceOutput);
	m_pContext->Map(m_pRenderBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceRender);

	COMPUTEMATRIX* pOutputVertices = (COMPUTEMATRIX*)SubResourceOutput.pData;		
	VTXMATRIX* pRenderVertices = (VTXMATRIX*)SubResourceRender.pData;

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		//두매트릭스는 같은데 맨뒤에 값이 추가됫냐 안됫냐 차이
		memcpy(&pRenderVertices[i], &pOutputVertices[i], sizeof(VTXMATRIX));
	}

	m_pContext->Unmap(Instance, 0);
	m_pContext->Unmap(m_pRenderBuffer, 0);	

	Safe_Release(Instance);

	ID3D11Buffer* pVertices[] = {
		m_pVB,
		m_pRenderBuffer//컴퓨트 셰이드 거치면 값이 바뀜(원본은 계산용, 이건 렌더용)
	};

	_uint pStrideVertices[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint pStartVertices[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertices, pStrideVertices, pStartVertices);
	m_pContext->IASetIndexBuffer(m_pIB, m_GIFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_Primitive_Topology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_InstanceDesc->iNumInstance, 0, 0, 0);


	return S_OK;
}

void CVIBuffer_Instance::Spread(_float fTimeDelta)
{

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		pVertices[i].vLifeTime.y += fTimeDelta;
		//x가 최종,y 가 current
		_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];


		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);
		XMStoreFloat4(&pVertices[i].vDirection, vDir);	

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				m_pOriginalOffsets[i] = _float3(m_InstanceDesc->vOffsetPos.x + XMVectorGetX(WorlPosition), m_InstanceDesc->vOffsetPos.y + XMVectorGetY(WorlPosition), m_InstanceDesc->vOffsetPos.z + XMVectorGetZ(WorlPosition)); // Loop를 위해 저장해준다.
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f);
				XMStoreFloat4(&pVertices[i].vDirection, vDir);

				pVertices[i].vTranslation.x += XMVectorGetX(WorlPosition);
				pVertices[i].vTranslation.y += XMVectorGetY(WorlPosition);
				pVertices[i].vTranslation.z += XMVectorGetZ(WorlPosition);
				


				pVertices[i].vRectSize.x = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);	//크기
				pVertices[i].vRectSize.y = m_pGameInstance->Get_Random(0.f, 360.f);//회전
			}
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
		pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
		pVertices[i].vLifeTime.y += fTimeDelta;

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::LifeTime_Check()
{

}

void CVIBuffer_Instance::SizeUp_Time(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		//x가 최종,y 가 current

		pVertices[i].vRectSize.x = (1.f + (pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x)) * m_pOriginalSize[i];

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc->vOffsetPos), 0.f);
				XMStoreFloat4(&pVertices[i].vDirection, vDir);
				pVertices[i].vRectSize.x = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);
			}
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::SizeDown_Time(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		//x가 최종,y 가 current
		pVertices[i].vRectSize.x = (1.f - (pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x)) * m_pOriginalSize[i];

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc->vOffsetPos), 0.f);
				XMStoreFloat4(&pVertices[i].vDirection, vDir);
				pVertices[i].vRectSize.x = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);
			}
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::Compute_Sort()
{
	//버퍼에 복사 해둠.
	D3D11_MAPPED_SUBRESOURCE		SubResourceOrigin{};
	D3D11_MAPPED_SUBRESOURCE		SubResourceCopy{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceOrigin);	
	//m_pContext->Map(m_pOutBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceCopy);	

	VTXMATRIX* pVertices = (VTXMATRIX*)SubResourceOrigin.pData;

	COMPUTEMATRIX* pCopyBlends = new COMPUTEMATRIX[m_InstanceDesc->iNumInstance];
	ZeroMemory(pCopyBlends, sizeof(VTXMATRIX) * m_InstanceDesc->iNumInstance);

	_vector CamPos = m_pGameInstance->Get_CamPosition();

	ZeroMemory(pCopyBlends, sizeof(COMPUTEMATRIX) * m_InstanceDesc->iNumInstance);//계산용은 초기화후 사용

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		memcpy(&pCopyBlends[i], &pVertices[i], sizeof(VTXMATRIX));	

		memcpy(&pCopyBlends[i].vCamPos, &CamPos, sizeof(_float4));
	}

//	m_pContext->Unmap(m_pOutBuffer, 0);
	m_pContext->Unmap(m_pVBInstance, 0);

	m_InitialData.pSysMem = pCopyBlends;

	if (FAILED(m_pDevice->CreateBuffer(&m_OutBufferDesc, &m_InitialData, &m_pOutBuffer)))
		return ;
	//셰이더에서 읽기,쓰기.
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pOutBuffer, &uavDesc, &mOutputUAV)))
		return ;


}

void CVIBuffer_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pOriginalPositions);
		Safe_Delete_Array(m_pOriginalOffsets);
		Safe_Delete_Array(m_pOriginalSize);
	}

	Safe_Release(m_pComputeShader);
	Safe_Release(m_pRenderBuffer);
	Safe_Release(m_pOutBuffer);
	Safe_Release(mOutputUAV);
	Safe_Release(m_pVBInstance);
}
