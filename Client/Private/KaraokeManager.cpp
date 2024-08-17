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
		KeyInput(); //�̰� ī����� Ű�� �뵵
	else
	{
	//���⼭ ui�� ��Ʈ�� ���� ����ؼ� �÷���������,Ű�ޱ� ���
		if (m_pGameInstance->GetKeyState(DIK_O) == TAP)//�̰� �뷡 �����ٴ� ������
		{
			m_pUIManager->Open_Scene(TEXT("Karaoke_Score"));
			m_isStart = false;

			//�Ʒ� ���� �ָ�� ���� ����
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
