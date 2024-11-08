#include "Sequance.h"

#include "BTNode.h"

CSequance::CSequance()
{
}

CBTNode::NODE_STATE CSequance::Execute()
{
	for (auto& pChildren : m_Childrens)
	{
		CBTNode::NODE_STATE eResult = pChildren->Execute();

		if (eResult == CBTNode::FAIL || eResult == CBTNode::RUNNING)
			return eResult;
	}
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

