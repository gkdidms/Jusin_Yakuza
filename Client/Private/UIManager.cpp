#include "UIManager.h"
#include "GameInstance.h"
#include "UI_Object.h"
#include "UIScene.h"

#include "UIMenu.h"
#include "UILife.h"
#include "UIMoney.h"
#include "UIInven.h"
#include "InventoryManager.h"
IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
	:m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUIManager::Initialize()
{
	m_pInventory = CInventoryManager::Create();//¿øº»
	
	CUIScene* pScene = CUIMenu::Create();
	m_AllScene.emplace( make_pair(TEXT("Menu"), pScene) );

	pScene = CUILife::Create();
	m_AllScene.emplace(make_pair(TEXT("Life"), pScene));
	m_AlwaysUI.push_back(pScene);
	pScene = CUIMoney::Create();
	m_AllScene.emplace(make_pair(TEXT("Money"), pScene));
	m_AlwaysUI.push_back(pScene);

	CUIInven::IVENSCENE_DESC Desc{};
	Desc.pInventory = m_pInventory;

	pScene = CUIInven::Create(&Desc);
	m_AllScene.emplace(make_pair(TEXT("Inven"), pScene));

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

	m_PlayScene.push_back(pUIScene);
	m_PlayScene.back()->Show_Scene();
	m_isClose = false;
}

void CUIManager::Close_Scene()
{
	if (!m_PlayScene.empty())
	{
		m_PlayScene.back()->Close_Scene();
		m_isClose = true;
	}
}

void CUIManager::Click()
{
	if (!m_PlayScene.empty())
		m_PlayScene.back()->Click_InterSect();
}

HRESULT CUIManager::Tick(const _float& fTimeDelta)
{
	if(!m_PlayScene.empty())
	{
		m_PlayScene.back()->Tick(fTimeDelta);
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
		if (m_PlayScene.back()->Get_isClose()&& m_PlayScene.back()->Get_isAnimFinish())
		{
			m_PlayScene.pop_back();
		}

		if (!m_PlayScene.empty())
		{
			m_PlayScene.back()->Late_Tick(fTimeDelta);
			
		}
			
		
	}

#ifdef _DEBUG
	if (m_isRender)
	{
		for (auto& pUIScene : m_AlwaysUI)
		{
			pUIScene->Late_Tick(fTimeDelta);
		}
		return S_OK;
	}
#else
	for (auto& pUIScene : m_AlwaysUI)
	{
		pUIScene->Late_Tick(fTimeDelta);
	}
	return S_OK;
#endif // _DEBUG
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

	for (auto iter : m_PlayScene)
	{
		Safe_Release(iter);
	}
	for (auto iter : m_AllScene)
	{
		Safe_Release(iter.second);
	}
	m_AllScene.clear();

	Safe_Release(m_pInventory);
	Safe_Release(m_pGameInstance);
}
