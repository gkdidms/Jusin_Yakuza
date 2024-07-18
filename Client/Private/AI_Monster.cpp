#include "AI_Monster.h"

#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

#include "Effect.h"

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
	if (m_isAttack == false)
		m_fAttackDelayTime += fTimeDelta;

	if (m_isBreak)
	{
		m_fBreakTime += fTimeDelta;

		if (m_fBreakDuration <= m_fBreakTime)
		{
			m_isBreak = false;
			m_fBreakTime = 0.f;
		}
	}

	if (m_isGuard)
		m_fGuardTime += fTimeDelta;
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

/* 정리 할 예정임 (건들이지마셈) */
//러쉬
_uint CAI_Monster::Check_KRH(_uint iPlayerLv, _bool isBehine, _bool isAnimChange)
{
	_uint iDir = PLAYER_ATK_DIR_END;

	if (Find_CurrentAnimationName("p_krh_atk_down")) // 다운되었을때
	{
		if (isAnimChange)
			*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DNF_BOUND : CMonster::MONSTER_DWN_DNF_BOUND_G;

		iDir = F;
	}


	if (Find_CurrentAnimationName("p_krh_atk_heavy_f")) // 강공격 ?
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F_SP : CMonster::MONSTER_DWN_BODY_F_SP;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B_SP : CMonster::MONSTER_DWN_BODY_B_SP;
		}
			
		iDir = F;
	}

	//if (Find_CurrentAnimationName("p_krh_atk_punch_b"))  //펀치
	//	*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krh_atk_run_heavy")) // 어퍼컷
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B;
		}

		iDir = F;
	}

	if (Find_CurrentAnimationName("p_krh_atk_sp")) //발차기 
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F_SP;
			else 
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B_SP;
		}
			
		iDir = F;
	}

	if (Find_CurrentAnimationName("p_krh_atk_standup_kick_dnb")) // 다운백
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F_SP;
			else 
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B_SP;
		}
			

		iDir = F;
	}

	if (Find_CurrentAnimationName("p_krh_atk_standup_kick_dnf"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F_SP;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B_SP;
		}
			
		iDir = F;
	}

	if (Find_CurrentAnimationName("p_krh_cmb_01"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_F : CMonster::MONSTER_DAM_HEAD_LV02_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;
		}

		iDir = F;
	}

	if (Find_CurrentAnimationName("p_krh_cmb_02"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_L : CMonster::MONSTER_DAM_HEAD_LV02_L;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;
		}
			
		iDir = L;
	}

	if (Find_CurrentAnimationName("p_krh_cmb_03"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_R : CMonster::MONSTER_DAM_HEAD_LV02_R;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;
		}
			
		
		iDir = R;
	}
		
	if (Find_CurrentAnimationName("p_krh_cmb_04"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;
		}
			
		iDir = F;
	}

	if (Find_CurrentAnimationName("p_krh_cmb_05"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_F : CMonster::MONSTER_DAM_HEAD_LV02_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;
		}
			

		iDir = F;
	}
	
	if (Find_CurrentAnimationName("p_krh_cmb_06"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_D : CMonster::MONSTER_DAM_BODY_LV02_D;
			else 
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;
		}
			
		iDir = F;
	}
		
	if (Find_CurrentAnimationName("p_krh_cmb_07"))
	{
		if (isAnimChange)
		{
			m_pThis->Set_Down(true);
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DIRECT_B : CMonster::MONSTER_DWN_DIRECT_B;
			else 
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DIRECT_F : CMonster::MONSTER_DWN_DIRECT_F;
		}

		iDir = F;
	}	

	if (Find_CurrentAnimationName("p_krh_cmb_08"))
	{
		if (isAnimChange)
		{
			m_pThis->Set_Down(true);
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F_SP;
			else 
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B_SP;
		}

		iDir = F;
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
		if (isAnimChange)
		{
			m_pThis->Set_Down(true);
			if (!isBehine)
				*m_pState = CMonster::MONSTER_DWN_BODY_F_SP;
			else
				*m_pState = CMonster::MONSTER_DWN_BODY_B_SP;
		}

		iDir = F;
	}
		
	return iDir;
}

_uint CAI_Monster::Check_KRS(_uint iPlayerLv, _bool isBehine, _bool isAnimChange)
{
	_uint iDir = PLAYER_ATK_DIR_END;
	//ATK_Counter 넘어질때 밟음
	if (Find_CurrentAnimationName("p_krs_atk_down_trample"))
	{
		if (isAnimChange)
			*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DNF_BOUND : CMonster::MONSTER_DWN_DNF_BOUND_G;
			
		iDir = F;
	}
		
	//ATK_Kick_f
	if (Find_CurrentAnimationName("p_krs_atk_kick_f"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_L : CMonster::MONSTER_DAM_HEAD_LV01_L;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV01_B;
		}
			
		iDir = F;
	}

	//ATK_Kick_f_2
	if (Find_CurrentAnimationName("p_krs_atk_kick_f_02"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_F : CMonster::MONSTER_DAM_HEAD_LV02_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;
		}
			
		iDir = F;
	}
		
	//ATK_Kick_f_3
	if (Find_CurrentAnimationName("p_krs_atk_kick_f_03"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B;
		}

		iDir = F;
	}
				
	//ATK_Run_Heavy  // 기합의 날라차기
	if (Find_CurrentAnimationName("p_krs_atk_run_heavy"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_EXPLODE_F : CMonster::MONSTER_DWN_EXPLODE_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_EXPLODE_B : CMonster::MONSTER_DWN_EXPLODE_B;
		}
			
		iDir = F;
	}	

	//TAK_Tame_LOOP
	//if (Find_CurrentAnimationName("p_krs_atk_tame_en"))
	//	*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krs_cmb_01"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_L : CMonster::MONSTER_DAM_HEAD_LV02_L;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;
		}
			
		iDir = L;
	}
		
	if (Find_CurrentAnimationName("p_krs_cmb_02"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_R : CMonster::MONSTER_DAM_HEAD_LV02_R;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;
		}

		iDir = R;
	}
		
	if (Find_CurrentAnimationName("p_krs_cmb_03"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_L : CMonster::MONSTER_DAM_HEAD_LV02_L;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;
		}

		iDir = L;
	}	

	if (Find_CurrentAnimationName("p_krs_cmb_04"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_R : CMonster::MONSTER_DAM_HEAD_LV02_R;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;
		}
			
		iDir = R;
	}
	
	if (Find_CurrentAnimationName("p_krs_cmb_01_fin")
		|| Find_CurrentAnimationName("p_krs_cmb_02_fin")
		|| Find_CurrentAnimationName("p_krs_cmb_03_fin")
		|| Find_CurrentAnimationName("p_krs_cmb_04_fin")
		|| Find_CurrentAnimationName("p_krs_sync_cmb_03_fin"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B;
		}
			
		iDir = F;
	}

	if (Find_CurrentAnimationName("p_krs_cmb_04_finw"))
	{
		//강공격
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F_SP : CMonster::MONSTER_DWN_BODY_F_SP;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B_SP : CMonster::MONSTER_DWN_BODY_B_SP;
		}
			
		iDir = F;
	}

	if (*m_pState == CMonster::MONSTER_DWN_BODY_F ||
		*m_pState == CMonster::MONSTER_DWN_BODY_F_SP ||
		*m_pState == CMonster::MONSTER_DWN_EXPLODE_F)
	{
		m_pThis->Set_Down(true);
	}
		

	return iDir;
}

_uint CAI_Monster::Check_KRC(_uint iPlayerLv, _bool isBehine, _bool isAnimChange)
{
	_uint iDir = PLAYER_ATK_DIR_END;

	if (Find_CurrentAnimationName("p_krc_atk_down_trample"))
	{
		if (isAnimChange)
			*m_pState = CMonster::MONSTER_DWN_DNF_BOUND_G;

			
		iDir = F;
	}

	//가드 후 반격
	if (Find_CurrentAnimationName("p_krc_atk_guard_heavy"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = CMonster::MONSTER_DWN_DIRECT_F_BOUND_G;
			else
				*m_pState = CMonster::MONSTER_DWN_DIRECT_B_BOUND_G;
		}

		iDir = F;
	}

	if (Find_CurrentAnimationName("p_krc_atk_guard_tame"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = CMonster::MONSTER_DWN_DIRECT_F_BOUND_G;
			else
				*m_pState = CMonster::MONSTER_DWN_DIRECT_B_BOUND_G;
		}

		iDir = F;
	}	

	if (Find_CurrentAnimationName("p_krc_atk_tackle_f"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = CMonster::MONSTER_DWN_DIRECT_F_BOUND_G;
			else 
				*m_pState = CMonster::MONSTER_DWN_DIRECT_B_BOUND_G;
		}

		iDir = F;
	}

	//if (Find_CurrentAnimationName("p_krc_atk_tame_tackle_en"))
	//	*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krc_cmb_01"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_L;
			else
				*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_B;
		}

		iDir = L;
	}

	if (Find_CurrentAnimationName("p_krc_cmb_02"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_R;
			else
				*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_B;
		}

		iDir = R;
	}

	if (Find_CurrentAnimationName("p_krc_cmb_03"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_B;
		}

		iDir = B;
	}

	if (Find_CurrentAnimationName("p_krc_cmb_01_fin")
		|| Find_CurrentAnimationName("p_krc_cmb_02_fin")
		|| Find_CurrentAnimationName("p_krc_cmb_03_fin"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = CMonster::MONSTER_DWN_BODY_F_SP;
			else
				*m_pState = CMonster::MONSTER_DWN_BODY_B_SP;
		}

		iDir = F;
	}
	
	return iDir;
}

_bool CAI_Monster::Check_StandUp()
{
	if (*m_pState == CMonster::MONSTER_DWN_EXPLODE_F
		|| *m_pState == CMonster::MONSTER_DWN_BODY_F
		|| *m_pState == CMonster::MONSTER_DWN_BODY_F_SP
		|| *m_pState == CMonster::MONSTER_DWN_DIRECT_F_BOUND_G
		|| *m_pState == CMonster::MONSTER_DWN_DIRECT_F)
	{
		
		*m_pState = CMonster::MONSTER_STANDUP_DNF_FAST;
		return false;
	}

	if (*m_pState == CMonster::MONSTER_DWN_DIRECT_B
		|| *m_pState == CMonster::MONSTER_DWN_DIRECT_B_BOUND_G
		|| *m_pState == CMonster::MONSTER_DWN_BODY_B
		|| *m_pState == CMonster::MONSTER_DWN_BODY_B_SP)
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
	//객체가 죽었는가? 
	//다운상태인가?
	if (m_iSkill == SKILL_DEAD)
	{
		return CBTNode::RUNNING;
	}
	
	if (m_pThis->isObjectDead() || m_pThis->isDown())
		return CBTNode::SUCCESS;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::StandUpAndDead()
{
	if (m_pThis->isObjectDead())
		return CBTNode::SUCCESS;

	if (m_pThis->isDown())
	{
		//다운되어있는 애니메이션 상태인가?
		if (!m_pAnimCom->Get_AnimFinished())
			return CBTNode::RUNNING;

		return CBTNode::SUCCESS;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::StandUp()
{
	if (m_pThis->isObjectDead())
		return CBTNode::FAIL;

	if (Check_StandUp() == true)
		return CBTNode::SUCCESS;
	
	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::Dead()
{
	_uint iDir = { PLAYER_ATK_DIR_END };
	_uint iPlayerLevel = m_pPlayer->Get_CurrentHitLevel();
	_bool isBehine = this->isBehine();

	m_iSkill = SKILL_DEAD;

	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRH)
		iDir = Check_KRH(iPlayerLevel, isBehine);
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRS)
		iDir = Check_KRS(iPlayerLevel, isBehine);
	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRC)
		iDir = Check_KRC(iPlayerLevel, isBehine);

	if (iDir == PLAYER_ATK_DIR_END)
		return CBTNode::FAIL;

	if (!m_pThis->isDown())
	{
		if (iDir == F)
			*m_pState = CMonster::MONSTER_DED_F_2;
		else if (iDir == B)
			*m_pState = CMonster::MONSTER_DED_B_2;
		else if (iDir == L)
			*m_pState = CMonster::MONSTER_DED_L;
		else if (iDir == R)
			*m_pState = CMonster::MONSTER_DED_R;
	} 

	return CBTNode::SUCCESS;
}

//플레이어가 공격중일때 피할 수 있도록 
CBTNode::NODE_STATE CAI_Monster::Check_Sway()
{
	if (m_isGuard || m_iSkill == SKILL_HIT)
		return CBTNode::FAIL;

	if (m_isSway)
	{
		if (m_pAnimCom->Get_AnimFinished())
		{
			m_isSway = false;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	//플래이어가 현재 공격중인가? && 플레이어와 충돌하지 않았는가?
	if (m_pPlayer->isAttack() && !m_pThis->isColl())
	{
		//플레이어와의 거리가 어느정도 있는 상태여야만 함
		if (DistanceFromPlayer() >= 1.5f && DistanceFromPlayer() < 1.8f)
		{
			Reset_State();
			return CBTNode::SUCCESS;		
		}
	}
		
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Sway()
{
	_uint iPlayerAtkDir = PLAYER_ATK_DIR_END;
	_uint iPlayerLv = m_pPlayer->Get_CurrentHitLevel();

	//플레이어가 몬스터의 뒤에 있을때 스웨이를 하지 않는다.
	if (isBehine())
		return CBTNode::FAIL;

	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRS)
		iPlayerAtkDir = Check_KRS(iPlayerLv, false);
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRH)
		iPlayerAtkDir = Check_KRH(iPlayerLv, false);
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRC)
		iPlayerAtkDir = Check_KRC(iPlayerLv, false);

	if (iPlayerAtkDir == PLAYER_ATK_DIR_END)
		return CBTNode::FAIL;

	m_isSway = true;

	Reset_State();

	//스웨이 모션 넣기
	//F
	if (iPlayerAtkDir == F)
		*m_pState = CMonster::MONSTER_SWAY_B;
	//B
		if (iPlayerAtkDir == B)
			*m_pState = CMonster::MONSTER_SWAY_F;
		//L
		if (iPlayerAtkDir == L)
			*m_pState = CMonster::MONSTER_SWAY_R;
		//R
		if (iPlayerAtkDir == R)
			*m_pState = CMonster::MONSTER_SWAY_L;

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

	// 러쉬를 제외한 모든 배틀 스타일이 사용 가능하다.
	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRH)
		return CBTNode::FAIL;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Check_Hit()
{

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::HitAndGuard()
{
	if (m_isGuard)
	{
		if (m_pThis->isColl())
		{
			//가드 상태에서 충돌을 했다면 데미지 누적
			CEffect::EFFECT_DESC EffectDesc;

			EffectDesc.pWorldMatrix = m_pThis->Get_TransformCom()->Get_WorldFloat4x4();

			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_GuardBlink"), TEXT("Layer_Particle"), &EffectDesc);
			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_GuardParticle"), TEXT("Layer_Particle"), &EffectDesc);
			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_GuardSmoke"), TEXT("Layer_Particle"), &EffectDesc);
			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part0"), TEXT("Layer_Particle"), &EffectDesc);

			m_fGuardAtkAcc += m_pThis->Get_HitDamage();

		}

		return CBTNode::SUCCESS;
	}

	if (m_pThis->isColl())
	{
		//충돌되면 플레이어 공격인지 아닌지 체크가 풀림
		Reset_State();

		if (!m_isGuard)
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
CBTNode::NODE_STATE CAI_Monster::Hit()
{
	_uint iLevel = m_pPlayer->Get_CurrentHitLevel();

	_bool isBehine = this->isBehine();
	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRS)
	{
		Check_KRS(iLevel, isBehine);
	}
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRH)
	{
		Check_KRH(iLevel, isBehine);
	}
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRC)
	{
		Check_KRC(iLevel, isBehine);
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Guard()
{
	if (m_iSkill != SKILL_GUARD)
		return CBTNode::FAIL;

	if (m_isGuard)
	{

		if (*m_pState == CMonster::MONSTER_GURAD_START && m_pAnimCom->Get_AnimFinished())
		{
			//가드 지속시간 끝
			*m_pState = CMonster::MONSTER_GURAD_LOOP;
		}

		//가드 후 데미지가 일정 수준을 넘게 된다면
		if (m_fGuardBroken <= m_fGuardAtkAcc)
		{
			*m_pState = CMonster::MONSTER_GURAD_FLOAT;
		}

		//가드 시간이 끝난다면
		if (*m_pState == CMonster::MONSTER_GURAD_LOOP && m_fGuardDuration <= m_fGuardTime)
		{
			*m_pState = CMonster::MONSTER_GURAD_END;
		}

		//최종 가드 마무리
		if ((*m_pState == CMonster::MONSTER_GURAD_END 
			|| *m_pState == CMonster::MONSTER_GURAD_FLOAT)
			&& m_pAnimCom->Get_AnimFinished())
		{
			m_isGuard = false; 
			m_fGuardTime = 0.f;
			m_fGuardAtkAcc = 0.f;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	//첫 진입 시
	if (m_isGuard == false && m_iSkill == SKILL_GUARD)
	{
		m_isGuard = true;
		m_iSkill = SKILL_GUARD;
		*m_pState = CMonster::MONSTER_GURAD_START;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Check_Angry()
{
	// 분노상태 전환 분기
	if (!m_isAngry)
	{
		_uint iRandom = m_pGameInstance->Get_Random(0, 100);

		if (iRandom < 30.f)
		{
			// 분노 상태로 이동
			return CBTNode::SUCCESS;
		}
		//일정 데미지 이상을 받았을때 분노모드

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
	if (iCount == 4)
		m_iSkill = SKILL_SHIFT;
	else 
		m_iSkill = SKILL_IDLE;
	
	if (iCount >= 10)
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
