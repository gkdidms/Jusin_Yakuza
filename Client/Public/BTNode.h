#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
/*
* 24.06.28 혜원 추가
* 행동트리 노드 최상위 클래스
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
