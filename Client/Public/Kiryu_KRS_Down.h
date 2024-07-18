/*
* 24.07.02 ���� �߰�
* Ű�� ���Ѵ� �ִϸ��̼� ���̵�
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRS_Down : public CBehaviorAnimation
{
public:
	struct KRS_DOWN_DESC
	{
		_int iDownState;		// -1 �Ⱦ��ڴ�, 0 B(��), 1 F(��)
		_int iDirection;		// ����� ��ġ�� ������� (�����ϴ� �ݴ�� ��������), -1 ���̶�� ��Ʈ���� �ƴϴ�
		string strAnimationName;
	};

	enum ANIM_STATE
	{
		ANIM_LOOP, ANIM_END, ANIM_ONCE
	};


private:
	CKiryu_KRS_Down();
	virtual ~CKiryu_KRS_Down() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Stop() override;
	virtual _bool Stopping() override;
	virtual void Reset() override;
	virtual void Setting_Value(void* pValue) override;

private:
	ANIM_STATE m_eAnimState = { ANIM_LOOP };
	_bool m_isStop = { false };
	_bool m_isFront = { false };

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END