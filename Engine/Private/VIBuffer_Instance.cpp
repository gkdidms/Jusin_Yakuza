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
	m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance);
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

	m_pComputeShader = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute.hlsl"));


	//���� �ٲ� instance ���۸� ���� ���� ��ü(�б� ����)
	m_BlendSortbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BlendSortbufferDesc.ByteWidth = sizeof(BlendSort) * m_InstanceDesc->iNumInstance;
	m_BlendSortbufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	m_BlendSortbufferDesc.CPUAccessFlags = 0;
	m_BlendSortbufferDesc.StructureByteStride = sizeof(BlendSort);
	m_BlendSortbufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	
	
	m_pDevice->CreateBuffer(&m_BlendSortbufferDesc, nullptr, &m_pBlendSortBuffer);

	//�����̴��� �ڷḦ ����Ҽ� �ְ� ����(����ؼ� ������ ���µ� �ٽ� instance���ۿ� ��������ߵ�.)
	m_OutBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_OutBufferDesc.ByteWidth = sizeof(BlendSort) * m_InstanceDesc->iNumInstance;
	m_OutBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	m_OutBufferDesc.CPUAccessFlags = 0;
	m_OutBufferDesc.StructureByteStride = sizeof(BlendSort);
	m_OutBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	
	m_pDevice->CreateBuffer(&m_OutBufferDesc, nullptr, &m_pOutBuffer);



	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = m_InstanceDesc->iNumInstance;


	m_pDevice->CreateShaderResourceView(m_pBlendSortBuffer, &srvDesc, &mInputSRV);//���̴����� �б� ����


	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = m_InstanceDesc->iNumInstance;


	m_pDevice->CreateUnorderedAccessView(m_pOutBuffer, &uavDesc, &mOutputUAV);//���̴����� �б�,����.


	m_RenderBufferDesc.ByteWidth = m_iInstanceStride * m_InstanceDesc->iNumInstance;
	m_RenderBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_RenderBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_RenderBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_RenderBufferDesc.MiscFlags = 0;
	m_RenderBufferDesc.StructureByteStride = m_iInstanceStride;

	VTXMATRIX* pInstanceVertices = new VTXMATRIX[m_InstanceDesc->iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_InstanceDesc->iNumInstance);

	m_InitialData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_RenderBufferDesc, &m_InitialData, &m_pRenderBuffer)))//���� �����
		return E_FAIL;


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
	ComputeDesc.InputBuffer = mInputSRV;
	ComputeDesc.OutputBuffer = mOutputUAV;
	ComputeDesc.NumElement = m_InstanceDesc->iNumInstance;

	m_pComputeShader->Render(&ComputeDesc);


	//�̰����� m_pBlendSortBuffer�� ���۸� m_pVBInstance ���۷� �ٲ���
	D3D11_MAPPED_SUBRESOURCE		SubResourceRender{};
	D3D11_MAPPED_SUBRESOURCE		SubResourceOutput{};

	m_pContext->Map(m_pRenderBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceRender);
	m_pContext->Map(m_pOutBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceOutput);

	BlendSort* pOutputVertices = (BlendSort*)SubResourceOutput.pData;
	VTXMATRIX* pRenderVertices = (VTXMATRIX*)SubResourceRender.pData;

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		memcpy(&pRenderVertices[i], &pOutputVertices[i].vMatrix, sizeof(VTXMATRIX));
	}

	m_pContext->Unmap(m_pRenderBuffer, 0);	
	m_pContext->Unmap(m_pOutBuffer, 0);



	ID3D11Buffer* pVertices[] = {
		m_pVB,
		m_pRenderBuffer//��ǻƮ ���̵� ��ġ�� ���� �ٲ�(������ �״�� ��ߵǼ�)
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
		//x�� ����,y �� current
		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc->vOffsetPos), 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);
		XMStoreFloat4(&pVertices[i].vDirection, vDir);	
		//�ð��� ������ ���� �۾���
		//	pVertices[i].vRectSize = pVertices[i].vRectSize * ((pVertices[i].vLifeTime.x - pVertices[i].vLifeTime.y) / pVertices[i].vLifeTime.x);
		//�ð��� ������ ���� Ŀ��
		//pVertices[i].vRectSize = pVertices[i].vRectSize * (pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x) ;
		
		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc->vOffsetPos), 0.f);
				XMStoreFloat4(&pVertices[i].vDirection, vDir);
				pVertices[i].vRectSize = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);	
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

void CVIBuffer_Instance::Blend_Sort()
{
	//�� ƽ ������ �Ͼ�� �Ǵϱ�
	//�̰� ������ ������ ���Ѱ��� �̵��̳� ��ȯ�� �����Ƿ� �̰��� ������ ���� ���� �����ڿ� �ٽ� �Ű� ����.
	
	list<BlendSort> InstanceMatrix;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	//�޾ƿ� �ӽ�����

	if(nullptr == m_pTempVertices)
		m_pTempVertices = pVertices;

	_vector CamPos = m_pGameInstance->Get_CamPosition();

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		if (nullptr != m_pTempVertices)
			pVertices[i] = m_pTempVertices[i];

		BlendSort BlendDesc{};

		BlendDesc.ViewZ = XMVectorGetX(XMVector4Length(XMLoadFloat4(&pVertices[i].vTranslation) - CamPos));
		memcpy(&BlendDesc.vMatrix, &pVertices[i], sizeof(VTXMATRIX));

		InstanceMatrix.emplace_back(BlendDesc);
	}

	//����
	InstanceMatrix.sort([](BlendSort pSour, BlendSort pDest)->_bool
		{
			return pSour.ViewZ > pDest.ViewZ;
		});

	auto iter = InstanceMatrix.begin();
	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		pVertices[i] = iter->vMatrix;
		iter++;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::Compute_Sort()
{
	//���ۿ� ���� �ص�.
	D3D11_MAPPED_SUBRESOURCE		SubResourceOrigin{};
	D3D11_MAPPED_SUBRESOURCE		SubResourceCopy{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceOrigin);
	m_pContext->Map(m_pBlendSortBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceCopy);

	VTXMATRIX* pVertices = (VTXMATRIX*)SubResourceOrigin.pData;
	BlendSort* pCopyBlends = (BlendSort*)SubResourceCopy.pData;

	_vector CamPos = m_pGameInstance->Get_CamPosition();

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		memcpy(&pCopyBlends[i].vMatrix, &pVertices[i], sizeof(VTXMATRIX));	
		pCopyBlends[i].ViewZ = XMVectorGetX(XMVector4Length(XMLoadFloat4(&pVertices[i].vTranslation) - CamPos));	
	}

	m_pContext->Unmap(m_pBlendSortBuffer, 0);
	m_pContext->Unmap(m_pVBInstance, 0);


	//���ε� �ϱ� ���� �Ű������� �� ���� ���� �ʱ�ȭ.

	D3D11_MAPPED_SUBRESOURCE		SubResourceOutput{};

	m_pContext->Map(m_pOutBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceOutput);

	BlendSort* pOutputBlends = (BlendSort*)SubResourceOutput.pData;

	ZeroMemory(pOutputBlends, sizeof(BlendSort) * m_InstanceDesc->iNumInstance);

	m_pContext->Unmap(m_pOutBuffer, 0);

	//���� �� �ٲٸ� �ڵ����� ���̴����ҽ��� �� �� �ٲ� 
	

}

void CVIBuffer_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pOriginalPositions);
	}

	Safe_Release(m_pComputeShader);
	Safe_Release(m_pBlendSortBuffer);
	Safe_Release(m_pVBInstance);
}
