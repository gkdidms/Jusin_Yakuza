#include "AI_OfficeYakuza.h"

#include "GameInstance.h"

#include "LeafNode.h"
#include "Selector.h"
#include "Sequance.h"

#include "OfficeYakuza.h"
#include "Player.h"

CAI_OfficeYakuza::CAI_OfficeYakuza()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAI_OfficeYakuza::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	AI_OFFICE_YAKUZA_DESC* pDesc = static_cast<AI_OFFICE_YAKUZA_DESC*>(pArg);
	m_pThis = pDesc->pYakuza;
	m_pState = pDesc->pState;
	m_pModelCom = pDesc->pModel;
	m_pAnimCom = pDesc->pAnim;

	//Ʈ�� ����
	Ready_Tree();
	
	return S_OK;
}

void CAI_OfficeYakuza::Tick(const _float& fTimeDelta)
{
	if (m_isAttack == false)
		m_fAttackDelayTime += fTimeDelta;

	if (m_iSkill == SKILL_SHIFT)
		m_fShiftTime += fTimeDelta;

	this->Execute();
}

CBTNode::NODE_STATE CAI_OfficeYakuza::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_OfficeYakuza::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();
	
#pragma region Death
	CSequance* pDeadSeq = CSequance::Create();
	pDeadSeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Check_Death, this)));
#pragma endregion

#pragma region Sway
	CSequance* pSwaySeq = CSequance::Create();
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Check_Sway, this)));
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Sway, this)));
#pragma endregion

#pragma region Sync
	CSequance* pSyncSeq = CSequance::Create();
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Chcek_Sync, this)));
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Sync_Neck, this)));
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pHitGuardSeq = CSequance::Create();
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Check_Hit, this)));
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::HitAndGuard, this)));

	CSelector* pHitGuard = CSelector::Create();
	CSelector* pHitSelector = CSelector::Create();
	pHitSelector->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Normal_Hit, this)));
	pHitSelector->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Strong_Hit, this)));

	CSelector* pGuardSelector = CSelector::Create();
	pGuardSelector->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Guard, this)));

	pHitGuard->Add_Children(pHitSelector);
	pHitGuard->Add_Children(pGuardSelector);

	pHitGuardSeq->Add_Children(pHitGuard);
#pragma endregion

#pragma region Angry
	CSequance* pAngrySeq = CSequance::Create();
	pAngrySeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Check_Angry, this)));
	pAngrySeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Angry, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Angry_Attack, this)));

	CSelector* pAttackSelector = CSelector::Create();
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::ATK_Punch, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::ATK_CMD, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::ATK_Angry_Punch, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::ATK_Angry_Kick, this)));

	pAttackSeq->Add_Children(pAttackSelector);
#pragma endregion

#pragma region Shift
	CSequance* pShiftSeq = CSequance::Create();
	pShiftSeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Check_Shift, this)));
	pShiftSeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Shift, this)));
#pragma endregion

#pragma region Idle
	CSequance* pIdleSeq = CSequance::Create();
	pIdleSeq->Add_Children(CLeafNode::Create(bind(&CAI_OfficeYakuza::Idle, this)));
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

CBTNode::NODE_STATE CAI_OfficeYakuza::Check_Death()
{
	if (*m_pState != CMonster::DEATH)
		return CBTNode::FAIL;

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::Check_Sway()
{
	
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::Sway()
{
	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::Chcek_Sync()
{
	if (!m_isSync)
		return CBTNode::SUCCESS;

	if (m_pAnimCom->Get_AnimFinished())
		return CBTNode::SUCCESS;

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::Sync_Neck()
{
	// �÷��̾ ����� ��� ����� �Ҷ�
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	// ������ ������ ��� ��Ʋ ��Ÿ���� ��� �����ϴ�.
	if (pPlayer->Get_BattleStyle() == CPlayer::KRH)
		return CBTNode::FAIL;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::Check_Hit()
{
	if (m_iSkill != SKILL_HIT)
		return CBTNode::SUCCESS;

	// ��Ʈ����� �ϰ� �ִ°�?
	

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::HitAndGuard()
{
	//Hit üũ�ϰ� ���带 �� ������, Hit�� ������?
	//�浹�Ұ��ΰ�?
	return CBTNode::FAIL;
}

//KRS: ���Ѵ�, KRH: ����, KRC: �ı���
CBTNode::NODE_STATE CAI_OfficeYakuza::Normal_Hit()
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
CBTNode::NODE_STATE CAI_OfficeYakuza::Strong_Hit()
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

CBTNode::NODE_STATE CAI_OfficeYakuza::Guard()
{
	//����?
	
	return CBTNode::SUCCESS;
}


CBTNode::NODE_STATE CAI_OfficeYakuza::Check_Angry()
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

		return CBTNode::FAIL;
	}

	//ȭ�� ���ִ� ���¿��� �ִϸ��̼��� �������ΰ�?
	if (*m_pState == CMonster::ANGRY_START && m_pAnimCom->Get_AnimFinished())
		return CBTNode::FAIL;
	else if (*m_pState == CMonster::ANGRY_START) return CBTNode::RUNNING;

	//�̹� ȭ�� �� �ִٸ� �ٷ� ���� ������� �̵�
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::Angry()
{
	m_isAngry = true;
	*m_pState = CMonster::ANGRY_START;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::Check_Attack()
{
	if (m_isAttack == false)
	{
		if (m_fDelayAttackDuration >= m_fAttackDelayTime)
			return CBTNode::FAIL;

		m_fAttackDelayTime = 0.f; // �ʱ�ȭ
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::Attack()
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

CBTNode::NODE_STATE CAI_OfficeYakuza::Angry_Attack()
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

CBTNode::NODE_STATE CAI_OfficeYakuza::ATK_Punch()
{
	if (m_iSkill == SKILL_PUNCH && m_isAttack)
	{
		if (*m_pState == CMonster::PUNCH && *(m_pAnimCom->Get_AnimPosition()) >= 50.0)
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_PUNCH)
	{
		*m_pState = CMonster::PUNCH;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::ATK_CMD()
{
	if (m_iSkill == SKILL_CMD && m_isAttack)
	{
		if (*m_pState == CMonster::CMD_1 && *(m_pAnimCom->Get_AnimPosition()) >= 10.0)
		{
			// �� ������
			*m_pState = CMonster::CMD_2;
		}
		else if (*m_pState == CMonster::CMD_2 && *(m_pAnimCom->Get_AnimPosition()) >= 10.0)
		{
			*m_pState = CMonster::CMD_3;
		}
		else if (*m_pState == CMonster::CMD_3 && *(m_pAnimCom->Get_AnimPosition()) >= 10.0)
		{
			*m_pState = CMonster::CMD_4;
		}
		else if (*m_pState == CMonster::CMD_4 && *(m_pAnimCom->Get_AnimPosition()) >= 10.0)
		{
			*m_pState = CMonster::CMD_5;
		}
		else if (*m_pState == CMonster::CMD_5 && m_pAnimCom->Get_AnimFinished())
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
		*m_pState = CMonster::CMD_1;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::ATK_Angry_Punch()
{
	if (m_iSkill == SKILL_ANGRY_CHOP && m_isAttack)
	{
		if (*m_pState == CMonster::ANGRY_CHOP && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_ANGRY_CHOP)
	{
		*m_pState = CMonster::ANGRY_CHOP;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::ATK_Angry_Kick()
{
	if (m_iSkill == SKILL_ANGRY_KICK && m_isAttack)
	{
		if (*m_pState == CMonster::ANGRY_KICK && m_pAnimCom->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_ANGRY_KICK)
	{
		*m_pState = CMonster::ANGRY_KICK;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::Check_Shift()
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

CBTNode::NODE_STATE CAI_OfficeYakuza::Shift()
{
	m_fShiftDuration = m_pGameInstance->Get_Random(1.f, 3.f);

	_uint iIndex = m_pGameInstance->Get_Random(0, 10); // ���� �̵�����
	m_iSkill = SKILL_SHIFT;

	if (iIndex == 0)
		*m_pState = CMonster::SHIFT_F;
	else if (iIndex == 1)
		*m_pState = CMonster::SHIFT_B;
	else if (iIndex == 2)
		*m_pState = CMonster::SHIFT_L;
	else if (iIndex == 3)
		*m_pState = CMonster::SHIFT_R;
	else
		return CBTNode::FAIL;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_OfficeYakuza::Idle()
{
	*m_pState = CMonster::IDLE;

	return CBTNode::SUCCESS;
}

CAI_OfficeYakuza* CAI_OfficeYakuza::Create(void* pArg)
{
	CAI_OfficeYakuza* pInstance = new CAI_OfficeYakuza();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_OfficeYakuza::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pRootNode);
}
