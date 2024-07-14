/*
* 24.06.28 ���� �߰�
* ĳ���� �ִϸ��̼� ��üȭ ����Ŭ����
*/
#pragma once
#include "Base.h"
#include "Client_Defines.h"

#pragma region Adventure
#include "Kiryu_Adventure_Idle.h"
#include "Kiryu_Adventure_Walk.h"
#include "Kiryu_Adventure_Run.h"
#pragma endregion

#pragma region KRS
#include "Kiryu_KRS_BattleStart.h"
#include "Kiryu_KRS_Idle.h"
#include "Kiryu_KRS_Walk.h"
#include "Kiryu_KRS_Run.h"
#include "Kiryu_KRS_Attack.h"
#include "Kiryu_KRS_FlyKick.h"
#include "Kiryu_KRS_KickCombo.h"
#include "Kiryu_KRS_Hit.h"
#include "Kiryu_KRS_Sway.h"
#pragma endregion

#pragma region KRH
#include "Kiryu_KRH_BattleStart.h"
#include "Kiryu_KRH_Idle.h"
#include "Kiryu_KRH_Walk.h"
#include "Kiryu_KRH_Run.h"
#include "Kiryu_KRH_Attack.h"
#include "Kiryu_KRH_Hit.h"
#include "Kiryu_KRH_Sway.h"
#pragma endregion

#pragma region KRC
#include "Kiryu_KRC_BattleStart.h"
#include "Kiryu_KRC_Idle.h"
#include "Kiryu_KRC_Walk.h"
#include "Kiryu_KRC_Run.h"
#include "Kiryu_KRC_Attack.h"
#include "Kiryu_KRC_Hit.h"
#include "Kiryu_KRC_Sway.h"
#pragma endregion

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CBehaviorAnimation : public CBase
{
protected:
	CBehaviorAnimation();
	virtual ~CBehaviorAnimation() = default;

public:
	virtual void Tick(const _float& fTimeDelta) = 0;
	virtual void Change_Animation() = 0;
	virtual _bool Get_AnimationEnd() { return false; };
	virtual void Stop() {};
	virtual void Reset() {};
	virtual void Combo_Count(_bool isFinAction = false) {};
	virtual void Setting_Value(void* pValue = nullptr) {};

protected:
	CGameInstance* m_pGameInstance = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };
	vector<_uint> m_AnimationIndices;
	_uint m_iCurrentIndex = { 0 };

public:
	// ADVENTURE: 0, KRS: 1, KRH: 2, KRC: 3
	static CBehaviorAnimation* Create(_uint iStyle, _uint iBehavior, class CPlayer* pPlayer);
	static CBehaviorAnimation* Create_Adventure_Behavior(_uint iBehavior, class CPlayer* pPlayer);
	static CBehaviorAnimation* Create_KRS_Behavior(_uint iBehavior, class CPlayer* pPlayer);
	static CBehaviorAnimation* Create_KRH_Behavior(_uint iBehavior, class CPlayer* pPlayer);
	static CBehaviorAnimation* Create_KRC_Behavior(_uint iBehavior, class CPlayer* pPlayer);

	virtual void Free() override;
};

END