/*
* 24.07.02 ���� �߰�
* Ű�� ���Ѵ� �ִϸ��̼� ���̵�
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRH_Sway : public CBehaviorAnimation
{
private:
	CKiryu_KRH_Sway();
	virtual ~CKiryu_KRH_Sway() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END