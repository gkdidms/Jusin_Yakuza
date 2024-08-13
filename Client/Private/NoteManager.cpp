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
		MSG_BOX("Note 데이터 로드 실패");
#endif // _DEBUG
		return E_FAIL;
	}

	string line;
	// 헤더를 건너뜁니다.
	getline(in, line);

	while (getline(in, line)) {
		istringstream lineStream(line);
		string cell;

		/*
		*	0 일반노드
			1 유지 노드
			2 연타 노드 
		*/

		// 노트 타입
		getline(lineStream, cell, ',');
		if (cell == "0")
		{
			// 넘겨주는 인자값을 파일에 저장해둔 값을 넘겨주고, 이니셜라이즈 할 때 저장해야한다.
			m_Notes.push_back(CNoteSingle::Create());
		}
		else if (cell == "1")
		{
			// 넘겨주는 인자값을 파일에 저장해둔 값을 넘겨주고, 이니셜라이즈 할 때 저장해야한다.
			m_Notes.push_back(CNoteLong::Create());
		}
		else if (cell == "2")
		{
			// 넘겨주는 인자값을 파일에 저장해둔 값을 넘겨주고, 이니셜라이즈 할 때 저장해야한다.
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