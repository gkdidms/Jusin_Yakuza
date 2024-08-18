#include "MainQuest.h"

#include "ScriptManager.h"
#include "UIManager.h"

CMainQuest::CMainQuest()
	: m_pUIManager { CUIManager::GetInstance() }
{
	Safe_AddRef(m_pUIManager);
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
}
