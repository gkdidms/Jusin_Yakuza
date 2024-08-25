#include "Chapter4_0.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "FileTotalMgr.h"

#include "Nishiki.h"
#include "Player.h"

CChapter4_0::CChapter4_0()
	: CMainQuest{}
{
}

HRESULT CChapter4_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pNishiki = dynamic_cast<CNishiki*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Nishiki"), 0));
	Safe_AddRef(m_pNishiki);

	m_pNishiki->Set_State(CNishiki::TALK);

	Player_Stop(true);

	m_pFileTotalMgr->Setting_Start_Cinemachine(30);

	return S_OK;
}

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
_bool CChapter4_0::Execute()
{
	if (m_isStart == false)
	{
		if (m_pUIManager->isFindFinished())
		{
			//�� ī�޶� �̵� �� ��ũ��Ʈ �ؽ��� ����
			m_pUIManager->Open_Scene(TEXT("Talk"));
			m_pUIManager->Start_Talk(m_iScriptIndex);

			m_isStart = true;
		}
	}
	else
	{
		if (m_pUIManager->isTalkFinished())
		{
			Player_Stop(false);
			return true;
		}

		if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
		{
			//UIƽ�� �� ���� -> index�� �ϳ� �� �� ������ ���� ��.
			if (m_pUIManager->Get_CurrentPage() == 3)
				m_pFileTotalMgr->Setting_Start_Cinemachine(31);
		}
	}

	return false;
}

CChapter4_0* CChapter4_0::Create(void* pArg)
{
	CChapter4_0* pInstance = new CChapter4_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter4_0::Free()
{
	__super::Free();

	Safe_Release(m_pNishiki);
}
