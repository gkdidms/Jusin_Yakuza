/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 시작 모션
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRH_BattleStart : public CBehaviorAnimation
{
private:
	CKiryu_KRH_BattleStart();
	virtual ~CKiryu_KRH_BattleStart() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END