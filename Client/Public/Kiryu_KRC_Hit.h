/*
* 24.07.02 ���� �߰�
* Ű�� ���Ѵ� �ִϸ��̼� ���̵�
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRC_Hit : public CBehaviorAnimation
{
public:
	struct KRC_Hit_DESC
	{
		const _float3* pDir;			// ������ ����(�ָ��̳� ���� �̵�����), ��ƼŬ���� ���
		_float fDamage;
		string strAnimationName;		// ������ �ִϸ��̼ǿ� ���� ��Ʈ����� �����ϱ� ����
		_uint iDirection;				// ������ ������ ����
	};

private:
	CKiryu_KRC_Hit();
	virtual ~CKiryu_KRC_Hit() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Reset();
	virtual void Setting_Value(void* pValue = nullptr);
	virtual string Get_AnimationName();

private:
	vector<string> m_AnimationNames;

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END