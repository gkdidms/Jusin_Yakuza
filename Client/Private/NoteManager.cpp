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

HRESULT CNoteManager::Initialize()
{
    return S_OK;
}


void CNoteManager::Tick()
{
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
		string cell;

		/*
		*	0 �Ϲݳ��
			1 ���� ���
			2 ��Ÿ ��� 
		*/

		// ��Ʈ Ÿ��
		getline(lineStream, cell, ',');
		if (cell == "0")
		{
			// �Ѱ��ִ� ���ڰ��� ���Ͽ� �����ص� ���� �Ѱ��ְ�, �̴ϼȶ����� �� �� �����ؾ��Ѵ�.
			m_Notes.push_back(CNoteSingle::Create());
		}
		else if (cell == "1")
		{
			// �Ѱ��ִ� ���ڰ��� ���Ͽ� �����ص� ���� �Ѱ��ְ�, �̴ϼȶ����� �� �� �����ؾ��Ѵ�.
			m_Notes.push_back(CNoteLong::Create());
		}
		else if (cell == "2")
		{
			// �Ѱ��ִ� ���ڰ��� ���Ͽ� �����ص� ���� �Ѱ��ְ�, �̴ϼȶ����� �� �� �����ؾ��Ѵ�.
			m_Notes.push_back(CNoteBurstHold::Create());
		}

	}

	return S_OK;
}

void CNoteManager::Free()
{
    Safe_Release(m_pGameInstance);

	for (auto& pNote : m_Notes)
		Safe_Release(pNote);
	m_Notes.clear();
}