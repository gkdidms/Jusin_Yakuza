/*
* 24.06.28 ���� �߰�
* ĳ���� �ִϸ��̼� ��üȭ ����Ŭ����
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