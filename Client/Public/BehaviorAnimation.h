/*
* 24.06.28 혜원 추가
* 캐릭터 애니메이션 객체화 상위클래스
*/
#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CModel;
END

BEGIN(Client)

class CBehaviorAnimation abstract : public CBase
{
protected:
	CBehaviorAnimation();
	virtual ~CBehaviorAnimation() = default;

public:
	virtual void Change_Animation() = 0;

protected:
	CGameInstance* m_pGameInstance = { nullptr };
	CModel* m_pModelCom = { nullptr };
	vector<_uint> m_AnimationIndex;

public:
	virtual void Free() override;
};

END