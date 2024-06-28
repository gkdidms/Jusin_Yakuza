#pragma once
#include "BTNode.h"

BEGIN(Client)
class CCompositeNode :
    public CBTNode
{
public:
	CCompositeNode();
	virtual ~CCompositeNode() = default;

public:
	virtual NODE_EXE_TYPE Execute();

private:
	vector<CBTNode*> m_ChildNodes;

public:
	static CBTNode* Create();
	virtual void Free() override;
};
END