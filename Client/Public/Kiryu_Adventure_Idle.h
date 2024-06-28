/*
* 24.06.28 혜원 추가
* 키류 애니메이션 어드밴처 아이들
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