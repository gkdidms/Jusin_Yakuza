#include "FightManager.h"

#include "GameInstance.h"

#include "TutorialManager.h"
#include "UIManager.h"

#include "Monster.h"
#include "MonsterGroup.h"

IMPLEMENT_SINGLETON(CFightManager)

CFightManager::CFightManager()
	: m_pGameInstance{CGameInstance::GetInstance()}
	,m_pUIManager{CUIManager::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pUIManager);
}

void CFightManager::Set_FightStage(_bool isFightStage, CMonsterGroup* pMonsterGroup)
{
	m_isFightStage = isFightStage;

	if (m_isFightStage)
	{
		//Ÿ��Ʋ ���� ��.
		m_pUIManager->Open_Scene(TEXT("Title"));
		m_pUIManager->Start_Title(0);
		

		if (pMonsterGroup != nullptr)
		{
			m_pCurrentMonsterGroup = pMonsterGroup;
			Safe_AddRef(pMonsterGroup);
		}
		else
		{
			vector<CGameObject*> Monsters = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"));

			for (auto& pMonster : Monsters)
			{
				CMonster* pTarget = dynamic_cast<CMonster*>(pMonster);
				pTarget->Set_Start(true);
			}
		}
	}
	else
	{
		Safe_Release(pMonsterGroup);
	}
}

HRESULT CFightManager::Initialize()
{
	m_pTutorialManager = CTutorialManager::Create();
	if (nullptr == m_pTutorialManager)
		return E_FAIL;

	return S_OK;
}

void CFightManager::Tick(const _float& fTimeDelta)
{
	//�������� �ƴϸ� �ٷ� ��ȯ
	if (!m_isFightStage) return;

	//��Ÿ� �� ��, ���Ͱ� �� ������ ������ ������.
	if (m_isStreetFight)
	{
		//vector<CGameObject*> Monsters = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_MonsterGroup"));
		if (m_pGameInstance->Get_CurrentLevel() == LEVEL_TUTORIAL)
		{
			if (m_pUIManager->isTitleEnd())
			{
				if (!m_pUIManager->isOpen(TEXT("Tutorial")))
				{
					m_pTutorialManager->Start_Tutorial();
					return;
				}
					
				m_pTutorialManager->Tick();
			}
		}

		vector<CMonster*> Monsters = m_pCurrentMonsterGroup->Get_Monsters();
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
				Safe_Release(m_pCurrentMonsterGroup);
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
	Safe_Release(m_pCurrentMonsterGroup);

	Safe_Release(m_pTutorialManager);
}
