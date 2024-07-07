#pragma once
#include "BTNode.h"

#include "Client_Defines.h"

/* Ʈ���� ������ ��� (�ൿ ���)*/

BEGIN(Client)
class CLeafNode :
    public CBTNode
{
private:
	CLeafNode();
	virtual ~CLeafNode() = default;

public:
	HRESULT Initialize(function<CBTNode::NODE_STATE()> Action);
	virtual CBTNode::NODE_STATE Execute();

private:
	function<CBTNode::NODE_STATE()> m_pAction = { nullptr };

public:
	static CBTNode* Create(function<CBTNode::NODE_STATE()> Action);
	virtual void Free() override;
};

END