#include "Chapter1_1.h"

#include "UIManager.h"
#include "ScriptManager.h"
#include "UITutorial.h"

CChapter1_1::CChapter1_1()
	: CMainQuest{}
{
}

HRESULT CChapter1_1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//씬 카메라 이동 후 스크립트 텍스쳐 나옴
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	return S_OK;
}

_bool CChapter1_1::Execute()
{
	if (m_pUIManager->isTalkFinished())
		return true;

	return false;
}

CChapter1_1* CChapter1_1::Create(void* pArg)
{
	CChapter1_1* pInstance = new CChapter1_1();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter1_1::Free()
{
	__super::Free();
}
