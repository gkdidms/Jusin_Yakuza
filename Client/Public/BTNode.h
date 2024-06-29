/*
* 24.06.28 ���� �߰�
* �ൿƮ�� ��� �ֻ��� Ŭ����
*/
#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CBTNode abstract : public CBase
{
public:
	enum NODE_EXE_TYPE
	{
		SUCCESS, FAIL, RUNNING
	};

public:
	CBTNode();
	virtual ~CBTNode() = default;

public:
	virtual NODE_EXE_TYPE Execute() = 0;
	const string& Get_NodeName() const { return strNodeName; }

private:
	string strNodeName = "";

public:
	virtual void Free() override = 0;
};
END
