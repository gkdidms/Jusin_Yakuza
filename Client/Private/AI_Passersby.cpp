#include "AI_Passersby.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

CAI_Passersby::CAI_Passersby()
	: CAI_Adventure{}
{
}

CAI_Passersby::CAI_Passersby(const CAI_Passersby& rhs)
	: CAI_Adventure { rhs }
{
}

HRESULT CAI_Passersby::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Passersby::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Root();

	return S_OK;
}

void CAI_Passersby::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_Passersby::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Passersby::Ready_Root()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Coll
	CSequance* pCollSeq = CSequance::Create();
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_Passersby::Check_Coll, this)));
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_Passersby::Coll, this)));
#pragma endregion

#pragma region Walk
	CSequance* pWalkSeq = CSequance::Create();
	pWalkSeq->Add_Children(CLeafNode::Create(bind(&CAI_Passersby::Check_Walk, this)));
	pWalkSeq->Add_Children(CLeafNode::Create(bind(&CAI_Passersby::Walk, this)));
#pragma endregion

	pRoot->Add_Children(pCollSeq);
	pRoot->Add_Children(pWalkSeq);

	m_pRootNode = pRoot;
}

CAI_Passersby* CAI_Passersby::Create()
{
	CAI_Passersby* pInstance = new CAI_Passersby();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Passersby::Clone(void* pArg)
{
	CAI_Passersby* pInstance = new CAI_Passersby(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Passersby::Free()
{
	__super::Free();
}
