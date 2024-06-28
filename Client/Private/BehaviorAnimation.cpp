#include "BehaviorAnimation.h"
#include "GameInstance.h"

CBehaviorAnimation::CBehaviorAnimation()
	: m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

void CBehaviorAnimation::Free()
{
	Safe_Release(m_pGameInstance);
}
