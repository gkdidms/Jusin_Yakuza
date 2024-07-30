#include "TutorialManager.h"

IMPLEMENT_SINGLETON(CTutorialManager)
CTutorialManager::CTutorialManager()
{
}

HRESULT CTutorialManager::Initialize()
{
	return S_OK;
}

void CTutorialManager::Free()
{
}
