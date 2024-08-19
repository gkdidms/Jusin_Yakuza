#include "Sequance.h"

#include "BTNode.h"

CSequance::CSequance()
{
}

CBTNode::NODE_STATE CSequance::Execute()
{
	// Add�ϸ� children�� �־���
	for (auto& pChildren : m_Childrens)
	{
		// execute�� ��tick ���� ����
		// for�� ����
		CBTNode::NODE_STATE eResult = pChildren->Execute();

		//running : ������
		if (eResult == CBTNode::FAIL || eResult == CBTNode::RUNNING)
			return eResult;
	}

	// success�� ������ �ڽĳ�带 ����
	return CBTNode::SUCCESS;
}

void CSequance::Add_Children(CBTNode* pChildren)
{
	m_Childrens.emplace_back(pChildren);
}

CSequance* CSequance::Create()
{
    return new CSequance();
}

void CSequance::Free()
{
    __super::Free();

    for (auto& pChildren : m_Childrens)
        Safe_Release(pChildren);
}

