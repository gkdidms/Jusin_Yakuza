#include "YonedaTrigger.h"

#include "Player.h"
#include "Yoneda.h"
#include "Navigation.h"

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

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0));

	if (nullptr != pPlayer)
	{
		//�浹 �� ��״ٿ��� ��ũ �׼� ����
		if (m_pColliderCom->Intersect(pPlayer->Get_PlayerCollider(), 3))
		{
			m_isColl = true;
		}
		else if (m_isColl && m_iCollCount < 1)				//�浹�ߴٰ� ��������
		{
			m_iCollCount++;
			m_isColl = false;

			// ��״� Ʈ���Ŵ� ������ ��״ٸ� �����Ѵ�.

			//YONEDA_H,                   //A60300 ��״� ����
			//YONEDA_DOWN_ATTACK,         //A60330 ��״� ����
			//YONEDA_DOSU,                //a60350 ��״� ȭ���
			CPlayer::CUTSCENE_ANIMATION_TYPE eType = CPlayer::YONEDA_H;
			if (m_tTriggerDesc.iTriggerID == 1000)
				eType = CPlayer::YONEDA_H;
			else if (m_tTriggerDesc.iTriggerID == 1001)
				eType = CPlayer::YONEDA_DOWN_ATTACK;
			else if (m_tTriggerDesc.iTriggerID == 1002)
				eType = CPlayer::YONEDA_DOSU;

			CYoneda* pYoneda = { nullptr };

			// ���� �ƽ��� �����ϰ�� ���λ����ؾ���.
			if (m_tTriggerDesc.iTriggerID != 1001)
				Create_Yoneda();

			// ��״ٰ� ���ٸ� ������� ����������Ѵ�.
			pYoneda = dynamic_cast<CYoneda*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Yoneda"), 0));
			pPlayer->Set_TargetObject(pYoneda);
			pYoneda->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

			// �������̶�� �������ֱ�
			if (m_tTriggerDesc.iTriggerID == 1001)
			{
				pYoneda->Reset_Monster();
			}

			dynamic_cast<CNavigation*>(pYoneda->Get_Component(TEXT("Com_Navigation")))->Set_Index(
				dynamic_cast<CNavigation*>(pYoneda->Get_Component(TEXT("Com_Navigation")))->Find_PlayerMonster_Index(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			pYoneda->Set_TriggerQte(m_tTriggerDesc.iYonedaKnife, m_tTriggerDesc.iTriggerID);

			pPlayer->Set_CutSceneAnim(eType);
		}
	}



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
	return false;
}

HRESULT CYonedaTrigger::Create_Yoneda()
{
	CYoneda::MONSTER_IODESC		monsterDesc;
	monsterDesc.vStartPos = m_pTransformCom->Get_WorldMatrix();
	monsterDesc.wstrModelName = TEXT("Yoneda");

	monsterDesc.fSpeedPecSec = 10.f;
	monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
	monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);

	if (FAILED(m_pGameInstance->Add_GameObject(m_iCurrentLevel, TEXT("Prototype_GameObject_Yoneda"), TEXT("Layer_Yoneda"), &monsterDesc)))
		return E_FAIL;

	return S_OK;
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
