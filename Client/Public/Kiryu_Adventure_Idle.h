/*
* 24.06.28 ���� �߰�
* Ű�� �ִϸ��̼� ����ó ���̵�
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_Adventer_Idel : public CBehaviorAnimation
{
private:
	CKiryu_Adventer_Idel();
	virtual ~CKiryu_Adventer_Idel() = default;

public:
	virtual void Change_Animation();

public:
	virtual void Free() override;
};
END