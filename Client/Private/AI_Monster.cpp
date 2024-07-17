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

void CAI_Monster::Check_KRH(_uint iPlayerLv)
{
	if (Find_CurrentAnimationName("p_krh_atk_down")) // �ٿ�Ǿ�����
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DNF_BOUND : CMonster::MONSTER_DWN_DNF_BOUND;
		
	if (Find_CurrentAnimationName("p_krh_atk_heavy_f")) // 
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krh_atk_punch_b"))  //��ġ
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krh_atk_run_heavy")) // ������
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;
		
	if (Find_CurrentAnimationName("p_krh_atk_sp")) //������ 
		*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_CurrentAnimationName("p_krh_atk_standup_kick_dnb")) // �ٿ��
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
	//ATK_Counter �Ѿ����� ����
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
		
	//ATK_Run_Heavy  // ������ ��������
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
		//������
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

	//���� �� �ݰ�
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

	//standUp ��
	if (*m_pState == CMonster::MONSTER_STANDUP_DNF_FAST
		|| *m_pState == CMonster::MONSTER_STANDUP_DNB_FAST)
		m_pThis->Set_Down(false);
		
	return true;
}

CBTNode::NODE_STATE CAI_Monster::Check_Down()
{
	//�ٿ�Ǿ��ִ� ���������� üũ�ؾ���.
	if (m_iSkill != SKILL_DEAD || m_pThis->isDown())
		return CBTNode::SUCCESS;

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::StandUpAndDead()
{
	if (m_pThis->isDown())
	{
		//�ٿ�Ǿ��ִ� �ִϸ��̼� �����ΰ�?
		if (!m_pAnimCom->Get_AnimFinished())
			return CBTNode::RUNNING;

		return CBTNode::SUCCESS;
	}

	if (m_pThis->Get_Info().iHp < 0.f)
	{
		//����
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

//�÷��̾ �������϶� ���� �� �ֵ��� 
CBTNode::NODE_STATE CAI_Monster::Check_Sway()
{
	//�÷��̾ ���� �������ΰ�? && �÷��̾�� �浹���� �ʾҴ°�?
	if (m_pPlayer->isAttack() && !m_pThis->isColl())
	{
		//Ȯ�������� ���ϱ� (80%)
		_uint iRandom = m_pGameInstance->Get_Random(0, 100);
		if (iRandom > 80)
			return CBTNode::SUCCESS;
	}
		
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Sway()
{
	//������ ��� �ֱ�
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
	// �÷��̾ ����� ��� ����� �Ҷ�

	// ������ ������ ��� ��Ʋ ��Ÿ���� ��� �����ϴ�.
	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRH)
		return CBTNode::FAIL;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Check_Hit()
{
	if (m_isGuard)
	{
		//�������� �ϰ�������
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
		//�浹�Ǹ� �÷��̾� �������� �ƴ��� üũ�� Ǯ��
		Reset_State();

		if (m_isGuard)
			m_iGuardAtkCount += 1;
		else
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
//Hit �Լ��� �ϳ��� ���ľ� ��.
CBTNode::NODE_STATE CAI_Monster::Hit()
{
	if (m_iSkill != SKILL_HIT)
		return CBTNode::FAIL;

	_uint iLevel = m_pPlayer->Get_CurrentHitLevel();

#ifdef _DEBUG
	cout << " ��Ʈ!!!!" << endl;
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
	cout << "����!!!!" << endl;
#endif // DEBUG

	//����?
	if (m_iGuardAtk <= m_iGuardAtkCount)
	{
		// Count ���� ������ ���� ���ü����� ������ ���尡 Ǯ��.
		// ���尡 Ǯ���� �ִϸ��̼��� �ִ��� Ȯ���� �ȿ���.
		*m_pState = CMonster::MONSTER_GURAD_FLOAT;
		//m_pThis->Set_Down(true);
		m_iGuardAtkCount = 0; // �ʱ�ȭ

		return CBTNode::FAIL;
	}

	m_isGuard = true;
	m_iSkill = SKILL_GUARD;
	*m_pState = CMonster::MONSTER_GURAD;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Check_Angry()
{
	// �г���� ��ȯ �б�
	if (!m_isAngry)
	{
		_uint iRandom = m_pGameInstance->Get_Random(0, 100);

		//if (iRandom < 30.f)
		//{
		//	// �г� ���·� �̵�
		//	return CBTNode::SUCCESS;
		//}
		//���� ü�� �����϶�, �ǰ� �� �г� ������� �ƴ����� üũ�Ͽ� ���� �Լ����� �г� ���� ��ȯ�Ѵ�.

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
