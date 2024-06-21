#include "VIBuffer_Instance.h"

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

//void CVIBuffer_Instance::Fountain(_float fTimeDelta)
//{
//	D3D11_MAPPED_SUBRESOURCE		SubResource{};
//
//	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
//
//	for (size_t i = 0; i < m_iNumInstance; i++)
//	{
//		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
//		pVertices[i].vLifeTime.y += fTimeDelta;
//		m_pAccelTime[i] += fTimeDelta * 0.01f;
//
//		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc->vOffsetPos), 0.f);
//
//		_vector vPos = XMLoadFloat4(&pVertices[i].vTranslation) + (XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);
//		_float fY = XMVectorGetY(vPos);
//		_float fTemp = (m_pPower[i] * m_pAccelTime[1]) - (9.8f * m_pAccelTime[1] * m_pAccelTime[1] * 0.5f);
//
//		vPos = XMVectorSetY(vPos, fY + fTemp);
//
//		XMStoreFloat4(&pVertices[i].vTranslation, vPos);
//
//		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
//		{
//			if (true == m_InstanceDesc->isLoop)
//			{
//				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
//				pVertices[i].vLifeTime.y = 0.f;
//				m_pAccelTime[1] = 0.f;
//			}
//		}
//	}
//
//	m_pContext->Unmap(m_pVBInstance, 0);
//}

void CVIBuffer_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pOriginalPositions);
	}

	Safe_Release(m_pVBInstance);
}
