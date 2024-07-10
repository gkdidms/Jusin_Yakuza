#include "BehaviorAnimation.h"
#include "GameInstance.h"

#pragma region Adventure
#include "Kiryu_Adventure_Idle.h"
#include "Kiryu_Adventure_Walk.h"
#include "Kiryu_Adventure_Run.h"
#pragma endregion

#pragma region Adventure
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

CBehaviorAnimation::CBehaviorAnimation()
	: m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

CBehaviorAnimation* CBehaviorAnimation::Create_Adventure_Behavior(_uint iBehavior, class CPlayer* pPlayer)
{
	// IDLE, WALK, RUN, ADVENTURE_BEHAVIOR_END
	switch (iBehavior)
	{
	case 0:
		return CKiryu_Adventure_Idle::Create(pPlayer);
	case 1:
		return CKiryu_Adventure_Walk::Create(pPlayer);
	case 2:
		return CKiryu_Adventure_Run::Create(pPlayer);
	}

	return nullptr;
}

CBehaviorAnimation* CBehaviorAnimation::Create_KRS_Behavior(_uint iBehavior, class CPlayer* pPlayer)
{
/*
        BTL_START, IDLE, WALK, RUN, ATTACK, HIT, 
        SWAY, DOWN, 
		SKILL_FLY_KICK, SKILL_KICK_COMBO,
*/
	switch (iBehavior)
	{
	case 0:			//BTL_START
		return CKiryu_KRS_BattleStart::Create(pPlayer);
	case 1:			// IDLE
		return CKiryu_KRS_Idle::Create(pPlayer);
	case 2:			// WALK
		return CKiryu_KRS_Walk::Create(pPlayer);
	case 3:			// RUN
		return CKiryu_KRS_Run::Create(pPlayer);
	case 4:			//ATTACK
		return CKiryu_KRS_Attack::Create(pPlayer);
	case 5:			//HIT
		return CKiryu_KRS_Hit::Create(pPlayer);
	case 6:			// SWAY
		return CKiryu_KRS_Sway::Create(pPlayer);
	case 7:			// DOWN
		break;
	case 8:			//SKILL_FLY_KICK
		return CKiryu_KRS_FlyKick::Create(pPlayer);
	case 9:			// SKILL_KICK_COMBO
		return CKiryu_KRS_KickCombo::Create(pPlayer);
	}
	return nullptr;
}

CBehaviorAnimation* CBehaviorAnimation::Create_KRH_Behavior(_uint iBehavior, class CPlayer* pPlayer)
{
	/*
		BTL_START, IDLE, WALK, RUN, ATTACK, HIT,
		SWAY, DOWN, KRS_BEHAVIOR_END
	*/
	switch (iBehavior)
	{
	case 0:			//BTL_START
		return CKiryu_KRS_BattleStart::Create(pPlayer);
	case 1:			// IDLE
		return CKiryu_KRS_Idle::Create(pPlayer);
	case 2:			// WALK
		return CKiryu_KRS_Walk::Create(pPlayer);
	case 3:			// RUN
		return CKiryu_KRS_Run::Create(pPlayer);
	case 4:			//ATTACK
		return CKiryu_KRS_Attack::Create(pPlayer);
	case 5:			//HIT
		return CKiryu_KRS_Hit::Create(pPlayer);
	case 6:			// SWAY
		return CKiryu_KRS_Sway::Create(pPlayer);
	case 7:			// DOWN
		break;
	case 8:			//SKILL_FLY_KICK
		return CKiryu_KRS_FlyKick::Create(pPlayer);
	case 9:			// SKILL_KICK_COMBO
		return CKiryu_KRS_KickCombo::Create(pPlayer);
	}
	return nullptr;
}

CBehaviorAnimation* CBehaviorAnimation::Create_KRC_Behavior(_uint iBehavior, class CPlayer* pPlayer)
{
	return nullptr;
}

CBehaviorAnimation* CBehaviorAnimation::Create(_uint iStyle, _uint iBehavior, class CPlayer* pPlayer)
{
	CBehaviorAnimation* pInstance = { nullptr };
	
	// ADVENTURE: 0, KRS: 1, KRH: 2, KRC: 3
	switch (iStyle)
	{
	case 0:
		pInstance = Create_Adventure_Behavior(iBehavior, pPlayer);
		break;
	case 1:
		pInstance = Create_KRS_Behavior(iBehavior, pPlayer);
		break;
	case 2:
		pInstance = Create_KRH_Behavior(iBehavior, pPlayer);
		break;
	case 3:
		pInstance = Create_KRC_Behavior(iBehavior, pPlayer);
		break;
	}

	return pInstance;
}

void CBehaviorAnimation::Free()
{
	Safe_Release(m_pGameInstance);
}
