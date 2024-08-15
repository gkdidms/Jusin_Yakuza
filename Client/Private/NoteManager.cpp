#include "NoteManager.h"
#include "GameInstance.h"

#include "NoteSingle.h"
#include "NoteLong.h"
#include "NoteBurstHold.h"

CNoteManager::CNoteManager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CNoteManager::Initialize(string strSongName)
{
	Load_NoteData(strSongName);
    return S_OK;
}


HRESULT CNoteManager::Load_NoteData(string strSongName)
{
	string pBinFilePath = "../Bin/DataFiles/KaraokeNoteData/" + strSongName + "_Note.csv";

	ifstream in(pBinFilePath, ios::binary);

	if (!in.is_open()) {

#ifdef _DEBUG
		MSG_BOX("Note ������ �ε� ����");
#endif // _DEBUG
		return E_FAIL;
	}

	string line;
	// ����� �ǳʶݴϴ�.
	getline(in, line);

	while (getline(in, line)) {
		istringstream lineStream(line);
		string strNoteType, Button, TimePos_S, TimePos_E;

		/*
		*	0 �Ϲݳ��
			1 ���� ���
			2 ��Ÿ ��� 
		*/

		// ��Ʈ Ÿ��
		getline(lineStream, strNoteType, ',');
		getline(lineStream, Button, ',');
		getline(lineStream, TimePos_S, ',');
		getline(lineStream, TimePos_E, ',');


		if (strNoteType == "0")//�Ϲ�
		{
			// �Ѱ��ִ� ���ڰ��� ���Ͽ� �����ص� ���� �Ѱ��ְ�, �̴ϼȶ����� �� �� �����ؾ��Ѵ�.
			m_Notes.push_back(CNoteSingle::Create(stoi(strNoteType), stoi(Button), stof(TimePos_S)));
		}
		else if (strNoteType == "1")//����
		{
			// �Ѱ��ִ� ���ڰ��� ���Ͽ� �����ص� ���� �Ѱ��ְ�, �̴ϼȶ����� �� �� �����ؾ��Ѵ�.
			m_Notes.push_back(CNoteLong::Create(stoi(strNoteType), stoi(Button), stof(TimePos_S), stof(TimePos_E)));
		}
		else if (strNoteType == "2")//��Ÿ
		{
			// �Ѱ��ִ� ���ڰ��� ���Ͽ� �����ص� ���� �Ѱ��ְ�, �̴ϼȶ����� �� �� �����ؾ��Ѵ�.
			m_Notes.push_back(CNoteBurstHold::Create(stoi(strNoteType), stoi(Button), stof(TimePos_S), stof(TimePos_E)));
		}

	}

	return S_OK;
}

CNoteManager* CNoteManager::Create(string strSongName)
{
	CNoteManager* pInstance = new CNoteManager;
	if (FAILED(pInstance->Initialize(strSongName)))
	{
		MSG_BOX("NoteManager make Failed");
		return nullptr;
	}
	return pInstance;
}

void CNoteManager::Free()
{
    Safe_Release(m_pGameInstance);

	for (auto& pNote : m_Notes)
		Safe_Release(pNote);
	m_Notes.clear();
}