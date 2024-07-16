/*
* 24.07.02 ���� �߰�
* Ű�� ���Ѵ� �ִϸ��̼� ���̵�
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRH_Walk : public CBehaviorAnimation
{
private:
	CKiryu_KRH_Walk();
	virtual ~CKiryu_KRH_Walk() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;

private:
	_bool m_isStop = { false };

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END