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

CBTNode::NODE_STATE CAI_Monster::Check_Angry()
{
	// 분노상태 전환 분기
	if (!m_isAngry)
	{
		_uint iRandom = m_pGameInstance->Get_Random(0, 100);

		//if (iRandom < 30.f)
		//{
		//	// 분노 상태로 이동
		//	return CBTNode::SUCCESS;
		//}
		//일정 체력 이하일때, 피격 후 분노 모드인지 아닌지를 체크하여 현재 함수에서 분노 모드로 전환한다.

		return CBTNode::FAIL;
	}

	//화가 나있는 상태에서 애니메이션이 진행중인가?
	if (*m_pState == CMonster::MONSTER_ANGRY_START && m_pAnimCom->Get_AnimFinished())
		return CBTNode::FAIL;
	else if (*m_pState == CMonster::MONSTER_ANGRY_START) return CBTNode::RUNNING;

	//이미 화가 나 있다면 바로 공격 모션으로 이동
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Angry()
{
	m_isAngry = true;
	*m_pState = CMonster::MONSTER_ANGRY_START;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::ATK_Angry_Punch()
{
	if (m_iSkill == SKILL_ANGRY_CHOP && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_ANGRY_CHOP && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_ANGRY_CHOP)
	{
		*m_pState = CMonster::MONSTER_ANGRY_CHOP;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::ATK_Angry_Kick()
{
	if (m_iSkill == SKILL_ANGRY_KICK && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_ANGRY_KICK && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_ANGRY_KICK)
	{
		*m_pState = CMonster::MONSTER_ANGRY_KICK;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Check_Shift()
{
	if (m_iSkill != SKILL_SHIFT)
		return CBTNode::SUCCESS;

	if (m_fShiftDuration <= m_fShiftTime)
	{
		m_iSkill = SKILL_END;
		m_fShiftTime = 0.f;

		return CBTNode::FAIL;
	}

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::Shift()
{
	m_fShiftDuration = m_pGameInstance->Get_Random(1.f, 3.f);

	static _uint iCount = 0;
	
	if (iCount == 5 || iCount == 7)
	{
		m_iSkill = SKILL_SHIFT;
	}
	else
	{
		iCount++;
		return CBTNode::FAIL;
	}

	_uint iIndex = m_pGameInstance->Get_Random(0, 4);
	if (iIndex == 0)
		*m_pState = CMonster::MONSTER_SHIFT_F;
	else if (iIndex == 1)
		*m_pState = CMonster::MONSTER_SHIFT_B;
	else if (iIndex == 2)
		*m_pState = CMonster::MONSTER_SHIFT_L;
	else if (iIndex == 3)
		*m_pState = CMonster::MONSTER_SHIFT_R;
	else
		return CBTNode::FAIL;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Check_Idle()
{
	if (*m_pState != CMonster::MONSTER_IDLE)
		return CBTNode::SUCCESS;

	if (m_fIdleDuration <= m_fIdleTime)
	{
		m_isIdle = false;
		m_fIdleTime = 0.f;

		return CBTNode::RUNNING;
	}

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::Idle()
{
	if (m_isIdle) return CBTNode::SUCCESS;

	m_fIdleDuration = m_pGameInstance->Get_Random(2.f, 4.f);

	*m_pState = CMonster::MONSTER_IDLE;
	m_isIdle = true;

	return CBTNode::SUCCESS;
}

void CAI_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pAnimCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRootNode);
}
