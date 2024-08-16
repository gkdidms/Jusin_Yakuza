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
		//Ÿ��Ʋ ���� ��.
	}
}

void CFightManager::Tick(const _float& fTimeDelta)
{
	//��Ÿ� �ο� �� ��� ���Ͱ� ������ -> 
}

void CFightManager::Free()
{
	Safe_Release(m_pUIManager);
}
