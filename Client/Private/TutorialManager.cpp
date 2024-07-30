#include "TutorialManager.h"

#include "Tutorial.h"

CTutorialManager::CTutorialManager()
{
}

HRESULT CTutorialManager::Initialize()
{
	if (FAILED(Ready_Tutorial()))
		return E_FAIL;

	return S_OK;
}

void CTutorialManager::Tick()
{
	if (m_Tutorials[m_iCurrentIndex]->Execute())
	{
		m_iCurrentIndex++;
	}
}

HRESULT CTutorialManager::Ready_Tutorial()
{
	CTutorial::TUTORIAL_DESC Desc{};
	Desc.iConstance = 10;
	Desc.strPlayerSkillName = "";

	CTutorial* pTutorial = CTutorial::Create(&Desc);
	if (nullptr == pTutorial)
		return E_FAIL;

	m_Tutorials.emplace_back(pTutorial);

	return S_OK;
}

CTutorialManager* CTutorialManager::Create()
{
	CTutorialManager* pInstance = new CTutorialManager();

	if (FAILED(pInstance->Initialize()))
		Safe_Release(pInstance);

	return pInstance;
}

void CTutorialManager::Free()
{
	for (auto& pTutorial : m_Tutorials)
		Safe_Release(pTutorial);
	m_Tutorials.clear();
}
