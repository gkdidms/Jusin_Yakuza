#include "CompositeNode.h"

CCompositeNode::CCompositeNode()
{
}

CBTNode::NODE_EXE_TYPE CCompositeNode::Execute()
{
    return NODE_EXE_TYPE();
}

CBTNode* CCompositeNode::Create()
{
    return nullptr;
}

void CCompositeNode::Free()
{
}
