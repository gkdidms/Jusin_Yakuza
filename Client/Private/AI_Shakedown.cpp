#include "AI_Shakedown.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

#include "Monster.h"
#include "Player.h"

/*
�����̴� �г��尡 ����.
����ϴ� ��ų�� �� 4������
1. ��������
2. ������Ʈ
3. �޺�3�ܰ�
4. �ٿ����

1. �������� �� �÷��̾���� �Ÿ��� �� ��� ����ϴ� ��ų�̴�.
�������忡 �°� �� �÷��̾�� ������ �ٷ� �ٿ�ȴ�.

2. ������Ʈ �� ������ �´� �� �ߵ��ϰų� ������ ���� �� Ȯ�������� ��Ÿ����.
�÷��̾ ������Ʈ�� ���ϰ� �Ǹ� ��Ʈ�׼��� ������ ���� �� �ִ�.
https://www.youtube.com/watch?v=N3vh9EAJvzE

3. �޺� 3�ܰ�� Ȯ�������� 1~3�ܰ�� �ߵ��Ѵ�.

4. �ٿ������ �÷��̾ �ٿ�� �� �ߵ��Ѵ�.
�÷��̾ �ٿ�� ��� ��ų ����� ������ �ߵ��Ѵ�.
�´� ���� ���� ���.
*/


CAI_Shakedown::CAI_Shakedown()
	: CAI_Monster{}
{
}

CAI_Shakedown::CAI_Shakedown(const CAI_Shakedown& rhs)
	: CAI_Monster{ rhs }
{
}

HRESULT CAI_Shakedown::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Shakedown::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	m_fDelayAttackDuration = 3.f;

	m_iMonsterType = CMonster::SHAKDDOWN;

	m_fSwayDistance = _float2(1.8f, 2.f);

	return S_OK;
}

void CAI_Shakedown::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_Shakedown::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Shakedown::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Sync
	CSequance* pSyncSeq = CSequance::Create();
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Chcek_Sync, this)));
#pragma endregion

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::StandUpAndDead, this)));

	CSelector* pDownSelector = CSelector::Create();
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::StandUp, this)));
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Dead, this)));
	pDownSeq->Add_Children(pDownSelector);
#pragma endregion

#pragma region PlayerDown
	CSequance* pPlayerDownSeq = CSequance::Create();
	pPlayerDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Check_PlayerDown, this)));
	pPlayerDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::ATK_Down, this)));
#pragma endregion


#pragma region Sway
	CSequance* pSwaySeq = CSequance::Create();
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Check_Sway, this)));
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Sway, this)));
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pHitGuardSeq = CSequance::Create();
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Check_Hit, this)));
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::HitAndGuard, this)));

	CSelector* pHitGuard = CSelector::Create();
	pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Guard, this)));
	pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Hit, this)));

	pHitGuardSeq->Add_Children(pHitGuard);
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Attack, this)));

	CSelector* pAttackSelector = CSelector::Create();
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::ATK_GuardRun, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::ATK_Rariatto, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::ATK_CMD, this)));

	pAttackSeq->Add_Children(pAttackSelector);
#pragma endregion

#pragma region Shift/Idle
	CSequance* pBreakSeq = CSequance::Create();
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Check_Break, this)));
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::ShiftAndIdle, this)));

	CSelector* pBreakSelector = CSelector::Create();
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Shift, this)));
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_Shakedown::Idle, this)));

	pBreakSeq->Add_Children(pBreakSelector);
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pSyncSeq);
	pRoot->Add_Children(pDownSeq);
	pRoot->Add_Children(pPlayerDownSeq);
	pRoot->Add_Children(pSwaySeq);
	pRoot->Add_Children(pHitGuardSeq);
	pRoot->Add_Children(pAttackSeq);
	pRoot->Add_Children(pBreakSeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_Shakedown::Attack()
{
	if (m_isAttack)
		return CBTNode::SUCCESS;

	LookAtPlayer();

	_float fDistance = DistanceFromPlayer();
	if (fDistance > 5.f) // �Ÿ��� �ִٸ�
	{
		m_iSkill = SKILL_GUARD_RUN;
	}
	else
	{
		//�Ÿ��� �����ٸ�
		//1. ������Ʈ
		//2. �޺�
		_float fRariattoRamdom = m_pGameInstance->Get_Random(0.f, 100.f);
		if (fRariattoRamdom > 90.f)
		{
			m_iSkill = SKILL_RARIATTO;
		}
		else
		{
			m_iSkill = SKILL_CMD;
		}
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Shakedown::ATK_CMD()
{
	if (m_iSkill == SKILL_CMD && m_isAttack)
	{
		if (m_fCmbNum <= m_fCmbCount)
		{
			//�޺� ��
			if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
			{
				m_isAttack = false;

				return CBTNode::SUCCESS;
			}

			return CBTNode::RUNNING;
		}

		if (*m_pState == CMonster::MONSTER_CMB_1 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 24.0)
		{
			LookAtPlayer();
			*m_pState = CMonster::MONSTER_CMB_2;
			m_fCmbCount++;
		}
		else if (*m_pState == CMonster::MONSTER_CMB_2 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 29.0)
		{
			LookAtPlayer();
			*m_pState = CMonster::MONSTER_CMB_3;
			m_fCmbCount++;
		}
		else if (*m_pState == CMonster::MONSTER_CMB_3 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_CMD)
	{
		m_fCmbNum = m_pGameInstance->Get_Random(0.f, 3.f); // 0 ~ 2 ���̰��� ���´�.
		m_fCmbCount = 0.f;

		m_isAttack = true;
		*m_pState = CMonster::MONSTER_CMB_1;

		return CBTNode::SUCCESS;
	}
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Shakedown::ATK_GuardRun()
{
	if (m_iSkill == SKILL_GUARD_RUN && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_GUARD_RUN && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_GUARD_RUN)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_GUARD_RUN;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Shakedown::ATK_Rariatto()
{
	if (m_iSkill == SKILL_RARIATTO && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_RARIATTO && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_RARIATTO)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_RARIATTO;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CAI_Shakedown* CAI_Shakedown::Create()
{
	CAI_Shakedown* pInstance = new CAI_Shakedown();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Shakedown::Clone(void* pArg)
{
	CAI_Shakedown* pInstance = new CAI_Shakedown(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Shakedown::Free()
{
	__super::Free();
}