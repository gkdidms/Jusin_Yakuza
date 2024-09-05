#include "FightManager.h"

#include "GameInstance.h"

#include "TutorialManager.h"
#include "UIManager.h"

#include "Monster.h"
#include "MonsterGroup.h"

#include "UIFightScore.h"
#include "PlayerCamera.h"
#include "Player.h"

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
	m_isStreetFight = false;
	m_isInverseEnd = false;
	m_isMoney = false;

	m_pUIManager->Set_AlwayUI(true);

	if (m_isFightStage)
	{
		_int iTitleIndex = -1;

		switch (m_pGameInstance->Get_CurrentLevel())
		{
		case LEVEL_DOGIMAZO:		//�������� ������
			iTitleIndex = 16;
			break;
		case LEVEL_DOGIMAZO_BOSS:	//����
			iTitleIndex = 5;
			break;
		case LEVEL_OFFICE_1F:		//���� ũ����Ʈ
			iTitleIndex = 17;
			break;
		case LEVEL_OFFICE_BOSS:		//����
			iTitleIndex = 4;
			break;
		case LEVEL_CARCHASE:		//��Ȳí
			iTitleIndex = 3;
			break;
		case LEVEL_TEST:			//�׽�Ʈ������ �׳� �ϰų�����
			iTitleIndex = 15;
			break;
		case LEVEL_TUTORIAL:			//��ť��
		{
			m_pGameInstance->StopSound(SOUND_BGM);
			m_pGameInstance->PlayBGM(TEXT("Tutorial_Fight.mp3"), 0.6f);
			m_isStreetFight = true;
			iTitleIndex = 13;
			break;
		}
		case LEVEL_STREET:			//����
		{
			// �Ÿ����� ������Ȳ�̸� ������
			if (nullptr != pMonsterGroup)
			{
				iTitleIndex = 12;
			}
			break;
		}
		default:
		{
			// �� �� �������� �ҷ����� ���� �������� �ٸ� ������ �Ǵ��Ͽ� �ƹ��� ó���� ���� ����
			break;
		}
		}

		// Ÿ��Ʋ �ε����� 0���� ū ��쿡�� Ÿ��Ʋ�� ���� ������
		if (0 < iTitleIndex)
		{
			//Ÿ��Ʋ ���� ��.
			m_pGameInstance->StopSound(SOUND_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("47cd [5].wav"), SOUND_EFFECT, 0.5f);

			m_pUIManager->Open_Scene(TEXT("Title"));

			m_pUIManager->Start_Title(iTitleIndex);

			if (pMonsterGroup != nullptr)
			{
				m_pCurrentMonsterGroup = pMonsterGroup;
				Safe_AddRef(pMonsterGroup);
			}
			else
			{
				if (m_pGameInstance->Get_CurrentLevel() == LEVEL_CARCHASE)
					return;

				vector<CGameObject*> Monsters = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"));

				for (auto& pMonster : Monsters)
				{
					CMonster* pTarget = dynamic_cast<CMonster*>(pMonster);
					pTarget->Set_Start(true);
				}
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
	if (m_pGameInstance->Get_CurrentLevel() == LEVEL_TUTORIAL)
	{
		m_pTutorialManager = CTutorialManager::Create();
		if (nullptr == m_pTutorialManager)
			return E_FAIL;
	}
	m_pGameInstance->Set_InvertColorDuration(1.f);

	return S_OK;
}

_bool CFightManager::Tick(const _float& fTimeDelta)
{
	//�������� �ƴϸ� �ٷ� ��ȯ
	if (!m_isFightStage) return false;

	//��Ÿ� �� ��, ���Ͱ� �� ������ ������ ������.
	if (m_isStreetFight)
	{
		//vector<CGameObject*> Monsters = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_MonsterGroup"));
		if (m_pGameInstance->Get_CurrentLevel() == LEVEL_TUTORIAL)
		{
			if (m_isTutorialEnd)
			{
				m_fFinishTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

				_float fRatio = (m_fFinishDuration - m_fFinishTime * 1.5f) / m_fFinishDuration;

				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), fRatio < 0 ? 0.f : fRatio);
				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), fRatio < 0 ? 0.f : fRatio);

				if (!m_isMoney)
				{
					m_pGameInstance->PlaySound_W(TEXT("4681 [31].wav"), SOUND_UI, 0.5f);

					m_isMoney = true;
					m_pUIManager->Open_Scene(TEXT("FightScore"));
					dynamic_cast<CUIFightScore*>(m_pUIManager->Find_Scene(TEXT("FightScore")))->StartAddMoney(500000);
				}

				CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
				pCamera->Set_CustomRatio(fRatio);
				pCamera->Set_TargetFoV(XMConvertToRadians(30.f));
				pCamera->Start_Zoom();

				if (m_fFinishDuration < m_fFinishTime)
				{
					Safe_Release(m_pCurrentMonsterGroup);
					m_isFightStage = false;
					m_pGameInstance->Set_InvertColor(false);
					//�÷��̾ �ٽ� ���ƿ´�.
					CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
					pPlayer->Style_Change(CPlayer::ADVENTURE);
					m_pGameInstance->StopSound(SOUND_BGM);
					m_pUIManager->Set_AlwayUI(false);

					m_pGameInstance->PlayBGM(TEXT("Street_BGM.mp3"), 0.3f);

					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 1.f);

					m_pUIManager->Close_Scene(TEXT("FightScore"));

					m_pGameInstance->StopSound(SOUND_UI);

					return true;
				}
				m_pGameInstance->Set_InvertColor(true);
			}

			if (m_pUIManager->isTitleEnd())
			{
				if (!m_pUIManager->isOpen(TEXT("Tutorial")) && !m_isTutorialStart)
				{
					m_isTutorialStart = true;
					m_pTutorialManager->Start_Tutorial();
					return false;
				}

				//�Ʒ��� �ִ� �ڵ� ����. ���� ��.
				if (m_pTutorialManager->Tick())
				{
					m_isTutorialEnd = true;
				}
			}

			return false;
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
				return true;
			}
			m_pGameInstance->Set_InvertColor(true);
		}
	}
	else
	{
		if (Check_Stage_Clear())
		{
			_uint iLevelIndex = m_pGameInstance->Get_CurrentLevel();

			// ���������� �ƴ϶� ���� ������ ���۰� ���� �����ؾ��Ѵ�.(��ä���� or �������� ��)
			if (iLevelIndex == LEVEL_DOGIMAZO_BOSS || iLevelIndex == LEVEL_OFFICE_BOSS || iLevelIndex == LEVEL_TEST)
			{
				m_fFinishTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

				_float fRatio = (m_fFinishDuration - m_fFinishTime * 1.5f) / m_fFinishDuration;

				if (!m_isMoney)
				{
					m_pGameInstance->PlaySound_W(TEXT("4681 [31].wav"), SOUND_UI, 0.5f);

					m_isMoney = true;
					m_pUIManager->Open_Scene(TEXT("FightScore"));
					dynamic_cast<CUIFightScore*>(m_pUIManager->Find_Scene(TEXT("FightScore")))->AddMoney(CPlayer::PlayerInfo.iMoney);
				}

				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), fRatio < 0 ? 0.f : fRatio);
				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), fRatio < 0 ? 0.f : fRatio);

				CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
				pCamera->Set_CustomRatio(fRatio);
				pCamera->Set_TargetFoV(XMConvertToRadians(30.f));
				pCamera->Start_Zoom();

				if (m_fFinishDuration < m_fFinishTime)
				{
					m_fFinishTime = 0.f;
					m_isInverseEnd = true;
					m_isFightStage = false;
					m_pGameInstance->Set_InvertColor(false);

					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 1.f);

					m_pUIManager->Close_Scene(TEXT("FightScore"));

					m_pGameInstance->StopSound(SOUND_UI);
						
					return true;
				}
				m_pGameInstance->Set_InvertColor(true);
			}
			// ���� �� ������ �������ƴ����� üũ�Ѵ�.
			else 
			{
				m_fFinishTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.f);
				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 0.f);

				if (0.5f < m_fFinishTime)
				{

					m_pUIManager->Set_AlwayUI(false);

					m_fFinishTime = 0.f;
					m_isFightStage = false;
#ifdef _DEBUG
					cout << "��� ���̴� Off" << endl;
#endif

					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 1.f);

					return true;
				}
#ifdef _DEBUG
				cout << "��� ���̴� On" << endl;
#endif
			}

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

	// �������� ��ü�� ���ٸ� ����
	return 1 > iEnemyCount;
}

void CFightManager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pCurrentMonsterGroup);

	Safe_Release(m_pTutorialManager);
}
