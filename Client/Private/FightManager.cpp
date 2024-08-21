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
		//타이틀 떠야 함.
		m_pUIManager->Open_Scene(TEXT("Title"));

		_uint iTitleIndex = 0;

		switch (m_pGameInstance->Get_CurrentLevel())
		{
		case LEVEL_DOGIMAZO:		//도지마조 조직원
			iTitleIndex = 16;
			break;
		case LEVEL_DOGIMAZO_BOSS:	//쿠제
			iTitleIndex = 5;
			break;
		case LEVEL_OFFICE_1F:		//동흥 크레디트
			iTitleIndex = 17;
			break;
		case LEVEL_OFFICE_BOSS:		//삥쟁
			iTitleIndex = 4;
			break;
		case LEVEL_TEST:			//삥쟁
			iTitleIndex = 15;
			break;
		default:
		{
			if (nullptr != pMonsterGroup)
				iTitleIndex = 12;		// 거리에서 만난상황이면 야쿠자
			break;
		}
		}
		m_pUIManager->Start_Title(iTitleIndex);

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
	//전투중이 아니면 바로 반환
	if (!m_isFightStage) return;

	//길거리 일 때, 몬스터가 다 죽으면 전투가 끝난다.
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
		
		//전투가 끝났을 때 속도가 느려지며 invertcolor 셰이더 호출한다.
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
	else
	{
		if (Check_Stage_Clear())
		{
			_uint iLevelIndex = m_pGameInstance->Get_CurrentLevel();

			// 레벨단위가 아니라 던전 단위로 시작과 끝을 정의해야한다.(사채업자 or 도지마조 등)
			if (iLevelIndex == LEVEL_DOGIMAZO_BOSS || iLevelIndex == LEVEL_OFFICE_BOSS || iLevelIndex == LEVEL_TEST)
			{
				m_fFinishTime += fTimeDelta;

				if (m_fFinishDuration < m_fFinishTime)
				{
					m_fFinishTime = 0.f;
					m_isFightStage = false;
					m_pGameInstance->Set_InvertColor(false);
						
					return;
				}
				m_pGameInstance->Set_InvertColor(true);
			}
			//// 던전 속 레벨이 마무리됐는지를 체크한다.
			//else 
			//{

			//}

		}

	}
}

_bool CFightManager::Check_Stage_Clear()
{
	auto pMonsters = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"));
	auto pYonedas = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Yoneda"));

	_uint iEnemyCount = { 0 };

	for (auto& pMonster : pMonsters)
	{
		if (!pMonster->isObjectDead())
			iEnemyCount++;
	}

	for (auto& pYoneda : pYonedas)
	{
		if (!pYoneda->isObjectDead())
			iEnemyCount++;
	}

	// 죽지않은 객체가 없다면 종료
	return 1 > iEnemyCount;
}

void CFightManager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pCurrentMonsterGroup);

	Safe_Release(m_pTutorialManager);
}
