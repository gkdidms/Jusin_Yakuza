#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
/*
* 24.06.28 ���� �߰�
* �ൿƮ�� ��� �ֻ��� Ŭ����
*/

BEGIN(Client)
class CBTNode abstract : public CBase
{
public:
	enum NODE_STATE { SUCCESS, FAIL, RUNNING };

public:
	CBTNode();
	virtual ~CBTNode() = default;

public:
	virtual NODE_STATE Execute() = 0;

public:
	virtual void Free() override;
};
END
