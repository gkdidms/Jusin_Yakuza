#include "TutorialManager.h"

#include "UIManager.h"
#include "UITutorial.h"

#include "Tutorial.h"

CTutorialManager::CTutorialManager()
	: m_pUIManager{ CUIManager::GetInstance() }
{
	Safe_AddRef(m_pUIManager);
}

HRESULT CTutorialManager::Initialize()
{
	if (FAILED(Ready_Tutorial()))
		return E_FAIL;

	return S_OK;
}

void CTutorialManager::Tick()
{
	if (m_pUIManager->isShowTutorialUI(CUITutorial::TOTU_NALNARI))
	{
		if (m_pUIManager->isCloseTutorialUIAnim())
			m_isStart = false;
	}

	if (!m_isStart)
	{
		if (m_Tutorials[m_iCurrentIndex]->Execute())
		{
			m_iCurrentIndex++;

			//Ʃ�丮�󺸴� �ε����� �� ũ�ٸ� Ʃ�丮�� ��.
			if (m_iCurrentIndex >= m_Tutorials.size())
				m_isStart = true;
		}
	}
}

void CTutorialManager::Start_Tutorial()
{
	m_pUIManager->Open_Scene(TEXT("Tutorial"));
	m_pUIManager->Change_TutorialUI(CUITutorial::TOTU_NALNARI);
	m_isStart = true;
}

HRESULT CTutorialManager::Ready_Tutorial()
{
	CTutorial::TUTORIAL_DESC Desc{};
	Desc.iConstance = 12;
	Desc.TutorialUI = { CUITutorial::TOTU_RUSHCOMBO };
	Desc.iTutorialCheckUI = CUITutorial::TOTU_RUSHCHECKBOX;
	Desc.strPlayerSkillName = "RushCombo";
	Desc.isFinished = false;

	CTutorial* pTutorial = CTutorial::Create(&Desc);
	if (nullptr == pTutorial)
		return E_FAIL;

	m_Tutorials.emplace_back(pTutorial);

	Desc.iConstance = 3;
	Desc.TutorialUI = { CUITutorial::TOTU_FINISHBLOW };
	Desc.iTutorialCheckUI = CUITutorial::TOTU_FINISHBLOW_CHECKBOX;
	Desc.strPlayerSkillName = "FinishBlow";
	Desc.isFinished = false;

	pTutorial = CTutorial::Create(&Desc);
	if (nullptr == pTutorial)
		return E_FAIL;

	m_Tutorials.emplace_back(pTutorial);

	Desc.iConstance = 6;
	Desc.TutorialUI = { CUITutorial::TOTU_HP, CUITutorial::TOTU_GRAB };
	Desc.iTutorialCheckUI = CUITutorial::TOTU_GRAB_CHECKBOX;
	Desc.strPlayerSkillName = "Grap";
	Desc.isFinished = false;

	pTutorial = CTutorial::Create(&Desc);
	if (nullptr == pTutorial)
		return E_FAIL;

	m_Tutorials.emplace_back(pTutorial);

	Desc.iConstance = 10;
	Desc.TutorialUI = { CUITutorial::TOTU_CLEAR };
	Desc.iTutorialCheckUI = CUITutorial::TOTU_CLEAR_CEHCKBOX;
	Desc.strPlayerSkillName = "Clear";
	Desc.isFinished = true;

	pTutorial = CTutorial::Create(&Desc);
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
	Safe_Release(m_pUIManager);

	for (auto& pTutorial : m_Tutorials)
		Safe_Release(pTutorial);
	m_Tutorials.clear();
}
