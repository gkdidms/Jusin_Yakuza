/*
* 24.07.02 ���� �߰�
* Ű�� ���Ѵ� �ִϸ��̼� ���̵�
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRS_Run : public CBehaviorAnimation
{
public:
	enum BEHAVIOR_STATE
	{
		TURN, WALK, RUN, BUMP, BEHAVIOR_STATE_END
	};

	enum ANIM_STATE
	{
		ANIM_START, ANIM_LOOP, ANIM_END
	};


private:
	CKiryu_KRS_Run();
	virtual ~CKiryu_KRS_Run() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Stop() override;

private:
	ANIM_STATE m_eAnimState = { ANIM_START };
	_bool m_isStop = { false };

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END