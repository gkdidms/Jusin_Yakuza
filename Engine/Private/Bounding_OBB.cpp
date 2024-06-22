#include "Bounding_OBB.h"

#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB()
	: CBounding{}
{
}

void CBounding_OBB::Set_Value(void* pDesc)
{
	m_vExtents = *((_float3*)pDesc);
}

HRESULT CBounding_OBB::Initialize(const void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const BOUNDING_OBB_DESC* pDesc = static_cast<const BOUNDING_OBB_DESC*>(pArg);

	_float4		vQuaternion;
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_vCenter = pDesc->vCenter;
	m_vExtents = pDesc->vExtents;

	m_pOriginalBox = new BoundingOrientedBox(m_vCenter, m_vExtents, vQuaternion);
	m_pBoundingBox = new BoundingOrientedBox(*m_pOriginalBox);
	if (m_pBoundingBox == nullptr)
		return E_FAIL;

	return S_OK;
}

void CBounding_OBB::Tick(_fmatrix WorldMatrix)
{
	m_pOriginalBox->Transform(*m_pBoundingBox, WorldMatrix);
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eTargetType, CBounding* pTargetBounding)
{
	_bool isColl = { false };
	switch (eTargetType)
	{
	case Engine::CCollider::COLLIDER_AABB:
	{
		BoundingBox* pDesc = static_cast<BoundingBox*>(pTargetBounding->Get_Desc());
		isColl = m_pBoundingBox->Intersects(*pDesc);
		break;
	}
	case Engine::CCollider::COLLIDER_OBB:
	{
		BoundingOrientedBox* pDesc = static_cast<BoundingOrientedBox*>(pTargetBounding->Get_Desc());
		isColl = m_pBoundingBox->Intersects(*pDesc);
		break;
	}
	case Engine::CCollider::COLLIDER_SPHERE:
		break;
	case Engine::CCollider::COLLIDER_END:
		break;
	default:
		break;
	}

	return isColl;
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingBox, XMVectorSet(0.f, 0.f, 1.f, 1.f));

	return S_OK;
}
#endif // _DEBUG

CBounding_OBB* CBounding_OBB::Create(const void* pArg)
{
	CBounding_OBB* pInstance = new CBounding_OBB();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();
}
