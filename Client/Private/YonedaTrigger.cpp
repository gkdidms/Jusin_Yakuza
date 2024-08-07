#include "YonedaTrigger.h"

#include "Player.h"
#include "Yoneda.h"

CYonedaTrigger::CYonedaTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTrigger{ pDevice, pContext }
{
}

CYonedaTrigger::CYonedaTrigger(const CYonedaTrigger& rhs)
	: CTrigger{ rhs }
{
}

HRESULT CYonedaTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CYonedaTrigger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CYonedaTrigger::Priority_Tick(const _float& fTimeDelta)
{
}

void CYonedaTrigger::Tick(const _float& fTimeDelta)
{
	m_pColliderCom->Tick(m_WorldMatrix);

	__super::Tick(fTimeDelta);
}

void CYonedaTrigger::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CYonedaTrigger::Render()
{
	return S_OK;
}

bool CYonedaTrigger::Move_Scene(int& iLevelNum)
{
	//충돌 시 요네다에게 싱크 액션 전달
	if (m_pColliderCom->Intersect(dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0))->Get_PlayerCollider(), 3))
	{
		if (m_pGameInstance->GetKeyState(DIK_G) == TAP)
		{
			
		}
	}
	return false;
}

HRESULT CYonedaTrigger::Add_Components(void* pArg)
{
	TRIGGEROBJ_DESC* triggerDesc = (TRIGGEROBJ_DESC*)pArg;

	m_tTriggerDesc = triggerDesc->tTriggerDesc;


	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(triggerDesc->tTriggerDesc.vTransform._11, triggerDesc->tTriggerDesc.vTransform._22, triggerDesc->tTriggerDesc.vTransform._33);
	ColliderDesc.vCenter = _float3(triggerDesc->tTriggerDesc.vTransform._41, triggerDesc->tTriggerDesc.vTransform._42, triggerDesc->tTriggerDesc.vTransform._43);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CYonedaTrigger* CYonedaTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CYonedaTrigger* pInstance = new CYonedaTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CYonedaTrigger");
		Safe_Release(pInstance);
	}
		

	return pInstance;
}

CGameObject* CYonedaTrigger::Clone(void* pArg)
{
	CYonedaTrigger* pInstance = new CYonedaTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Created : CYonedaTrigger");
		Safe_Release(pInstance);
	}
		

	return pInstance;
}

void CYonedaTrigger::Free()
{
	__super::Free();
}
