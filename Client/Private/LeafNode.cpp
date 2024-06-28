#include "LeafNode.h"

CLeafNode::CLeafNode()
{
}

CBTNode::NODE_EXE_TYPE CLeafNode::Execute()
{
    return NODE_EXE_TYPE();
}

CBTNode* CLeafNode::Create()
{
    return nullptr;
}

void CLeafNode::Free()
{
}
