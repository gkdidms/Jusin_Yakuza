#include "Selector.h"

CSelector::CSelector()
{
}

CBTNode::NODE_STATE CSelector::Execute()
{
    // Failure 상태와 Running 상태일때 return
    // Sccess일때 계속 루프
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
