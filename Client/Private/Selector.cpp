#include "Selector.h"

CSelector::CSelector()
{
}

CBTNode::NODE_STATE CSelector::Execute()
{
    // Failure ���¿� Running �����϶� return
    // Sccess�϶� ��� ����
    for (auto& pChildren : m_ChildNode)
    {
        CBTNode::NODE_STATE eState = pChildren->Execute();

        // Success�� Running�϶� �ٷγ���
        // coll ���ȴµ� coll->success��? : root���� succes�� run�̸� return�ż� walk��°� ������ �ȵ�
        // Selector : �ڽĳ�尡 �ϳ��� success�� ������ ����Ƚ�Ŵ
        // Sequence : and ����
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
