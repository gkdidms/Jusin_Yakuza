#include "Chapter7_0.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "Background.h"

CChapter7_0::CChapter7_0()
	: CMainQuest{}
{
}

HRESULT CChapter7_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//씬 카메라 이동 후 스크립트 텍스쳐 나옴
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	return S_OK;
}

//대화가 끝나면 자동으로 다음 스테이지로 넘어감
_bool CChapter7_0::Execute()
{
	if (m_pUIManager->isTalkFinished())
		return true;

	return false;
}

CChapter7_0* CChapter7_0::Create(void* pArg)
{
	CChapter7_0* pInstance = new CChapter7_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter7_0::Free()
{
	__super::Free();
}
