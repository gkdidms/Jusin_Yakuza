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

//�÷��̾���� �Ÿ�
_float CAI_Monster::DistanceFromPlayer()
{
	_vector vPlayerPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vThisPos = m_pThis->m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDistance = XMVector3Length(vPlayerPos - vThisPos).m128_f32[0]; // �Ÿ� ����	

	return fDistance;
}

_bool CAI_Monster::isBehine()
{
	//���͸� �������� �÷��̾ �ڿ� �����ϴ��� Ȯ���ϱ�.
	_vector vThisLook = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	_vector vThisPos = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	_vector vToPlayer = XMVector3Normalize(vPlayerPos - vThisPos);
	
	_float fDot = XMVector3Dot(vThisLook, vToPlayer).m128_f32[0];
	_float fAngle = acos(fDot);
	
	//�÷��̾��� �þ߰��� 180���� �ΰ� ����Ѵ�.
	//�ڿ� �ִ��� ������ üũ.
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

/* ���� �� ������ (�ǵ���������) */
//����
_uint CAI_Monster::Check_KRH(_uint iPlayerLv, _bool isBehine, _bool isAnimChange)
{
	_uint iDir = PLAYER_ATK_DIR_END;

	if (Find_CurrentAnimationName("p_krh_atk_down")) // �ٿ�Ǿ�����
	{
		if (isAnimChange)
			*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DNF_BOUND : CMonster::MONSTER_DWN_DNF_BOUND_G;

		iDir = F;
	}


	if (Find_CurrentAnimationName("p_krh_atk_heavy_f")) // ������ ?
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

	//if (Find_CurrentAnimationName("p_krh_atk_punch_b"))  //��ġ
	//	*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krh_atk_run_heavy")) // ������
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

	if (Find_CurrentAnimationName("p_krh_atk_sp")) //������ 
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

	if (Find_CurrentAnimationName("p_krh_atk_standup_kick_dnb")) // �ٿ��
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
	//ATK_Counter �Ѿ����� ����
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
				
	//ATK_Run_Heavy  // ������ ��������
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
		//������
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

	//���� �� �ݰ�
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

	//standUp ��
	if (*m_pState == CMonster::MONSTER_STANDUP_DNF_FAST
		|| *m_pState == CMonster::MONSTER_STANDUP_DNB_FAST)
		m_pThis->Set_Down(false);
		
	return true;
}

CBTNode::NODE_STATE CAI_Monster::Check_Down()
{
	//��ü�� �׾��°�? 
	//�ٿ�����ΰ�?
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
		//�ٿ�Ǿ��ִ� �ִϸ��̼� �����ΰ�?
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

//�÷��̾ �������϶� ���� �� �ֵ��� 
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

	//�÷��̾ ���� �������ΰ�? && �÷��̾�� �浹���� �ʾҴ°�?
	if (m_pPlayer->isAttack() && !m_pThis->isColl())
	{
		//�÷��̾���� �Ÿ��� ������� �ִ� ���¿��߸� ��
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

	//�÷��̾ ������ �ڿ� ������ �����̸� ���� �ʴ´�.
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

	//������ ��� �ֱ�
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
	// �÷��̾ ����� ��� ����� �Ҷ�

	// ������ ������ ��� ��Ʋ ��Ÿ���� ��� �����ϴ�.
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
			//���� ���¿��� �浹�� �ߴٸ� ������ ����
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
		//�浹�Ǹ� �÷��̾� �������� �ƴ��� üũ�� Ǯ��
		Reset_State();

		if (!m_isGuard)
		{
			//Hit üũ�ϰ� ���带 �� ������, Hit�� ������?
			//�������� ó���ϱ� (3 Ȯ���� ����)
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
		//�浹���� ���� ���¿��� ��Ʈ ����� ������?
		if (m_iSkill == SKILL_HIT && !m_pAnimCom->Get_AnimFinished())
			return CBTNode::RUNNING;
	}
	
	return CBTNode::FAIL;
}

//KRS: ���Ѵ�, KRH: ����, KRC: �ı���
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
			//���� ���ӽð� ��
			*m_pState = CMonster::MONSTER_GURAD_LOOP;
		}

		//���� �� �������� ���� ������ �Ѱ� �ȴٸ�
		if (m_fGuardBroken <= m_fGuardAtkAcc)
		{
			*m_pState = CMonster::MONSTER_GURAD_FLOAT;
		}

		//���� �ð��� �����ٸ�
		if (*m_pState == CMonster::MONSTER_GURAD_LOOP && m_fGuardDuration <= m_fGuardTime)
		{
			*m_pState = CMonster::MONSTER_GURAD_END;
		}

		//���� ���� ������
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

	//ù ���� ��
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
	// �г���� ��ȯ �б�
	if (!m_isAngry)
	{
		_uint iRandom = m_pGameInstance->Get_Random(0, 100);

		if (iRandom < 30.f)
		{
			// �г� ���·� �̵�
			return CBTNode::SUCCESS;
		}
		//���� ������ �̻��� �޾����� �г���

		return CBTNode::FAIL;
	}

	//ȭ�� ���ִ� ���¿��� �ִϸ��̼��� �������ΰ�?
	if (*m_pState == CMonster::MONSTER_ANGRY_START && m_pAnimCom->Get_AnimFinished())
		return CBTNode::FAIL;
	else if (*m_pState == CMonster::MONSTER_ANGRY_START) return CBTNode::RUNNING;

	//�̹� ȭ�� �� �ִٸ� �ٷ� ���� ������� �̵�
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
