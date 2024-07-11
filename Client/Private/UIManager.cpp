#include "UIManager.h"
#include "GameInstance.h"
#include "UI_Object.h"
#include "UIScene.h"

#include "UIMenu.h"
#include "UILife.h"
#include "UIMoney.h"
IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
	:m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUIManager::Initialize()
{
	CUIScene* pScene = CUIMenu::Create();
	m_AllScene.emplace( make_pair(TEXT("Menu"), pScene) );

	pScene = CUILife::Create();
	m_AllScene.emplace(make_pair(TEXT("Life"), pScene));

	pScene = CUIMoney::Create();
	m_AllScene.emplace(make_pair(TEXT("Money"), pScene));

	return S_OK;
}

HRESULT CUIManager::Add_Data(const wstring strSceneName, wstring strProtoName)
{
	CUIScene* pScene = Find_Scene(strSceneName);

	if (nullptr == pScene)	
		return E_FAIL;
	

	if (FAILED(pScene->Add_UIData(dynamic_cast<CUI_Object*>(m_pGameInstance->Clone_Object(strProtoName, nullptr)))))
		return E_FAIL;
	
	return S_OK;
}

void CUIManager::Open_Scene(const wstring strSceneName)
{
	CUIScene* pUIScene = Find_Scene(strSceneName);	

	m_PlayScene.push(pUIScene);
	m_PlayScene.top()->Show_Scene();
}

void CUIManager::Close_Scene()
{
	if(!m_PlayScene.empty())
		m_PlayScene.top()->Close_Scene();	
}

HRESULT CUIManager::Tick(const _float& fTimeDelta)
{
	if(!m_PlayScene.empty())
	{
		m_PlayScene.top()->Tick(fTimeDelta);
	}

	for (auto& pUIScene : m_AlwaysUI)
	{
		pUIScene->Tick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CUIManager::Late_Tick(const _float& fTimeDelta)
{
	if (!m_PlayScene.empty())
	{
		if (m_PlayScene.top()->Get_isClose()&& m_PlayScene.top()->Get_isAnimFinish())
		{

			m_PlayScene.pop();
		}
		else
		{
			m_PlayScene.top()->Late_Tick(fTimeDelta);
		}
	}

	for (auto& pUIScene : m_AlwaysUI)
	{
		pUIScene->Late_Tick(fTimeDelta);
	}
	
	return S_OK;
}

void CUIManager::Click_InterSect()
{
	if(m_PlayScene.top()->Get_isAnimFinish())
		m_PlayScene.top()->Click_InterSect();
}



CUIScene* CUIManager::Find_Scene(wstring strSceneName)
{
	auto PairScene =m_AllScene.find(strSceneName);
	
	if (m_AllScene.end() == PairScene)
		return nullptr;
	
	return PairScene->second;
}

void CUIManager::Free()
{
	for (auto iter : m_AlwaysUI)
	{
		Safe_Release(iter);
	}

	for (size_t i = 0; i < m_PlayScene.size(); i++)
	{
		Safe_Release(m_PlayScene.top());
		m_PlayScene.pop();
	}

	for (auto iter : m_AllScene)
	{
		Safe_Release(iter.second);
	}
	m_AllScene.clear();

	Safe_Release(m_pGameInstance);
}
