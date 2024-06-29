#include "DecoratorNode.h"

CDecoratorNode::CDecoratorNode()
{
}

CBTNode::NODE_EXE_TYPE CDecoratorNode::Execute()
{
    return NODE_EXE_TYPE();
}

CBTNode* CDecoratorNode::Create()
{
    return nullptr;
}

void CDecoratorNode::Free()
{
}
