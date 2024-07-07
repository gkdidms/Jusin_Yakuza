#include "Bounding_AABB.h"

#include "DebugDraw.h"

CBounding_AABB::CBounding_AABB()
	: CBounding{}
{
}

void CBounding_AABB::Set_Value(void* pDesc)
{
	m_vExtents = *((_float3*)pDesc);

	m_pOriginalBox->Extents = m_vExtents;
}

void CBounding_AABB::Set_Center(const _float3& vCenter)
{
	m_vCenter = vCenter;

	m_pOriginalBox->Center = m_vCenter;
}

void CBounding_AABB::Set_Rotation(const _float3& vRotation)
{
}

HRESULT CBounding_AABB::Initialize(const void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const BOUNDING_AABB_DESC* pDesc = static_cast<const BOUNDING_AABB_DESC*>(pArg);
	m_vCenter = pDesc->vCenter;
	m_vExtents = pDesc->vExtents;

	m_vColor = _float4(0.f, 1.f, 0.f, 1.f);

	m_pOriginalBox = new BoundingBox(m_vCenter, m_vExtents);
	m_pBoundingBox = new BoundingBox(*m_pOriginalBox);
	if (nullptr == m_pBoundingBox)
		return E_FAIL;

	return S_OK;
}

void CBounding_AABB::Tick(_fmatrix WorldMatrix)
{
	_matrix		TransformMatrix = WorldMatrix;


	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]);

	m_pOriginalBox->Transform(*m_pBoundingBox, TransformMatrix);
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eTargetType, CBounding* pTargetBounding)
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
	{
		BoundingSphere* pDesc = static_cast<BoundingSphere*>(pTargetBounding->Get_Desc());
		isColl = m_pBoundingBox->Intersects(*pDesc);
		break;
	}
	case Engine::CCollider::COLLIDER_END:
		break;
	default:
		break;
	}

	return isColl;
}

const _float3& CBounding_AABB::ImpulseResolution(CCollider::TYPE eTargetType, CBounding* pTargetBounding)
{
	_float3 vPosition = _float3();
	float fIntersectSize = { 0.f };

	// 밀어내기 기능은 AABB에서만 할 것
	if (Intersect(eTargetType, pTargetBounding))
	{
		switch (eTargetType)
		{
		case Engine::CCollider::COLLIDER_AABB:
		{
			BoundingBox* pDesc = static_cast<BoundingBox*>(pTargetBounding->Get_Desc());
			vPosition = pDesc->Center;

			if (m_pBoundingBox->Intersects(*pDesc)) {
				// Calculate the intersection boundaries
				_vector vBox1Min = XMVectorSubtract(XMLoadFloat3(&m_pBoundingBox->Center), XMLoadFloat3(&m_pBoundingBox->Extents));
				_vector vBox1Max = XMVectorAdd(XMLoadFloat3(&m_pBoundingBox->Center), XMLoadFloat3(&m_pBoundingBox->Extents));

				_vector vBox2Min = XMVectorSubtract(XMLoadFloat3(&pDesc->Center), XMLoadFloat3(&pDesc->Extents));
				_vector vBox2Max = XMVectorAdd(XMLoadFloat3(&pDesc->Center), XMLoadFloat3(&pDesc->Extents));

				_vector vIntersectMin = XMVectorMax(vBox1Min, vBox2Min);
				_vector vIntersectMax = XMVectorMin(vBox1Max, vBox2Max);

				_vector vIntersectSize = XMVectorSubtract(vIntersectMax, vIntersectMin);

				_float fIntersectWidth = XMVectorGetX(vIntersectSize);
				_float fIntersectHeight = XMVectorGetY(vIntersectSize);
				_float fIntersectDepth = XMVectorGetZ(vIntersectSize);

				// Determine the smallest axis to push out along
				if (fIntersectWidth < fIntersectHeight && fIntersectWidth < fIntersectDepth) {
					// Push out along the X-axis
					float fPushX = (m_pBoundingBox->Center.x < pDesc->Center.x) ? -fIntersectWidth : fIntersectWidth;
					vPosition = XMFLOAT3(fPushX, 0.0f, 0.0f);
				}
				else if (fIntersectHeight < fIntersectDepth) {
					// Push out along the Y-axis
					float fPushY = (m_pBoundingBox->Center.y < pDesc->Center.y) ? -fIntersectHeight : fIntersectHeight;
					vPosition = XMFLOAT3(0.0f, fPushY, 0.0f);
				}
				else {
					// Push out along the Z-axis
					float fPushZ = (m_pBoundingBox->Center.z < pDesc->Center.z) ? -fIntersectDepth : fIntersectDepth;
					vPosition = XMFLOAT3(0.0f, 0.0f, fPushZ);
				}
			}
			else {
				// No intersection, no push out needed
				vPosition = _float3();
			}
			break;
		}
		default:
			vPosition = _float3();
			break;
		}
	}

	return vPosition;
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingBox, XMLoadFloat4(&m_vColor));

	return S_OK;
}
#endif // _DEBUG

CBounding_AABB* CBounding_AABB::Create(const void* pArg)
{
	CBounding_AABB* pInstance = new CBounding_AABB();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalBox);
	Safe_Delete(m_pBoundingBox);
}
