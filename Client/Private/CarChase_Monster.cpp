#include "CarChase_Monster.h"

#include "GameInstance.h"

CCarChase_Monster::CCarChase_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster { pDevice, pContext }
{
}

CCarChase_Monster::CCarChase_Monster(const CCarChase_Monster& rhs)
	: CMonster{ rhs }
{
}

HRESULT CCarChase_Monster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_Monster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CARCHASE_MONSTER_DESC* pDesc = static_cast<CARCHASE_MONSTER_DESC*>(pArg);
	m_pParentMatrix = pDesc->pParentMatrix;

	return S_OK;
}

void CCarChase_Monster::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChase_Monster::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCarChase_Monster::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CCarChase_Monster::Change_Animation()
{
}

HRESULT CCarChase_Monster::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.5, 0.8, 0.5);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_CarChaseAnim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

void CCarChase_Monster::Free()
{
	__super::Free();
}
