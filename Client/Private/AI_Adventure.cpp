#include "AI_Adventure.h"

#include "GameInstance.h"

#include "Player.h"
#include "Adventure.h"

CAI_Adventure::CAI_Adventure()
	: CBTNode{},
	m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CAI_Adventure::CAI_Adventure(const CAI_Adventure& rhs)
	: CBTNode{ rhs },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAI_Adventure::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Adventure::Initialize(void* pArg)
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

void CAI_Adventure::Tick(const _float& fTimeDelta)
{
}

_bool CAI_Adventure::isRouteMoveFinish()
{


	return false;
}

CBTNode::NODE_STATE CAI_Adventure::Check_Coll()
{
	if (m_pThis->isColl())
	{

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Adventure::Coll()
{

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Adventure::Check_Walk()
{

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Adventure::Turn()
{
	if (m_iSkill == SKILL_TURN)
	{
		// 현재 look 방향에서 현재 이동 좌표의 각도를 구한다.
		
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Adventure::Walk()
{
	if (m_iSkill == SKILL_WALK)
	{
		if (*m_pState == CAdventure::ADVENTURE_WALK_S && m_pAnimCom->Get_AnimFinished())
			*m_pState = CAdventure::ADVENTURE_WALK;

		return CBTNode::RUNNING;
	}

	m_iSkill = SKILL_WALK;
	*m_pState = CAdventure::ADVENTURE_WALK_S;

	return CBTNode::SUCCESS;
}

void CAI_Adventure::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRootNode);
}
