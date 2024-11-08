#include "Selector.h"

CSelector::CSelector()
{
}

CBTNode::NODE_STATE CSelector::Execute()
{
    for (auto& pChildren : m_ChildNode)
    {
        CBTNode::NODE_STATE eState = pChildren->Execute();

        if (eState == CBTNode::SUCCESS || eState == CBTNode::RUNNING)
            return eState;
    }

    return CBTNode::FAIL;
}

void CSelector::Add_Children(CBTNode* pChildren)
{
    m_ChildNode.emplace_back(pChildren);
}

CSelector* CSelector::Create()
{
    return new CSelector();
}

void CSelector::Free()
{
    __super::Free();

    for (auto& pChildren : m_ChildNode)
        Safe_Release(pChildren);
}
