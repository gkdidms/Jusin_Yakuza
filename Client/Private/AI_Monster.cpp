#include "AI_Monster.h"

#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

CAI_Monster::CAI_Monster()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAI_Monster::Initialize(void* pArg)
{
	AI_MONSTER_DESC* pDesc = static_cast<AI_MONSTER_DESC*>(pArg);
	m_pState = pDesc->pState;
	m_pAnimCom = pDesc->pAnim;
	Safe_AddRef(m_pAnimCom);
	
	return S_OK;
}

void CAI_Monster::Tick(const _float& fTimeDelta)
{
}

void CAI_Monster::Ready_Tree()
{
}


CBTNode::NODE_STATE CAI_Monster::Check_Death()
{
	if (*m_pState != CMonster::MONSTER_DEATH)
		return CBTNode::FAIL;

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::Check_Sway()
{
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Sway()
{
	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Chcek_Sync()
{
	if (!m_isSync)
		return CBTNode::SUCCESS;

	if (m_pAnimCom->Get_AnimFinished())
		return CBTNode::SUCCESS;

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::Sync_Neck()
{
	// 플레이어가 멱살을 잡는 모션을 할때
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	// 러쉬를 제외한 모든 배틀 스타일이 사용 가능하다.
	if (pPlayer->Get_BattleStyle() == CPlayer::KRH)
		return CBTNode::FAIL;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Check_Hit()
{
	//if (m_iSkill != SKILL_HIT)
	//	return CBTNode::SUCCESS;

	// 히트모션을 하고 있는가?


	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::HitAndGuard()
{
	//Hit 체크하고 가드를 할 것인지, Hit할 것인지?
	//충돌할것인가?
	return CBTNode::FAIL;
}

//KRS: 불한당, KRH: 러쉬, KRC: 파괴자
CBTNode::NODE_STATE CAI_Monster::Normal_Hit()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	if (pPlayer->Get_BattleStyle() == CPlayer::KRS)
	{
		//
	}
	else if (pPlayer->Get_BattleStyle() == CPlayer::KRH)
	{

	}
	else if (pPlayer->Get_BattleStyle() == CPlayer::KRC)
	{

	}

	return CBTNode::SUCCESS;
}

//KRS: 불한당, KRH: 러쉬, KRC: 파괴자
CBTNode::NODE_STATE CAI_Monster::Strong_Hit()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	if (pPlayer->Get_BattleStyle() == CPlayer::KRS)
	{
		//
	}
	else if (pPlayer->Get_BattleStyle() == CPlayer::KRH)
	{
		//
	}
	else if (pPlayer->Get_BattleStyle() == CPlayer::KRC)
	{

	}

	return CBTNode::SUCCESS();
}

CBTNode::NODE_STATE CAI_Monster::Guard()
{
	//랜덤?

	return CBTNode::SUCCESS;
}

void CAI_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pAnimCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRootNode);
}
