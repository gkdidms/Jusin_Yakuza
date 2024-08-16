#include "FightManager.h"

#include "GameInstance.h"
#include "UIManager.h"

#include "Monster.h"

IMPLEMENT_SINGLETON(CFightManager)

CFightManager::CFightManager()
	: m_pGameInstance{CGameInstance::GetInstance()}
	,m_pUIManager{CUIManager::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pUIManager);
}

void CFightManager::Set_FightStage(_bool isFightStage)
{
	m_isFightStage = isFightStage;

	if (m_isFightStage)
	{
		//Ÿ��Ʋ ���� ��.
		m_pUIManager->Open_Scene(TEXT("Title"));
		m_pUIManager->Start_Title(0);


		vector<CGameObject*> Monsters = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"));

		for (auto& pMonster : Monsters)
		{
			CMonster* pTarget = dynamic_cast<CMonster*>(pMonster);
			pTarget->Set_Start(true);
		}
	}
}

void CFightManager::Tick(const _float& fTimeDelta)
{
	//�������� �ƴϸ� �ٷ� ��ȯ
	if (!m_isFightStage) return;

	//��Ÿ� �� ��, ���Ͱ� �� ������ ������ ������.
	if (m_pGameInstance->Get_CurrentLevel() == LEVEL_STREET)
	{
		vector<CGameObject*> Monsters = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"));

		_bool isFinished = true;
		for (auto& pMonster : Monsters)
		{
			if (!pMonster->isObjectDead())
			{
				isFinished = false;
				break;
			}
		}
		
		//������ ������ �� �ӵ��� �������� invertcolor ���̴� ȣ���Ѵ�.
		if (isFinished == true)
		{
			m_fFinishTime += fTimeDelta;

			if (m_fFinishDuration < m_fFinishTime)
			{
				m_isFightStage = false;
				m_pGameInstance->Set_InvertColor(false);
			}
			m_pGameInstance->Set_InvertColor(true);
		}
	}

	// �� �� ���������� ù �������� ȭ�鿡���� Start Ÿ��Ʋ�� �����ش�.
	
}

void CFightManager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pUIManager);
}
