#include "AI_Monster.h"

#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

CAI_Monster::CAI_Monster()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

CAI_Monster::CAI_Monster(const CAI_Monster& rhs)
	: m_pGameInstance { rhs.m_pGameInstance}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAI_Monster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Monster::Initialize(void* pArg)
{
	AI_MONSTER_DESC* pDesc = static_cast<AI_MONSTER_DESC*>(pArg);
	m_pState = pDesc->pState;
	m_pAnimCom = pDesc->pAnim;
	m_pThis = pDesc->pThis;

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	return S_OK;
}

void CAI_Monster::Tick(const _float& fTimeDelta)
{
}

void CAI_Monster::Ready_Tree()
{
}

//플레이어와의 거리
_float CAI_Monster::DistanceFromPlayer()
{
	_vector vPlayerPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vThisPos = m_pThis->m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDistance = XMVector3Length(vPlayerPos - vThisPos).m128_f32[0]; // 거리 측정	

	return fDistance;
}

_bool CAI_Monster::isBehine()
{
	//몬스터를 기준으로 플레이어가 뒤에 존재하는지 확인하기.
	_vector vThisLook = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	_vector vThisPos = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	_vector vToPlayer = XMVector3Normalize(vPlayerPos - vThisPos);
	
	_float fDot = XMVector3Dot(vThisLook, vToPlayer).m128_f32[0];
	_float fAngle = acos(fDot);
	
	//플레이어의 시야각을 180도로 두고 계산한다.
	//뒤에 있는지 없는지 체크.
	return  fAngle > (XMConvertToRadians(180.f) * 0.5f);
}

void CAI_Monster::LookAtPlayer()
{
	_vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	m_pThis->Get_TransformCom()->LookAt(vPlayerPos);
}

_bool CAI_Monster::Find_CurrentAnimationName(string strAnimName)
{
	string strPlayerAnimName = m_pPlayer->Get_CurrentAnimationName();
	if (strPlayerAnimName.find(strAnimName) == string::npos)
		return false;

	return true;
}

void CAI_Monster::Check_KRH(_uint iPlayerLv)
{
	if (Find_CurrentAnimationName("p_krh_atk_down")) // 다운되었을때
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DNF_BOUND : CMonster::MONSTER_DWN_DNF_BOUND;
		
	if (Find_CurrentAnimationName("p_krh_atk_heavy_f")) // 
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krh_atk_punch_b"))  //펀치
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krh_atk_run_heavy")) // 어퍼컷
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;
		
	if (Find_CurrentAnimationName("p_krh_atk_sp")) //발차기 
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krh_atk_standup_kick_dnb")) // 다운백
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krh_atk_standup_kick_dnf")) // 
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krh_cmb_01"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV02_F : CMonster::MONSTER_DAM_HEAD_LV02_F;

	if (Find_CurrentAnimationName("p_krh_cmb_02"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV02_L : CMonster::MONSTER_DAM_HEAD_LV02_L;

	if (Find_CurrentAnimationName("p_krh_cmb_03"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV02_R : CMonster::MONSTER_DAM_HEAD_LV02_R;

	if (Find_CurrentAnimationName("p_krh_cmb_04"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV02_F : CMonster::MONSTER_DAM_BODY_LV02_F;

	if (Find_CurrentAnimationName("p_krh_cmb_05"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV02_F : CMonster::MONSTER_DAM_HEAD_LV02_F;

	if (Find_CurrentAnimationName("p_krh_cmb_06"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV02_D : CMonster::MONSTER_DAM_BODY_LV02_D;

	if (Find_CurrentAnimationName("p_krh_cmb_07"))
	{
		m_pThis->Set_Down(true);
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DIRECT_D : CMonster::MONSTER_DWN_DIRECT_D;
	}
		

	if (Find_CurrentAnimationName("p_krh_cmb_08"))
	{
		m_pThis->Set_Down(true);
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;
	}
		

	if (Find_CurrentAnimationName("p_krh_cmb_01_fin")
		|| Find_CurrentAnimationName("p_krh_cmb_02_fin")
		|| Find_CurrentAnimationName("p_krh_cmb_03_fin")
		|| Find_CurrentAnimationName("p_krh_cmb_05_fin")
		|| Find_CurrentAnimationName("p_krh_cmb_05_fin")
		|| Find_CurrentAnimationName("p_krh_cmb_06_fin")
		|| Find_CurrentAnimationName("p_krh_cmb_07_fin")
		|| Find_CurrentAnimationName("p_krh_cmb_08_fin"))
	{
		m_pThis->Set_Down(true);
		*m_pState = CMonster::MONSTER_DWN_BODY_F;
	}
		
}

void CAI_Monster::Check_KRS(_uint iPlayerLv)
{
	//ATK_Counter 넘어질때 밟음
	if (Find_CurrentAnimationName("p_krs_atk_down_trample"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DNF_BOUND : CMonster::MONSTER_DWN_DNF_BOUND;

	//ATK_Kick_f
	if (Find_CurrentAnimationName("p_krs_atk_kick_f"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_L : CMonster::MONSTER_DAM_HEAD_LV01_L;

	//ATK_Kick_f_2
	if (Find_CurrentAnimationName("p_krs_atk_kick_f_02"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_F : CMonster::MONSTER_DAM_HEAD_LV02_F;
		
	//ATK_Kick_f_3
	if (Find_CurrentAnimationName("p_krs_atk_kick_f_03"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;
		
	//ATK_Run_Heavy  // 기합의 날라차기
	if (Find_CurrentAnimationName("p_krs_atk_run_heavy"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_EXPLODE_F : CMonster::MONSTER_DWN_EXPLODE_F;

	//TAK_Tame_LOOP
	//if (Find_CurrentAnimationName("p_krs_atk_tame_en"))
	//	*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krs_cmb_01"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_L : CMonster::MONSTER_DAM_HEAD_LV02_L;

	if (Find_CurrentAnimationName("p_krs_cmb_02"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_R : CMonster::MONSTER_DAM_HEAD_LV02_R;

	if (Find_CurrentAnimationName("p_krs_cmb_03"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_L : CMonster::MONSTER_DAM_HEAD_LV02_L;

	if (Find_CurrentAnimationName("p_krs_cmb_04"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_R : CMonster::MONSTER_DAM_HEAD_LV02_R;

	if (Find_CurrentAnimationName("p_krs_cmb_01_fin"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;

	if (Find_CurrentAnimationName("p_krs_cmb_02_fin"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;

	if (Find_CurrentAnimationName("p_krs_cmb_03_fin"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;

	if (Find_CurrentAnimationName("p_krs_cmb_04_fin"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;

	if (Find_CurrentAnimationName("p_krs_sync_cmb_03_fin"))
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;

	if (Find_CurrentAnimationName("p_krs_cmb_04_finw"))
	{
		//강공격
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F_SP : CMonster::MONSTER_DWN_BODY_F_SP;
	}

	if (*m_pState == CMonster::MONSTER_DWN_BODY_F ||
		*m_pState == CMonster::MONSTER_DWN_BODY_F_SP ||
		*m_pState == CMonster::MONSTER_DWN_EXPLODE_F )
		m_pThis->Set_Down(true);
}

void CAI_Monster::Check_KRC(_uint iPlayerLv)
{
	if (Find_CurrentAnimationName("p_krc_atk_down_trample"))
		*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	//가드 후 반격
	if (Find_CurrentAnimationName("p_krc_atk_guard_heavy"))
		*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krc_atk_guard_tame"))
		*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krc_atk_tackle_f"))
		*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krc_atk_tame_tackle_en"))
		*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krc_cmb_01"))
		*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krc_cmb_02"))
		*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krc_cmb_03"))
		*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krc_cmb_01_fin"))
		*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krc_cmb_02_fin"))
		*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krc_cmb_03_fin"))
		*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

}

_bool CAI_Monster::Check_StandUp()
{
	if (*m_pState == CMonster::MONSTER_DWN_EXPLODE_F
		|| *m_pState == CMonster::MONSTER_DWN_BODY_F
		|| *m_pState == CMonster::MONSTER_DWN_BODY_F_SP)
	{
		*m_pState = CMonster::MONSTER_STANDUP_DNF_FAST;
		return false;
	}

	if (*m_pState == CMonster::MONSTER_DWN_DIRECT_D)
	{
		*m_pState = CMonster::MONSTER_STANDUP_DNB_FAST;
		return false;
	}

	//standUp 끝
	if (*m_pState == CMonster::MONSTER_STANDUP_DNF_FAST
		|| *m_pState == CMonster::MONSTER_STANDUP_DNB_FAST)
		m_pThis->Set_Down(false);
		
	return true;
}

CBTNode::NODE_STATE CAI_Monster::Check_Down()
{
	//다운되어있는 상태인지도 체크해야함.
	if (m_iSkill != SKILL_DEAD || m_pThis->isDown())
		return CBTNode::SUCCESS;

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::StandUpAndDead()
{
	if (m_pThis->isDown())
	{
		//다운되어있는 애니메이션 상태인가?
		if (!m_pAnimCom->Get_AnimFinished())
			return CBTNode::RUNNING;

		return CBTNode::SUCCESS;
	}

	if (m_pThis->Get_Info().iHp < 0.f)
	{
		//죽음
		m_iSkill = SKILL_DEAD;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::StandUp()
{
	if (m_iSkill == SKILL_DEAD)
		return CBTNode::FAIL;

	if (Check_StandUp() == true)
		return CBTNode::SUCCESS;
	
	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::Dead()
{
	*m_pState = CMonster::MONSTER_DEATH;

	return CBTNode::SUCCESS;
}

//플레이어가 공격중일때 피할 수 있도록 
CBTNode::NODE_STATE CAI_Monster::Check_Sway()
{
	//플래이어가 현재 공격중인가? && 플레이어와 충돌하지 않았는가?
	if (m_pPlayer->isAttack() && !m_pThis->isColl())
	{
		//확률적으로 피하기 (80%)
		_uint iRandom = m_pGameInstance->Get_Random(0, 100);
		if (iRandom > 80)
			return CBTNode::SUCCESS;
	}
		
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Sway()
{
	//스웨이 모션 넣기
	//F
	
	//B
	
	//L
	
	//R

	return CBTNode::FAIL;
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

	// 러쉬를 제외한 모든 배틀 스타일이 사용 가능하다.
	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRH)
		return CBTNode::FAIL;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Check_Hit()
{
	if (m_isGuard)
	{
		//가드모션을 하고있을때
		if (*m_pState == CMonster::MONSTER_GURAD_FLOAT)
		{
			if (m_pAnimCom->Get_AnimFinished())
			{
				m_isGuard = false;
				m_iSkill = SKILL_IDLE;
				return CBTNode::SUCCESS;
			}

			return CBTNode::RUNNING;
		}

		if (*m_pState == CMonster::MONSTER_GURAD && !m_pAnimCom->Get_AnimFinished())
		{
			return CBTNode::RUNNING;
		}
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::HitAndGuard()
{
	if (m_pThis->isColl())
	{
		//충돌되면 플레이어 공격인지 아닌지 체크가 풀림
		Reset_State();

		if (m_isGuard)
			m_iGuardAtkCount += 1;
		else
		{
			//Hit 체크하고 가드를 할 것인지, Hit할 것인지?
			//랜덤으로 처리하기 (3 확률로 가드)
			_uint iRandom = m_pGameInstance->Get_Random(0, 100);

			if (iRandom == 95 || iRandom == 40 || iRandom == 67)
				m_iSkill = SKILL_GUARD;
			else
				m_iSkill = SKILL_HIT;
		}

		return CBTNode::SUCCESS;
	}
	else
	{

		//충돌하지 않은 상태에서 히트 모션이 끝나면?
		if (m_iSkill == SKILL_HIT && !m_pAnimCom->Get_AnimFinished())
			return CBTNode::RUNNING;
	}
	
	return CBTNode::FAIL;
}

//KRS: 불한당, KRH: 러쉬, KRC: 파괴자
//Hit 함수를 하나로 합쳐야 함.
CBTNode::NODE_STATE CAI_Monster::Hit()
{
	if (m_iSkill != SKILL_HIT)
		return CBTNode::FAIL;

	_uint iLevel = m_pPlayer->Get_CurrentHitLevel();

#ifdef _DEBUG
	cout << " 히트!!!!" << endl;
#endif // DEBUG

	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRS)
	{
		Check_KRS(iLevel);
	}
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRH)
	{
		Check_KRH(iLevel);
	}
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRC)
	{
		Check_KRC(iLevel);
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Guard()
{
	if (m_iSkill != SKILL_GUARD)
		return CBTNode::FAIL;

#ifdef _DEBUG
	cout << "가드!!!!" << endl;
#endif // DEBUG

	//랜덤?
	if (m_iGuardAtk <= m_iGuardAtkCount)
	{
		// Count 수가 지정된 가드 어택수보다 많으면 가드가 풀림.
		// 가드가 풀리는 애니메이션이 있는지 확인이 팔요함.
		*m_pState = CMonster::MONSTER_GURAD_FLOAT;
		//m_pThis->Set_Down(true);
		m_iGuardAtkCount = 0; // 초기화

		return CBTNode::FAIL;
	}

	m_isGuard = true;
	m_iSkill = SKILL_GUARD;
	*m_pState = CMonster::MONSTER_GURAD;

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

CBTNode::NODE_STATE CAI_Monster::Check_Break()
{
	if (m_isBreak == false) return CBTNode::SUCCESS;

	LookAtPlayer();

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::ShiftAndIdle()
{
	static _uint iCount = 0;

	iCount++;
	if (iCount == 5)
		m_iSkill = SKILL_SHIFT;
	else 
		m_iSkill = SKILL_IDLE;
	
	if (iCount <= 10)
		iCount = 0;

	m_fBreakDuration = m_pGameInstance->Get_Random(2.f, 4.f);
	
	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Shift()
{
	if (m_iSkill != SKILL_SHIFT)
		return CBTNode::FAIL;

	_uint iIndex = m_pGameInstance->Get_Random(0, 3);
	if (iIndex == 0)
		*m_pState = CMonster::MONSTER_SHIFT_F;
	else if (iIndex == 1)
		*m_pState = CMonster::MONSTER_SHIFT_B;
	else if (iIndex == 2)
		*m_pState = CMonster::MONSTER_SHIFT_L;
	else if (iIndex == 3)
		*m_pState = CMonster::MONSTER_SHIFT_R;

	m_isBreak = true;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Idle()
{
	*m_pState = CMonster::MONSTER_IDLE;
	m_isBreak = true;

	return CBTNode::SUCCESS;
}

void CAI_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pRootNode);
}
