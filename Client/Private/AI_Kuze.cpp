#include "AI_Kuze.h"

#include "GameInstance.h"

#include "Selector.h"
#include "Sequance.h"
#include "LeafNode.h"

#include "Monster.h"
#include "Player.h"
#include "Kuze.h"


/*
쿠제 스킬
1. Down -> 플레이어 다운 시 사용
2. Heavy -> 근접일 시 사용
3. Hiji_2Ren
4. Jab -> 잘안쓰이는것 같기도 하고
5. Cmb_a
6. Cmb_b
7. Cmb_HeadButt
8. Cmb_Renda

1페이즈
1. Jab
2. CMD_A
3. CMD_B
4. Cmb_Renda

2페이즈
1. Heavy
2. Hiji_2Ren
3. CMD_A
4. CMD_B
5. CMB_HeadButt


도발 -> 플레이어가 넘어지면 도발함 ...
플레이어가 도발 -> 기게이지 


총 2페이즈
마지막 페이즈때는 몸에서 오라가 나온다. (붉은색)
참고 영상 : https://www.youtube.com/watch?v=Fp63CzT7IBY

일정 범위안에 들어오면 공격함.
랜덤으로 idle 상태를 유지하거나 플레이어에게 뛰어오도록 구현
*/

CAI_Kuze::CAI_Kuze()
	: CAI_Monster{}
{
}

CAI_Kuze::CAI_Kuze(const CAI_Kuze& rhs)
	: CAI_Monster{ rhs }
{
}

HRESULT CAI_Kuze::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Kuze::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	m_fDelayAttackDuration = 5.f;

	return S_OK;
}

void CAI_Kuze::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_Kuze::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Kuze::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::StandUpAndDead, this)));

	CSelector* pDownSelector = CSelector::Create();
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::StandUp, this)));
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Dead, this)));
	pDownSeq->Add_Children(pDownSelector);
#pragma endregion

#pragma region PlayerDown
	CSequance* pPlayerDownSeq = CSequance::Create();
	pPlayerDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_PlayerDown, this)));
	pPlayerDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_Down, this)));
#pragma endregion


#pragma region Sway
	CSequance* pSwaySeq = CSequance::Create();
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Sway, this)));
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Sway, this)));
#pragma endregion

#pragma region Sync
	CSequance* pSyncSeq = CSequance::Create();
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Chcek_Sync, this)));
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Sync_Neck, this)));
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pHitGuardSeq = CSequance::Create();
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Hit, this)));
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::HitAndGuard, this)));

	CSelector* pHitGuard = CSelector::Create();
	pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Guard, this)));
	pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Hit, this)));

	pHitGuardSeq->Add_Children(pHitGuard);
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Attack, this)));

	CSelector* pAttackSelector = CSelector::Create();
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_Heavy, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_Hiji_2Ren, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_Jab, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_CMD_A, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_CMD_B, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_HeadButt, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_Renda, this)));

	pAttackSeq->Add_Children(pAttackSelector);
#pragma endregion

#pragma region Step
	CSequance* pStepSeq = CSequance::Create();
	pStepSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Distance, this)));
	pStepSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Step, this)));
#pragma endregion


#pragma region Shift/Idle
	CSequance* pBreakSeq = CSequance::Create();
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Break, this)));
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ShiftAndIdle, this)));

	CSelector* pBreakSelector = CSelector::Create();
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Shift, this)));
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Idle, this)));

	pBreakSeq->Add_Children(pBreakSelector);
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pDownSeq);
	pRoot->Add_Children(pPlayerDownSeq);
	pRoot->Add_Children(pSwaySeq);
	pRoot->Add_Children(pSyncSeq);
	pRoot->Add_Children(pHitGuardSeq);
	pRoot->Add_Children(pAttackSeq);
	//pRoot->Add_Children(pStepSeq);
	pRoot->Add_Children(pBreakSeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_Kuze::Check_Attack()
{
	if (!m_isAttack)
	{
		if (m_fDelayAttackDuration > m_fAttackDelayTime)
			return CBTNode::FAIL;

		Reset_State();
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Kuze::Attack()
{
	if (m_isAttack)
		return CBTNode::SUCCESS;

	LookAtPlayer();

	//어택 정하기
	if (DistanceFromPlayer() > 4.f)
	{
		//근접이 아니라면 공격하지않고 대기 상태로 있게 된다.

		return CBTNode::FAIL;
	}

	CKuze* pKuze = dynamic_cast<CKuze*>(m_pThis);
	if (pKuze->Get_KuzePage() == CKuze::ONE)
	{
		//1페이즈
		static _uint iOneCount = 0;

		switch (iOneCount)
		{
		case 0:
			m_iSkill = SKILL_JAB;
			break;
		case 1:
		case 2:
		case 3:
			m_iSkill = SKILL_CMD_A;
			break;
		case 4:
		case 5:
			m_iSkill = SKILL_CMD_B;
			break;
		case 6:
			m_iSkill = SKILL_CMD_RENDA;
			break;
		default:
			break;
		}

		if (iOneCount >= 6)
			iOneCount = 0.f;
		else
			iOneCount++;
	}
	else if (pKuze->Get_KuzePage() == CKuze::TWO)
	{
		//2페이즈
		static _uint iTwoCount = 0;
		
		switch (iTwoCount)
		{
		case 0:
			m_iSkill = SKILL_HEAVY;
			break;
		case 1:
			m_iSkill = SKILL_HIJI_2REN;
			break;
		case 2:
		case 3:
		case 4:
			m_iSkill = SKILL_CMD_A;
			break;
		case 5:
		case 6:
			m_iSkill = SKILL_CMD_B;
			break;
		case 7:
			m_iSkill = SKILL_CMD_HEADBUTT;
			break;
		}

		if (iTwoCount <= 7)
			iTwoCount = 0;
		else
			iTwoCount++;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Kuze::Check_PlayerDown()
{
	if (!m_pPlayer->isDown())
		m_isPlayerDownAtk = false;

	if (m_pPlayer->isDown() || m_iSkill == SKILL_DOWN)
	{
		if (DistanceFromPlayer() > 3.f || m_isPlayerDownAtk)
			return CBTNode::FAIL;

		m_iSkill == SKILL_DOWN;
	
		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_Down()
{
	if (m_iSkill == SKILL_DOWN && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_ATK_DOWN && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;
			m_isPlayerDownAtk = true;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	m_isAttack = true;
	*m_pState = CMonster::MONSTER_ATK_DOWN;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_Heavy()
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
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_HEAVY_ATTACK;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_Hiji_2Ren()
{
	if (m_iSkill == SKILL_HIJI_2REN && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_HIJI_2REN && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_HIJI_2REN)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_HIJI_2REN;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_Jab()
{
	if (m_iSkill == SKILL_JAB && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_JAB && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_JAB)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_JAB;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_CMD_A()
{
	if (m_iSkill == SKILL_CMD_A && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMD_A_1 && *(m_pAnimCom->Get_AnimPosition()) >= 20.0)
		{
			*m_pState = CMonster::MONSTER_CMD_A_2;
		}
		else if (*m_pState == CMonster::MONSTER_CMD_A_2 && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_CMD_A)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_CMD_A_1;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_CMD_B()
{
	if (m_iSkill == SKILL_CMD_B && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMD_B_1 && *(m_pAnimCom->Get_AnimPosition()) >= 20.0)
			*m_pState = CMonster::MONSTER_CMD_B_2;
		else if (*m_pState == CMonster::MONSTER_CMD_B_2 && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_CMD_B)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_CMD_B_1;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_HeadButt()
{
	if (m_iSkill == SKILL_CMD_HEADBUTT && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMD_HEADBUTT_1 && *(m_pAnimCom->Get_AnimPosition()) >= 13.f)
			*m_pState = CMonster::MONSTER_CMD_HEADBUTT_2;
		else if (*m_pState == CMonster::MONSTER_CMD_HEADBUTT_2 && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_CMD_HEADBUTT)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_CMD_HEADBUTT_1;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_Renda()
{
	if (m_iSkill == SKILL_RENDA && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMD_RENDA_1 && m_pAnimCom->Get_AnimFinished())
			*m_pState = CMonster::MONSTER_CMD_RENDA_2;
		else if (*m_pState == CMonster::MONSTER_CMD_RENDA_2 && m_pAnimCom->Get_AnimFinished())
			*m_pState = CMonster::MONSTER_CMD_RENDA_3;
		else if (*m_pState == CMonster::MONSTER_CMD_RENDA_3 && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_RENDA)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_CMD_RENDA_1;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::Check_Distance()
{
	_float fDistance = DistanceFromPlayer();
	if (fDistance > 2.f)
	{
		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::Step()
{
	*m_pState = CMonster::MONSTER_STEP_F;

	return CBTNode::SUCCESS;
}

CAI_Kuze* CAI_Kuze::Create()
{
	CAI_Kuze* pInstance = new CAI_Kuze();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Kuze::Clone(void* pArg)
{
	CAI_Kuze* pInstance = new CAI_Kuze(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Kuze::Free()
{
	__super::Free();
}
