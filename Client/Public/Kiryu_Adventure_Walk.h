/*
* 24.06.28 혜원 추가
* 키류 애니메이션 어드밴처 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_Adventure_Walk : public CBehaviorAnimation
{
private:
	CKiryu_Adventure_Walk();
	virtual ~CKiryu_Adventure_Walk() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	
public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END