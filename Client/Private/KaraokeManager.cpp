#include "KaraokeManager.h"
#include "UIManager.h"
#include "NoteManager.h"
#include "GameInstance.h"
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


	return S_OK;
}

void CKaraokeManager::Tick(const _float& fTimeDelta)
{
	if(!m_isStart)
		KeyInput(); //이거 카라오케 키는 용도
	else
	{
	//여기서 ui랑 노트랑 같이 사용해서 플레이해주자,키받기 등등
		if (m_pGameInstance->GetKeyState(DIK_O) == TAP)//이건 노래 끝났다는 전제하
		{
			m_pUIManager->Open_Scene(TEXT("Karaoke_Score"));
			m_isStart = false;

			//아래 점수 주면됨 현재 점수
			m_iCurrentScore = 80;
			m_pUIManager->Set_Score(m_iCurrentScore);
		}
	}

}

void CKaraokeManager::KeyInput()
{
	if (m_pGameInstance->GetKeyState(DIK_K) == TAP)
	{
		m_pUIManager->Open_Scene(TEXT("Karaoke_Select"));
		m_isStart = true;
	}
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
