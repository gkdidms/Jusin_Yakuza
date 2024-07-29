#include "UIManager.h"
#include "GameInstance.h"
#include "UI_Object.h"
#include "UIScene.h"

#include"UINowLoading.h"
#include "UILoading.h"
#include "UIMenu.h"
#include "UILife.h"
#include "UIMoney.h"
#include "UIInven.h"
#include "UILogo.h"
#include "UIMainMenu.h"
#include "UITutorial.h"
#include "UITalk.h"

#include "InventoryManager.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
	:m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUIManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pInventory = CInventoryManager::Create();//¿øº»
	
	CUIScene::SCENE_DESC Desc{};
	Desc.isLoading = true;

	CUIScene* pScene = CUILoading::Create(m_pDevice ,m_pContext ,&Desc);
	m_AllScene.emplace(make_pair(TEXT("Loading"), pScene));

	 pScene = CUILogo::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("Logo"), pScene));

	pScene = CUINowLoading::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("NowLoading"), pScene));

	pScene = CUIMainMenu::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("MainMenu"), pScene));

	pScene = CUIMenu::Create(m_pDevice, m_pContext);
	m_AllScene.emplace( make_pair(TEXT("Menu"), pScene) );

	pScene = CUILife::Create(m_pDevice, m_pContext);
	m_AllScene.emplace(make_pair(TEXT("Life"), pScene));
	Safe_AddRef(pScene);
	m_AlwaysUI.push_back(pScene);

	pScene = CUIMoney::Create(m_pDevice, m_pContext);
	m_AllScene.emplace(make_pair(TEXT("Money"), pScene));
	Safe_AddRef(pScene);
	m_AlwaysUI.push_back(pScene);



	CUIInven::IVENSCENE_DESC InvenDesc{};

	InvenDesc.pInventory = m_pInventory;

	pScene = CUIInven::Create(m_pDevice, m_pContext ,&InvenDesc);
	m_AllScene.emplace(make_pair(TEXT("Inven"), pScene));

	pScene = CUITutorial::Create(m_pDevice, m_pContext);
	m_AllScene.emplace(make_pair(TEXT("Tutorial"), pScene));

	pScene = CUITalk::Create(m_pDevice, m_pContext);
	m_AllScene.emplace(make_pair(TEXT("Talk"), pScene));


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

	if (pUIScene->Get_isLoading())
	{
		if(!m_PlayScene.empty())
		m_PlayScene.pop_back();
	}
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

	if (!m_PlayScene.empty() )
	{
		if(!m_PlayScene.back()->Get_isLoading())
		{
			for (auto& pUIScene : m_AlwaysUI)
			{
				pUIScene->Tick(fTimeDelta);
			}
		}
	}
	else
	{
		for (auto& pUIScene : m_AlwaysUI)
		{
			pUIScene->Tick(fTimeDelta);
		}
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
		if (!m_PlayScene.empty())
		{
			if (!m_PlayScene.back()->Get_isLoading())
			{
				for (auto& pUIScene : m_AlwaysUI)
				{
					pUIScene->Late_Tick(fTimeDelta);
				}
			}
		}
		else
		{
			for (auto& pUIScene : m_AlwaysUI)
			{
				pUIScene->Late_Tick(fTimeDelta);
			}
		}
	}
#else
	if (!m_PlayScene.empty())
	{
		if (!m_PlayScene.back()->Get_isLoading())
		{
			for (auto& pUIScene : m_AlwaysUI)
			{
				pUIScene->Late_Tick(fTimeDelta);
			}
		}
	}
	else
	{
		for (auto& pUIScene : m_AlwaysUI)
		{
			pUIScene->Late_Tick(fTimeDelta);
		}
	}
#endif // _DEBUG

	return S_OK;
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

	m_AlwaysUI.clear();

	for (auto iter : m_AllScene)
	{
		Safe_Release(iter.second);
	}
	m_AllScene.clear();

	Safe_Release(m_pInventory);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}

void CUIManager::Start_Talk()
{
	if (!m_PlayScene.empty())
	{
		dynamic_cast<CUITalk*>(m_PlayScene.back())->Start_Talk();
	}
}
