#include "KaraokeManager.h"
#include "UIManager.h"
#include "NoteManager.h"
#include "GameInstance.h"

#include "UIKaraoke_Play.h"

#include "NoteSingle.h"
#include "NoteLong.h"
#include "NoteBurstHold.h"

CKaraokeManager::CKaraokeManager()
	: m_pUIManager{ CUIManager::GetInstance() },
	m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pUIManager);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CKaraokeManager::Initialize()
{
	if (FAILED(Ready_Karaoke()))
		return E_FAIL;

	// UI���� ��Ʈ ������ �����Ѵ�
	Setting_NoteUIs();

	return S_OK;
}

void CKaraokeManager::Tick(const _float& fTimeDelta)
{

	if(!m_isStart)
		KeyInput(); //�̰� ī����� Ű�� �뵵
	else
	{
	//���⼭ ui�� ��Ʈ�� ���� ����ؼ� �÷���������,Ű�ޱ� ���
		//if (m_pGameInstance->GetKeyState(DIK_O) == TAP)//�̰� �뷡 �����ٴ� ������
		//{
		//	m_pUIManager->Open_Scene(TEXT("Karaoke_Score"));
		//	m_isStart = false;

		//	//�Ʒ� ���� �ָ�� ���� ����
		//	m_iCurrentScore = 80;
		//	m_pUIManager->Set_Score(m_iCurrentScore);
		//}

		CUIKaraoke_Play* pPlayScene = dynamic_cast<CUIKaraoke_Play*>(m_pUIManager->Find_Scene(TEXT("Karaoke_Play")));

		if (pPlayScene->IsSongEnd())
		{
			m_isScoreTimerOn = true;
			m_pUIManager->Open_Scene(TEXT("Karaoke_Score"));
			m_pGameInstance->StopSound(SOUND_UI);
			//�Ʒ� ���� �ָ�� ���� ����
			m_iCurrentScore = 80;
			m_pUIManager->Set_Score(m_iCurrentScore);
		}

		if (m_isScoreTimerOn)
		{
			if(0 == m_fScoreTimer)
				m_pGameInstance->StopAndPlaySound(L"result_count_finish.wav", SOUND_UI, 0.5f);

			m_fScoreTimer += fTimeDelta;
			if (SCORE_TIMER <= m_fScoreTimer)
			{
				m_pGameInstance->StopSound(SOUND_UI);
				
				m_pUIManager->Close_Scene();

				m_isStart = false;
				m_isScoreEnd = true;
			}
		}

		Check_NextNote();
		Update_CurrentNoteIndex();			// ������ �ڿ��������
	}

}

void CKaraokeManager::KeyInput()
{
	if (m_pGameInstance->GetKeyState(DIK_K) == TAP)
	{
		Open_SelectWindow();
	}
}

void CKaraokeManager::Update_CurrentNoteIndex()
{
	auto pNotes = m_Music.front()->Get_Notes();

	_float fCurSoundPos = m_pGameInstance->GetSoundPosition(TEXT("Bakamita.mp3"), SOUND_BGM);

	for (size_t i = 0; i < pNotes->size(); i++)
	{
		// ���� ��ġ�� �� ũ�� �ǳʶ�
		if ((*pNotes)[i]->Get_StartTime() + MISS_TIME < fCurSoundPos)
			continue;

		// 0.3�� �̳��� �����ϰ������� ����Ʈ��
		if ((*pNotes)[i]->Get_StartTime() - MISS_TIME <= fCurSoundPos && fCurSoundPos <= (*pNotes)[i]->Get_StartTime() + MISS_TIME)
		{
			m_iCheckNoteIndex = i;
			break;
		}
	}

}

void CKaraokeManager::SingleNote_KeyInput(CNoteBase* pNote)
{
	// �ٸ�Ű�� �Է����� ������ Miss�� ������ ������, �ش��ϴ� Ű��ǲ�� �޴´�.
	switch (pNote->Get_ButtonType())
	{
	case 0:  //��
		if (m_pGameInstance->GetKeyState(DIK_UP) == TAP)
		{
			Store_SingleNoteScore(pNote);
			m_pGameInstance->StopAndPlaySound(L"snea_single01.wav", SOUND_UI, 0.5f);
		}
		break;
	case 1:		//�ٿ�
		if (m_pGameInstance->GetKeyState(DIK_DOWN) == TAP)
		{
			Store_SingleNoteScore(pNote);
			m_pGameInstance->StopAndPlaySound(L"snea_single01.wav", SOUND_UI, 0.5f);
		}
		break;
	case 2:		//����Ʈ
		if (m_pGameInstance->GetKeyState(DIK_LEFT) == TAP)
		{
			Store_SingleNoteScore(pNote);
			m_pGameInstance->StopAndPlaySound(L"snea_single01.wav", SOUND_UI, 0.5f);
		}

		break;
	case 3:		//����Ʈ
		if (m_pGameInstance->GetKeyState(DIK_RIGHT) == TAP)
		{
			Store_SingleNoteScore(pNote);
			m_pGameInstance->StopAndPlaySound(L"snea_single01.wav", SOUND_UI, 0.5f);
		}
		break;
	}

	// �ƹ� �Է��� ���ٸ� �̽��� üũ�ϰ���
	Store_Miss(pNote);
}

void CKaraokeManager::LongNote_KeyInput(CNoteBase* pNote)
{
	// �ٸ�Ű�� �Է����� ������ Miss�� ������ ������, �ش��ϴ� Ű��ǲ�� �޴´�.
	switch (pNote->Get_ButtonType())
	{
	case 0:  //��
		if (m_pGameInstance->GetKeyState(DIK_UP) == HOLD)
		{
			Acc_LongNoteScore(pNote);
			m_pGameInstance->PlaySoundIfNotPlay(L"snea_long.wav", SOUND_UI, 0.5f);
		}
		else if (m_pGameInstance->GetKeyState(DIK_UP) == AWAY)
		{
			Store_LongNoteScore(pNote);
			m_pGameInstance->PlaySoundIfNotPlay(L"snea_long.wav", SOUND_UI, 0.5f);
		}
		break;
	case 1:		//�ٿ�
		if (m_pGameInstance->GetKeyState(DIK_DOWN) == HOLD)
		{
			Acc_LongNoteScore(pNote);
			m_pGameInstance->PlaySoundIfNotPlay(L"snea_long.wav", SOUND_UI, 0.5f);
		}
		else if (m_pGameInstance->GetKeyState(DIK_DOWN) == AWAY)
		{
			Store_LongNoteScore(pNote);
			m_pGameInstance->PlaySoundIfNotPlay(L"snea_long.wav", SOUND_UI, 0.5f);
		}
		break;
	case 2:		//����Ʈ
		if (m_pGameInstance->GetKeyState(DIK_LEFT) == HOLD)
		{
			Acc_LongNoteScore(pNote);
			m_pGameInstance->PlaySoundIfNotPlay(L"snea_long.wav", SOUND_UI, 0.5f);
		}
		else if (m_pGameInstance->GetKeyState(DIK_LEFT) == AWAY)
		{
			Store_LongNoteScore(pNote);
			m_pGameInstance->PlaySoundIfNotPlay(L"snea_long.wav", SOUND_UI, 0.5f);
		}
		break;
	case 3:		//����Ʈ
		if (m_pGameInstance->GetKeyState(DIK_RIGHT) == HOLD)
		{
			Acc_LongNoteScore(pNote);
			m_pGameInstance->PlaySoundIfNotPlay(L"snea_long.wav", SOUND_UI, 0.5f);
		}
		else if (m_pGameInstance->GetKeyState(DIK_RIGHT) == AWAY)
		{
			Store_LongNoteScore(pNote);
			m_pGameInstance->PlaySoundIfNotPlay(L"snea_long.wav", SOUND_UI, 0.5f);
		}
		break;
	}
}

void CKaraokeManager::BurstNote_KeyInput(CNoteBase* pNote)
{
	// �ٸ�Ű�� �Է����� ������ Miss�� ������ ������, �ش��ϴ� Ű��ǲ�� �޴´�.
	switch (pNote->Get_ButtonType())
	{
	case 0:  //��
		if (m_pGameInstance->GetKeyState(DIK_UP) == TAP)
		{
			Acc_BurstNoteScore(pNote);
			m_pGameInstance->StopAndPlaySound(L"snea_single01.wav", SOUND_UI, 0.5f);
		}

		break;
	case 1:		//�ٿ�
		if (m_pGameInstance->GetKeyState(DIK_DOWN) == TAP)
		{
			Acc_BurstNoteScore(pNote);
			m_pGameInstance->StopAndPlaySound(L"snea_single01.wav", SOUND_UI, 0.5f);
		}

		break;
	case 2:		//����Ʈ
		if (m_pGameInstance->GetKeyState(DIK_LEFT) == TAP)
		{
			Acc_BurstNoteScore(pNote);
			m_pGameInstance->StopAndPlaySound(L"snea_single01.wav", SOUND_UI, 0.5f);
		}

		break;
	case 3:		//����Ʈ
		if (m_pGameInstance->GetKeyState(DIK_RIGHT) == TAP)
		{
			Acc_BurstNoteScore(pNote);
			m_pGameInstance->StopAndPlaySound(L"snea_single01.wav", SOUND_UI, 0.5f);
		}

		break;
	}
}

void CKaraokeManager::Setting_NoteUIs()
{
	CUIKaraoke_Play* pPlayScene = dynamic_cast<CUIKaraoke_Play*>(m_pUIManager->Find_Scene(TEXT("Karaoke_Play")));
	pPlayScene->Set_Notes(m_Music.front()->Get_Notes());
}

void CKaraokeManager::Check_NextNote()
{
	auto Notes = m_Music.front()->Get_Notes();

	// ��Ʈ ������ �Ѿ�ٸ� üũ�����ʴ´�. (�뷡�� �����ٴ��ǹ�)
	if ((*Notes).size() - 1 < m_iCheckNoteIndex)
		return;

	_float fCurSoundPos = m_pGameInstance->GetSoundPosition(TEXT("Bakamita.mp3"), SOUND_BGM);

	if (((*Notes)[m_iCheckNoteIndex]->Get_StartTime() - fCurSoundPos) <= MISS_TIME2)
	{
		// ���� �ε����� �̹� ����üũ�� �ߴٸ�, �ε����� ������Ų��.
		if ((*Notes)[m_iCheckNoteIndex]->Get_Score() == CNoteBase::NONE)
		{
			// ����üũ�� ���� ���� ��쿡�� �˻��Ѵ�. (�̹� üũ�Ȱ� ������)
			// ��� ��Ʈ�� Ÿ�Կ� ���� Ű��ǲ ����
			switch ((*Notes)[m_iCheckNoteIndex]->Get_Type())
			{
				// �̱۳�Ʈ
			case 0:
				SingleNote_KeyInput((*Notes)[m_iCheckNoteIndex]);
				break;
				// �ճ�Ʈ
			case 1:
				LongNote_KeyInput((*Notes)[m_iCheckNoteIndex]);
				break;
				// ��Ÿ��Ʈ
			case 2:
			{
				BurstNote_KeyInput((*Notes)[m_iCheckNoteIndex]);
				Store_BurstNoteScore((*Notes)[m_iCheckNoteIndex], fCurSoundPos);
				break;
			}
			default:
				break;
			}
		}
	}
}

void CKaraokeManager::Store_SingleNoteScore(CNoteBase* pNote)
{
	_float fCurSoundPos = m_pGameInstance->GetSoundPosition(TEXT("Bakamita.mp3"), SOUND_BGM);

	// 0.2�� ���� �̳���� Great
	if (pNote->Get_StartTime() - fCurSoundPos < GREAT_TIME)
	{
		pNote->Set_Score(CNoteBase::GREAT);
		Show_Grade_UI(pNote);
	}
	else if (pNote->Get_StartTime() - fCurSoundPos < GOOD_TIME)		//Good
	{
		pNote->Set_Score(CNoteBase::GOOD);
		Show_Grade_UI(pNote);
	}
	else if (pNote->Get_StartTime() - fCurSoundPos < MISS_TIME)		//Bad
	{
		pNote->Set_Score(CNoteBase::BAD);
		Show_Grade_UI(pNote);
	}
	else
	{
		pNote->Set_Score(CNoteBase::MISS);
		Show_Grade_UI(pNote);
	}
}

void CKaraokeManager::Acc_LongNoteScore(CNoteBase* pNote)
{
	CNoteLong* pLongNote = dynamic_cast<CNoteLong*>(pNote);

	// Ÿ�ӵ�Ÿ���� ������Ű��, ������ �� ��ŭ �෹�̼����� ������ ����ؾ��ҵ�
	pLongNote->Set_InputAcc(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game")));
}

void CKaraokeManager::Store_LongNoteScore(CNoteBase* pNote)
{
	CNoteLong* pLongNote = dynamic_cast<CNoteLong*>(pNote);

	_float fCurSoundPos = m_pGameInstance->GetSoundPosition(TEXT("Bakamita.mp3"), SOUND_BGM);
	_float fDuration = pLongNote->Get_EndTime() - pLongNote->Get_StartTime();

	// ������ ���� 90�ۼ�Ʈ �̻��̶�� Great
	if ((pLongNote->Get_InputAcc() / fDuration) >= 0.9f)
	{
		pLongNote->Set_Score(CNoteBase::GREAT);
		Show_Grade_UI(pNote);
	}
	else if ((pLongNote->Get_InputAcc() / fDuration) >= 0.7f)
	{
		pLongNote->Set_Score(CNoteBase::GOOD);
		Show_Grade_UI(pNote);
	}
	else if ((pLongNote->Get_InputAcc() / fDuration) >= 0.5f)
	{
		pLongNote->Set_Score(CNoteBase::BAD);
		Show_Grade_UI(pNote);
	}
	else
	{
		pLongNote->Set_Score(CNoteBase::MISS);
		Show_Grade_UI(pNote);
	}
}

void CKaraokeManager::Acc_BurstNoteScore(CNoteBase* pNote)
{
	CNoteBurstHold* pBurstNote = dynamic_cast<CNoteBurstHold*>(pNote);

	pBurstNote->Acc_Count();
}

void CKaraokeManager::Store_BurstNoteScore(CNoteBase* pNote, _float fCurSoundPos)
{
	CNoteBurstHold* pBurstNote = dynamic_cast<CNoteBurstHold*>(pNote);

	if (fCurSoundPos > pBurstNote->Get_EndTime())
	{
		// ������ ���� 90�ۼ�Ʈ �̻��̶�� Great
		if (pBurstNote->Get_InputCount() >= pBurstNote->Get_GreatCount())
		{
			pBurstNote->Set_Score(CNoteBase::GREAT);
			Show_Grade_UI(pNote);
		}
		else if (pBurstNote->Get_InputCount() - 1 >= pBurstNote->Get_GreatCount())
		{
			pBurstNote->Set_Score(CNoteBase::GOOD);
			Show_Grade_UI(pNote);
		}
		else if (pBurstNote->Get_InputCount() - 2 >= pBurstNote->Get_GreatCount())
		{
			pBurstNote->Set_Score(CNoteBase::BAD);
			Show_Grade_UI(pNote);
		}
		else
		{
			pBurstNote->Set_Score(CNoteBase::MISS);
			Show_Grade_UI(pNote);
		}
	}
}

void CKaraokeManager::Store_Miss(CNoteBase* pNote)
{
	if (pNote->Get_Score() == CNoteBase::NONE)
	{
		_float fCurSoundPos = m_pGameInstance->GetSoundPosition(TEXT("Bakamita.mp3"), SOUND_BGM);

		if (fCurSoundPos - pNote->Get_StartTime() >= MISS_TIME)
		{
			pNote->Set_Score(CNoteBase::MISS);
			Show_Grade_UI(pNote);
		}
	}
}

void CKaraokeManager::Show_Grade_UI(CNoteBase* pNote)
{
	CUIKaraoke_Play* pPlayScene = dynamic_cast<CUIKaraoke_Play*>(m_pUIManager->Find_Scene(TEXT("Karaoke_Play")));
	pPlayScene->Show_Grade(pNote);
}

void CKaraokeManager::Open_SelectWindow()
{
	m_pUIManager->Open_Scene(TEXT("Karaoke_Select"));
	m_isStart = true;
}

HRESULT CKaraokeManager::Ready_Karaoke()
{
	CNoteManager* pMusic = CNoteManager::Create("Bakamita");
	
	m_Music.push_back(pMusic);

	return S_OK;
}

CKaraokeManager* CKaraokeManager::Create()
{
	CKaraokeManager* pInstance = new CKaraokeManager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CKaraokeManager make Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKaraokeManager::Free()
{
	Safe_Release(m_pUIManager);
	Safe_Release(m_pGameInstance);

	for (auto& pKaraoke : m_Music)
		Safe_Release(pKaraoke);
	m_Music.clear();
}
