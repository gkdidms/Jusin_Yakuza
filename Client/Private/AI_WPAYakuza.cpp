#include "AI_WPAYakuza.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

#include "Monster.h"


CAI_WPAYakuza::CAI_WPAYakuza()
{
}

HRESULT CAI_WPAYakuza::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	return S_OK;
}

void CAI_WPAYakuza::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Death
	CSequance* pDeadSeq = CSequance::Create();
	pDeadSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Death, this)));
#pragma endregion

#pragma region Sway
	CSequance* pSwaySeq = CSequance::Create();
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Sway, this)));
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Sway, this)));
#pragma endregion

#pragma region Sync
	CSequance* pSyncSeq = CSequance::Create();
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Chcek_Sync, this)));
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Sync_Neck, this)));
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pHitGuardSeq = CSequance::Create();
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Hit, this)));
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::HitAndGuard, this)));

	CSelector* pHitGuard = CSelector::Create();
	CSelector* pHitSelector = CSelector::Create();
	pHitSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Normal_Hit, this)));
	pHitSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Strong_Hit, this)));

	CSelector* pGuardSelector = CSelector::Create();
	pGuardSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Guard, this)));

	pHitGuard->Add_Children(pHitSelector);
	pHitGuard->Add_Children(pGuardSelector);

	pHitGuardSeq->Add_Children(pHitGuard);
#pragma endregion

#pragma region Angry
	CSequance* pAngrySeq = CSequance::Create();
	pAngrySeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Angry, this)));
	pAngrySeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Angry, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Angry_Attack, this)));

	CSelector* pAttackSelector = CSelector::Create();
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::ATK_Heavy, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::ATK_CMD, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::ATK_Angry_Punch, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::ATK_Angry_Kick, this)));

	pAttackSeq->Add_Children(pAttackSelector);
#pragma endregion

#pragma region Shift
	CSequance* pShiftSeq = CSequance::Create();
	pShiftSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Shift, this)));
	pShiftSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Shift, this)));
#pragma endregion

#pragma region Idle
	CSequance* pIdleSeq = CSequance::Create();
	pIdleSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Idle, this)));
	pIdleSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Idle, this)));
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

CBTNode::NODE_STATE CAI_WPAYakuza::Check_Attack()
{
	if (m_isAttack == false)
	{
		if (m_fDelayAttackDuration >= m_fAttackDelayTime)
			return CBTNode::FAIL;

		m_fAttackDelayTime = 0.f; // �ʱ�ȭ
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_WPAYakuza::Attack()
{
	if (m_isAngry) return CBTNode::SUCCESS;

	//ȭ�� ���� �ʾ����� ��ų ���� (�ӽ�)
	static _uint iCount = 0;

	if (iCount == 0 || iCount == 2)
		m_iSkill = SKILL_CMD;
	else
		m_iSkill = SKILL_HEAVY;

	iCount++;
	if (iCount >= 3)
		iCount = 0;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_WPAYakuza::Angry_Attack()
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

//�ѹ� ������
CBTNode::NODE_STATE CAI_WPAYakuza::ATK_Heavy()
{
	if (m_iSkill == SKILL_HEAVY && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_HEAVY_ATTACK && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_HEAVY)
	{
		*m_pState = CMonster::MONSTER_HEAVY_ATTACK;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

//2�� ���ӱ�
CBTNode::NODE_STATE CAI_WPAYakuza::ATK_CMD()
{
	if (m_iSkill == SKILL_CMD && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMD_1 && *(m_pAnimCom->Get_AnimPosition()) >= 22.0)
		{
			*m_pState = CMonster::MONSTER_CMD_2;
		}
		else if (*m_pState == CMonster::MONSTER_CMD_2 && m_pAnimCom->Get_AnimFinished())
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

void CAI_WPAYakuza::Tick(const _float& fTimeDelta)
{
	if (m_isAttack == false)
		m_fAttackDelayTime += fTimeDelta;

	if (m_iSkill == SKILL_SHIFT)
		m_fShiftTime += fTimeDelta;

	this->Execute();
}

CBTNode::NODE_STATE CAI_WPAYakuza::Execute()
{
	return m_pRootNode->Execute();
}

CAI_WPAYakuza* CAI_WPAYakuza::Create(void* pArg)
{
	CAI_WPAYakuza* pInstance = new CAI_WPAYakuza();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_WPAYakuza::Free()
{
	__super::Free();
}
