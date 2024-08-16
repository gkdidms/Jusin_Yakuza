#include "FightManager.h"

#include "UIManager.h"

IMPLEMENT_SINGLETON(CFightManager)

CFightManager::CFightManager()
	: m_pUIManager{CUIManager::GetInstance()}
{
	Safe_AddRef(m_pUIManager);
}

void CFightManager::Set_FightStage(_bool isFightStage)
{
	m_isFightStage = isFightStage;

	if (m_isFightStage)
	{
		//타이틀 떠야 함.
	}
}

void CFightManager::Tick(const _float& fTimeDelta)
{
	//길거리 싸움 시 모든 몬스터가 죽으면 -> 
}

void CFightManager::Free()
{
	Safe_Release(m_pUIManager);
}
