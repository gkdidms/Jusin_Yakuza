/*
* 24.07.02 ���� �߰�
* Ű�� ���Ѵ� �ִϸ��̼� ���̵�
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRH_Idle : public CBehaviorAnimation
{
public:
	enum BEHAVIOR_STATE
	{
		BATTLE_ON, BATTLE_OFF, IDLE, BEHAVIOR_STATE_END
	};

	enum ANIM_STATE
	{
		ANIM_START, ANIM_LOOP, ANIM_END
	};

private:
	CKiryu_KRH_Idle();
	virtual ~CKiryu_KRH_Idle() = default;

private:
	const _float ANIM_CHANGE_TIME = 4.f;		// �� �� �̻� �����̰ų�, �������������� ���� ������

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Reset();

private:
	void Off_Battle(const _float& fTimeDelta);

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;

private:
	BEHAVIOR_STATE m_eCurrentBehavior = { BATTLE_ON };

	ANIM_STATE m_eAnimState = { ANIM_START };
	_bool m_isStop = { false };

	_float m_fOffBattleTimer = { 0.f };
};
END