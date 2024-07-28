#include "DefaultYakuza.h"

#include "GameInstance.h"

#include "AI_DefaultYakuza.h"

CDefaultYakuza::CDefaultYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster {pDevice, pContext}
{
}

CDefaultYakuza::CDefaultYakuza(const CDefaultYakuza& rhs)
	: CMonster { rhs }
{
}

HRESULT CDefaultYakuza::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDefaultYakuza::Initialize(void* pArg)
{

	return S_OK;
}

void CDefaultYakuza::Priority_Tick(const _float& fTimeDelta)
{
}

void CDefaultYakuza::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CDefaultYakuza::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CDefaultYakuza::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.5, 0.8, 0.5);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom[ATK_ANIM]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_SyncAnim"),
		TEXT("Com_SyncAnim"), reinterpret_cast<CComponent**>(&m_pAnimCom[SYNC_ANIM]))))
		return E_FAIL;

	//행동트리 저장
	CAI_DefaultYakuza::AI_MONSTER_DESC AIDesc{};
	AIDesc.pState = &m_iState;
	memcpy(AIDesc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_END);
	AIDesc.pCurrentAnimType = &m_iCurrentAnimType;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_DefaultYakuza*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_DefaultYakuza"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

void CDefaultYakuza::Change_Animation()
{
}

CDefaultYakuza* CDefaultYakuza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CDefaultYakuza::Clone(void* pArg)
{
	return nullptr;
}

void CDefaultYakuza::Free()
{
}
