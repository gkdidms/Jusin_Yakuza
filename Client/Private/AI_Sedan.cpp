#include "AI_Sedan.h"

#include "GameInstance.h"

#include "Player.h"
#include "CarChase_Monster.h"

#include "LeafNode.h"
#include "Sequance.h"
#include "Selector.h"

CAI_Sedan::CAI_Sedan()
	: CAI_CarChase{}
{
}

CAI_Sedan::CAI_Sedan(const CAI_Sedan& rhs)
	: CAI_CarChase { rhs }
{
}

HRESULT CAI_Sedan::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Sedan::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	return S_OK;
}

void CAI_Sedan::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

CBTNode::NODE_STATE CAI_Sedan::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Sedan::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Sedan::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Sedan::Dead, this)));
#pragma endregion

#pragma region HIT
	CSequance* pHitSeq = CSequance::Create();
	pHitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Sedan::Check_Hit, this)));
	pHitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Sedan::Hit, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Sedan::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Sedan::ATK_Shot, this)));
#pragma endregion

#pragma region Idle
	CSequance* pIdleSeq = CSequance::Create();
	pIdleSeq->Add_Children(CLeafNode::Create(bind(&CAI_Sedan::Check_Idle, this)));
	pIdleSeq->Add_Children(CLeafNode::Create(bind(&CAI_Sedan::Idle, this)));
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pDownSeq);
	pRoot->Add_Children(pHitSeq);
	pRoot->Add_Children(pAttackSeq);
	pRoot->Add_Children(pIdleSeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_Sedan::Dead()
{
	if (*m_pWeaponType == CCarChase_Monster::GUN_L)
		*m_pState = CCarChase_Monster::CARCHASE_AIML_DAD;
	else if (*m_pWeaponType == CCarChase_Monster::GUN_R)
		*m_pState = CCarChase_Monster::CARCHASE_AIMR_DAD;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Sedan::Hit()
{
	if (m_isHit)
	{
		if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isHit = false;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (*m_pWeaponType == CCarChase_Monster::GUN_L)
		*m_pState = CCarChase_Monster::CARCHASE_AIML_DAM;
	else if (*m_pWeaponType == CCarChase_Monster::GUN_R)
		*m_pState = CCarChase_Monster::CARCHASE_AIMR_DAM;
	else if (*m_pWeaponType == CCarChase_Monster::DRV)
		*m_pState = CCarChase_Monster::CARCHASE_SIT_DWN;

	m_isHit = true;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Sedan::Check_Attack()
{
	if (m_isAttack) return CBTNode::SUCCESS;

	if (*m_pWeaponType == CCarChase_Monster::DRV || !m_isAtkReady)
	{
		m_fAttackDelayTime = 0.f;
		return CBTNode::FAIL;
	}

	if (m_fDelayAttackDuration <= m_fAttackDelayTime)
	{
		m_iSkill = SKILL_SHOT;
		m_fAttackDelayTime = 0.f;
		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Sedan::ATK_Shot()
{
	if (m_iSkill == SKILL_SHOT && m_isAttack)
	{
		if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_SHOT)
	{
		m_isAttack = true;
		
		if (*m_pWeaponType == CCarChase_Monster::GUN_L)
			*m_pState = CCarChase_Monster::CARCHASE_AIML_SHOT;
		else if (*m_pWeaponType == CCarChase_Monster::GUN_R)
			*m_pState = CCarChase_Monster::CARCHASE_AIMR_SHOT;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Sedan::Check_Idle()
{
	if (m_isAtkReady || *m_pWeaponType == CCarChase_Monster::DRV) return CBTNode::SUCCESS;

	if (m_fDelayAttackReadyDuration < m_fAttackReadyDelayTime)
	{
		m_isAtkReady = true;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Sedan::Idle()
{
	if (m_isAtkReady)
	{
		//준비자세
		if (*m_pWeaponType == CCarChase_Monster::GUN_R)
			*m_pState = CCarChase_Monster::CARCHASE_AIMR_LP;
		else if (*m_pWeaponType == CCarChase_Monster::GUN_L)
			*m_pState = CCarChase_Monster::CARCHASE_AIML_LP;
	}
	else
	{
		if (*m_pWeaponType == CCarChase_Monster::GUN_R)
			*m_pState = CCarChase_Monster::CARCHASE_AIMR_STD_LP;
		else if (*m_pWeaponType == CCarChase_Monster::GUN_L)
			*m_pState = CCarChase_Monster::CARCHASE_AIML_STD_LP;
		else
			*m_pState = CCarChase_Monster::CARCHASE_SIT_LP;
	}

	return CBTNode::SUCCESS;
}

CAI_Sedan* CAI_Sedan::Create()
{
	CAI_Sedan* pInstance = new CAI_Sedan();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Sedan::Clone(void* pArg)
{
	CAI_Sedan* pInstance = new CAI_Sedan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Sedan::Free()
{
	__super::Free();
}
