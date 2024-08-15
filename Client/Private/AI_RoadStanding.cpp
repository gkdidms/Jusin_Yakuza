#include "AI_RoadStanding.h"

#include "GameInstance.h"

#include "Player.h"
#include "Adventure.h"

CAI_RoadStanding::CAI_RoadStanding()
	: CBTNode{},
	m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CAI_RoadStanding::CAI_RoadStanding(const CAI_RoadStanding& rhs)
	: CBTNode{ rhs },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAI_RoadStanding::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_RoadStanding::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	AI_ADVENTURE_DESC* pDesc = static_cast<AI_ADVENTURE_DESC*>(pArg);
	m_pAnimCom = pDesc->pAnimCom;
	m_pThis = pDesc->pThis;
	m_pState = pDesc->pState;
	m_pAStartCom = pDesc->pAStartCom;
	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	return S_OK;
}

void CAI_RoadStanding::Tick(const _float& fTimeDelta)
{
}

_bool CAI_RoadStanding::isRouteMoveFinish()
{


	return false;
}

CBTNode::NODE_STATE CAI_RoadStanding::Check_Coll()
{
	if (m_pThis->isColl())
	{

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_RoadStanding::Coll()
{

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RoadStanding::Check_Walk()
{

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RoadStanding::Turn()
{
	if (m_iSkill == SKILL_TURN)
	{
		// 현재 look 방향에서 현재 이동 좌표의 각도를 구한다.
		
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_RoadStanding::Walk()
{

	return CBTNode::SUCCESS;
}

void CAI_RoadStanding::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRootNode);
}
