/*
* 24.06.28 ���� �߰�
* Ű�� �ִϸ��̼� ����ó ���̵�
*/
#pragma once
#include "BehaviorAnimation.h"
#include "Client_Defines.h"

BEGIN(Client)

class CKiryu_Adventure_Idle : public CBehaviorAnimation
{
public:
	enum BEHAVIOR_STATE
	{
		IDLE, LOOK_AROUND, BEHAVIOR_STATE_END
	};

private:
	const _float ANIM_CHANGE_TIME = 30.f;	//30�ʿ� �ѹ� �� �ֺ� �ѷ����� �ִϸ��̼� �����Ű��

private:
	CKiryu_Adventure_Idle();
	virtual ~CKiryu_Adventure_Idle() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;

private:
	void LookAroundTimer(const _float& fTimeDelta);

private:
	BEHAVIOR_STATE m_eCurrentBehavior = { IDLE };
	_float m_fLookAroundTimer = { 0.f };
};
END