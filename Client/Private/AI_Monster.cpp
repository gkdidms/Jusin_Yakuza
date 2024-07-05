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
	// �÷��̾ ����� ��� ����� �Ҷ�
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	// ������ ������ ��� ��Ʋ ��Ÿ���� ��� �����ϴ�.
	if (pPlayer->Get_BattleStyle() == CPlayer::KRH)
		return CBTNode::FAIL;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Check_Hit()
{
	//if (m_iSkill != SKILL_HIT)
	//	return CBTNode::SUCCESS;

	// ��Ʈ����� �ϰ� �ִ°�?


	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::HitAndGuard()
{
	//Hit üũ�ϰ� ���带 �� ������, Hit�� ������?
	//�浹�Ұ��ΰ�?
	return CBTNode::FAIL;
}

//KRS: ���Ѵ�, KRH: ����, KRC: �ı���
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

//KRS: ���Ѵ�, KRH: ����, KRC: �ı���
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
	//����?

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
