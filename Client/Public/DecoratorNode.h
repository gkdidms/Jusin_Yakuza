#pragma once
#include "BTNode.h"

BEGIN(Client)
class CDecoratorNode :
    public CBTNode
{
public:
    CDecoratorNode();
    virtual ~CDecoratorNode() = default;

public:
    virtual NODE_EXE_TYPE Execute();

private:
    CBTNode* m_ChildNode = { nullptr };

public:
    static CBTNode* Create();
    virtual void Free() override;
};

END