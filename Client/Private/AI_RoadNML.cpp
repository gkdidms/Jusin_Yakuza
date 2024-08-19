#include "AI_RoadNML.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

CAI_RoadNML::CAI_RoadNML()
	: CAI_RoadWalker{}
{
}

CAI_RoadNML::CAI_RoadNML(const CAI_RoadNML& rhs)
	: CAI_RoadWalker { rhs }
{
}

HRESULT CAI_RoadNML::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_RoadNML::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Root();

	return S_OK;
}

void CAI_RoadNML::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_RoadNML::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_RoadNML::Ready_Root()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Coll
	CSequance* pCollSeq = CSequance::Create();
	// Add_children : �Լ� �־���
	// check_coll : �Լ�, coll : �Լ�
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Check_Coll, this)));
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Coll, this)));

	//check_coll �����ϸ� coll�� �Ѿ
	//sequence, select�� vector�� ����, 0~���������� ������� �����Ŵ
	//sequcence�� check_coll, coll�� �־�J��
	//check_coll -> coll �Ѿ��(success)�ؼ� �׷� coll�� ���� success�Ǹ�
	//pCollSeq : CAI_RoadNML::Coll�� success�� ����
	//execute�� ����
	//pCollSeq�� success, fail, running ���ϵǸ�
	// pCollSeq success ��ȯ : 

#pragma endregion

#pragma region Walk
	CSequance* pWalkSeq = CSequance::Create();
	pWalkSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Check_Walk, this)));

	CSelector* pWalkSelector = CSelector::Create();
	pWalkSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Back, this)));
	pWalkSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Turn, this)));
	pWalkSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Walk, this)));
	pWalkSeq->Add_Children(pWalkSelector);
#pragma endregion

	
	pRoot->Add_Children(pCollSeq);
	pRoot->Add_Children(pWalkSeq);

	m_pRootNode = pRoot;
}

CAI_RoadNML* CAI_RoadNML::Create()
{
	CAI_RoadNML* pInstance = new CAI_RoadNML();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_RoadNML::Clone(void* pArg)
{
	CAI_RoadNML* pInstance = new CAI_RoadNML(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_RoadNML::Free()
{
	__super::Free();
}
