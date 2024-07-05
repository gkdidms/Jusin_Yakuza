#include "AI_RushYakuza.h"

#include "GameInstance.h"

#include "LeafNode.h"
#include "Selector.h"
#include "Sequance.h"

#include "RushYakuza.h"
#include "Player.h"

CAI_RushYakuza::CAI_RushYakuza()
	: CAI_Monster{}
{
}

HRESULT CAI_RushYakuza::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	AI_OFFICE_YAKUZA_DESC* pDesc = static_cast<AI_OFFICE_YAKUZA_DESC*>(pArg);
	m_pModelCom = pDesc->pModel;
	Safe_AddRef(m_pModelCom);


	//Ʈ�� ����
	Ready_Tree();
	
	return S_OK;
}

void CAI_RushYakuza::Tick(const _float& fTimeDelta)
{
	if (m_isAttack == false)
		m_fAttackDelayTime += fTimeDelta;

	if (m_iSkill == SKILL_SHIFT)
		m_fShiftTime += fTimeDelta;

	this->Execute();
}

CBTNode::NODE_STATE CAI_RushYakuza::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_RushYakuza::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();
	
#pragma region Death
	CSequance* pDeadSeq = CSequance::Create();
	pDeadSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Death, this)));
#pragma endregion

#pragma region Sway
	CSequance* pSwaySeq = CSequance::Create();
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Sway, this)));
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Sway, this)));
#pragma endregion

#pragma region Sync
	CSequance* pSyncSeq = CSequance::Create();
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Chcek_Sync, this)));
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Sync_Neck, this)));
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pHitGuardSeq = CSequance::Create();
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Hit, this)));
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::HitAndGuard, this)));

	CSelector* pHitGuard = CSelector::Create();
	CSelector* pHitSelector = CSelector::Create();
	pHitSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Normal_Hit, this)));
	pHitSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Strong_Hit, this)));

	CSelector* pGuardSelector = CSelector::Create();
	pGuardSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Guard, this)));

	pHitGuard->Add_Children(pHitSelector);
	pHitGuard->Add_Children(pGuardSelector);

	pHitGuardSeq->Add_Children(pHitGuard);
#pragma endregion

#pragma region Angry
	CSequance* pAngrySeq = CSequance::Create();
	pAngrySeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Angry, this)));
	pAngrySeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Angry, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Angry_Attack, this)));

	CSelector* pAttackSelector = CSelector::Create();
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::ATK_Punch, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::ATK_CMD, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::ATK_Angry_Punch, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::ATK_Angry_Kick, this)));

	pAttackSeq->Add_Children(pAttackSelector);
#pragma endregion

#pragma region Shift
	CSequance* pShiftSeq = CSequance::Create();
	pShiftSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Shift, this)));
	pShiftSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Shift, this)));
#pragma endregion

#pragma region Idle
	CSequance* pIdleSeq = CSequance::Create();
	pIdleSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Idle, this)));
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pDeadSeq);
	pRoot->Add_Children(pSwaySeq);
	pRoot->Add_Children(pSyncSeq);
	pRoot->Add_Children(pHitGuardSeq);
	pRoot->Add_Children(pAngrySeq);
	pRoot->Add_Children(pAttackSeq);
	pRoot->Add_Children(pShiftSeq);
	pRoot->Add_Children(pIdleSeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_RushYakuza::Check_Angry()
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

		return CBTNode::FAIL;
	}

	//ȭ�� ���ִ� ���¿��� �ִϸ��̼��� �������ΰ�?
	if (*m_pState == CMonster::MONSTER_ANGRY_START && m_pAnimCom->Get_AnimFinished())
		return CBTNode::FAIL;
	else if (*m_pState == CMonster::MONSTER_ANGRY_START) return CBTNode::RUNNING;

	//�̹� ȭ�� �� �ִٸ� �ٷ� ���� ������� �̵�
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_RushYakuza::Angry()
{
	m_isAngry = true;
	*m_pState = CMonster::MONSTER_ANGRY_START;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RushYakuza::Check_Attack()
{
	if (m_isAttack == false)
	{
		if (m_fDelayAttackDuration >= m_fAttackDelayTime)
			return CBTNode::FAIL;

		m_fAttackDelayTime = 0.f; // �ʱ�ȭ
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RushYakuza::Attack()
{
	if (m_isAngry) return CBTNode::SUCCESS;

	//ȭ�� ���� �ʾ����� ��ų ���� (�ӽ�)
	static _uint iCount = 0;

	if (iCount == 0 || iCount == 1)
		m_iSkill = SKILL_CMD;
	else
		m_iSkill = SKILL_PUNCH;

	iCount++;
	if (iCount >= 3)
		iCount = 0;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RushYakuza::Angry_Attack()
{
	if (!m_isAngry) return CBTNode::SUCCESS;

	//ȭ������ ��ų ���� (�ӽ�)
	static _uint iCount = 0;

	if (iCount == 0 || iCount == 1)
		m_iSkill = SKILL_ANGRY_KICK;
	else
		m_iSkill = SKILL_ANGRY_CHOP;

	iCount++;

	if (iCount >= 3)
		iCount = 0;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RushYakuza::ATK_Punch()
{
	if (m_iSkill == SKILL_PUNCH && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_PUNCH && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_PUNCH)
	{
		*m_pState = CMonster::MONSTER_PUNCH;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_RushYakuza::ATK_CMD()
{
	if (m_iSkill == SKILL_CMD && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMD_1 && *(m_pAnimCom->Get_AnimPosition()) >= 10.0)
		{
			// �� ������
			*m_pState = CMonster::MONSTER_CMD_2;
		}
		else if (*m_pState == CMonster::MONSTER_CMD_2 && *(m_pAnimCom->Get_AnimPosition()) >= 10.0)
		{
			*m_pState = CMonster::MONSTER_CMD_3;
		}
		else if (*m_pState == CMonster::MONSTER_CMD_3 && *(m_pAnimCom->Get_AnimPosition()) >= 10.0)
		{
			*m_pState = CMonster::MONSTER_CMD_4;
		}
		else if (*m_pState == CMonster::MONSTER_CMD_4 && *(m_pAnimCom->Get_AnimPosition()) >= 10.0)
		{
			*m_pState = CMonster::MONSTER_CMD_5;
		}
		else if (*m_pState == CMonster::MONSTER_CMD_5 && m_pAnimCom->Get_AnimFinished())
		{
			//�޺� ��.
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	//ù ���� ��
	if (m_iSkill == SKILL_CMD)
	{
		*m_pState = CMonster::MONSTER_CMD_1;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_RushYakuza::ATK_Angry_Punch()
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

CBTNode::NODE_STATE CAI_RushYakuza::ATK_Angry_Kick()
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

CBTNode::NODE_STATE CAI_RushYakuza::Check_Shift()
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

CBTNode::NODE_STATE CAI_RushYakuza::Shift()
{
	m_fShiftDuration = m_pGameInstance->Get_Random(1.f, 3.f);

	_uint iIndex = m_pGameInstance->Get_Random(0, 10); // ���� �̵�����
	m_iSkill = SKILL_SHIFT;

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

CBTNode::NODE_STATE CAI_RushYakuza::Idle()
{
	*m_pState = CMonster::MONSTER_IDLE;

	return CBTNode::SUCCESS;
}

CAI_RushYakuza* CAI_RushYakuza::Create(void* pArg)
{
	CAI_RushYakuza* pInstance = new CAI_RushYakuza();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_RushYakuza::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
}
