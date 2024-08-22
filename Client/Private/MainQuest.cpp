#include "MainQuest.h"

#include "ScriptManager.h"
#include "UIManager.h"
#include "FileTotalMgr.h"

CMainQuest::CMainQuest()
	: m_pUIManager { CUIManager::GetInstance() },
	m_pFileTotalMgr{ CFileTotalMgr::GetInstance() }
{
	Safe_AddRef(m_pUIManager);
	Safe_AddRef(m_pFileTotalMgr);
}

HRESULT CMainQuest::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MAIN_QUEST_DESC* pDesc = static_cast<MAIN_QUEST_DESC*>(pArg);
	m_iScriptIndex = pDesc->iScriptIndex;

	return S_OK;
}

void CMainQuest::Free()
{
	Safe_Release(m_pUIManager);
	Safe_Release(m_pFileTotalMgr);
}
