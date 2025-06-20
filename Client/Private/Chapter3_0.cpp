#include "Chapter3_0.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "FileTotalMgr.h"

#include "Player.h"
#include "Nishiki.h"

CChapter3_0::CChapter3_0()
	: CMainQuest{}
{
}

HRESULT CChapter3_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	m_pNishiki = dynamic_cast<CNishiki*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Nishiki"), 0));
	Safe_AddRef(m_pNishiki);
	m_pNishiki->Set_State(CNishiki::TALK);

	Player_Stop(true);
	m_pGameInstance->Set_Vignette(true);

	m_pFileTotalMgr->Setting_Start_Cinemachine(19);

	return S_OK;
}

//대화가 끝나면 자동으로 다음 스테이지로 넘어감
_bool CChapter3_0::Execute()
{
	if (m_isStart == false)
	{
		if (m_pUIManager->isFindFinished())
		{
			//씬 카메라 이동 후 스크립트 텍스쳐 나옴
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
			m_pGameInstance->Set_Vignette(false);
			m_pNishiki->Set_State(CNishiki::IDLE);
			return true;
		}

		if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
		{
			//UI틱이 더 느림 -> index를 하나 씩 더 느리게 봐야 함.
			if (m_pUIManager->Get_CurrentPage() == 1)
				m_pFileTotalMgr->Setting_Start_Cinemachine(20);
			else if (m_pUIManager->Get_CurrentPage() == 2)
				m_pFileTotalMgr->Setting_Start_Cinemachine(21);
			else if (m_pUIManager->Get_CurrentPage() == 5)
				m_pFileTotalMgr->Setting_Start_Cinemachine(22);
		}
	}

	return false;
}

CChapter3_0* CChapter3_0::Create(void* pArg)
{
	CChapter3_0* pInstance = new CChapter3_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter3_0::Free()
{
	__super::Free();

	Safe_Release(m_pNishiki);
}
