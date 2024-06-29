#pragma once
#include "BTNode.h"

BEGIN(Client)
class CLeafNode :
    public CBTNode
{
public:
	CLeafNode();
	virtual ~CLeafNode() = default;

public:
	virtual CBTNode::NODE_EXE_TYPE Execute();

public:
	static CBTNode* Create();
	virtual void Free() override;
};

END