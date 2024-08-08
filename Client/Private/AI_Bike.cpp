#include "AI_Bike.h"

#include "GameInstance.h"

#include "Player.h"
#include "CarChase_Monster.h"

#include "LeafNode.h"
#include "Sequance.h"
#include "Selector.h"

CAI_Bike::CAI_Bike()
	: CAI_CarChase{}
{
}

CAI_Bike::CAI_Bike(const CAI_Bike& rhs)
	: CAI_CarChase{ rhs }
{
}

HRESULT CAI_Bike::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Bike::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	return S_OK;
}

void CAI_Bike::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

CBTNode::NODE_STATE CAI_Bike::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Bike::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Dead, this)));
#pragma endregion

	//#pragma region Sync
	//	CSequance* pSyncSeq = CSequance::Create();
	//	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Check_Sync, this)));
	//	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Sync, this)));
	//#pragma endregion

#pragma region HIT
	CSequance* pHitSeq = CSequance::Create();
	pHitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Check_Hit, this)));
	pHitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Hit, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::ATK_Shot, this)));
#pragma endregion

#pragma region Shift/Idle
	CSequance* pBreakSeq = CSequance::Create();
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Idle, this)));
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pDownSeq);
	//pRoot->Add_Children(pSyncSeq);
	pRoot->Add_Children(pHitSeq);
	pRoot->Add_Children(pAttackSeq);
	pRoot->Add_Children(pBreakSeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_Bike::Dead()
{
	if (*m_pWeaponType == CCarChase_Monster::RKT)
		*m_pState = CCarChase_Monster::CARCHASE_DED_L;
	else
		*m_pState = CCarChase_Monster::CARCHASE_TENTOU_B;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Bike::Hit()
{
	*m_pState = CCarChase_Monster::CARCHASE_DAM_LV_2;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Bike::Sync()
{
	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Bike::Check_Attack()
{
	return CBTNode::NODE_STATE();
}

CBTNode::NODE_STATE CAI_Bike::ATK_Shot()
{
	return CBTNode::NODE_STATE();
}

CBTNode::NODE_STATE CAI_Bike::Idle()
{
	return CBTNode::NODE_STATE();
}

CAI_Bike* CAI_Bike::Create()
{
	CAI_Bike* pInstance = new CAI_Bike();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Bike::Clone(void* pArg)
{
	CAI_Bike* pInstance = new CAI_Bike(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Bike::Free()
{
	__super::Free();
}
