#include "Chapter1_0.h"

#include "GameInstance.h"

#include "UIManager.h"
#include "ScriptManager.h"
#include "UITutorial.h"

#include "Background.h"

CChapter1_0::CChapter1_0()
	: CMainQuest{}
{
}

HRESULT CChapter1_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//백그라운드 생성
	if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_BackGround"), TEXT("Layer_StoryImage"), nullptr)))
		return E_FAIL;

	m_pBackground = dynamic_cast<CBackground*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_StoryImage"), 0));
	Safe_AddRef(m_pBackground);

	//씬 카메라 이동 후 스크립트 텍스쳐 나옴
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex); // 0번째 대화

	return S_OK;
}

_bool CChapter1_0::Execute()
{
	if (m_isTitleFinished)
		return true;

	if (m_pUIManager->isTalkFinished())
	{
		if (!m_isTitleStart) // 타이틀 띄우기
		{
			m_pUIManager->Open_Scene(TEXT("Title"));
			m_pUIManager->Set_TitleStart(true); // 니시키야마를 찾아라
			m_pUIManager->Start_Title(14);
			m_isTitleStart = true;
		}
		else
		{
			m_isTitleFinished = true;
		}
	}
	else 
	{
		if (m_pUIManager->Get_CurrentPage() == 1)
		{
			if (!m_pBackground->isShow())
				m_pBackground->Set_Show(true);

			return false;
		}

		else if (m_pUIManager->Get_CurrentPage() == 2)
		{
			if (m_pBackground->isShow())
				m_pBackground->Set_Show(false);

			return false;
		}
	}

	return false;
}

CChapter1_0* CChapter1_0::Create(void* pArg)
{
	CChapter1_0* pInstance = new CChapter1_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter1_0::Free()
{
	__super::Free();

	Safe_Release(m_pBackground);
}
