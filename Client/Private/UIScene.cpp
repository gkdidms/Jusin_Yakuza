#include "UIScene.h"
#include "UI_Object.h"
#include "Btn.h"
#include "UIManager.h"
#include"GameInstance.h"
CUIScene::CUIScene()
	:m_pGameInstance{CGameInstance::GetInstance()},
	m_pUIManager{CUIManager::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pUIManager);
}

CUIScene::CUIScene(const CUIScene& rhs)
{

}

HRESULT CUIScene::Add_UIData(CUI_Object* pUIObject)
{
	m_UI.push_back(pUIObject);

	if (CUI_Object::TYPE_BTN == pUIObject->Get_TypeIndex())
	{
		m_Button.push_back(dynamic_cast<CBtn*>(pUIObject));	
	}

	return S_OK;
}

HRESULT CUIScene::Show_Scene()
{
	for (auto& iter : m_UI)
	{
		iter->Show_UI();
	}
	m_isAnimFin = false;
	m_isClose = false;
	return S_OK;
}

HRESULT CUIScene::Close_Scene()
{
	for (auto& iter : m_UI)
	{
		iter->Close_UI();
	}
	m_isAnimFin = false;
	m_isClose = true;
	return S_OK;
}

HRESULT CUIScene::Initialize()
{
	return S_OK;
}

HRESULT CUIScene::Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_UI)
	{
		iter->Tick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CUIScene::Late_Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_UI)
		iter->Late_Tick(fTimeDelta);

	if (!m_isAnimFin)
		Check_AimFin();

	return S_OK;
}

void CUIScene::Check_AimFin()
{
	for (auto& iter : m_UI)
		if (!iter->Check_AnimFin())
		{
			m_isAnimFin = false;
			return;
		}
	
	m_isAnimFin = true;
}

_bool CUIScene::Click_InterSect()
{
	for (size_t i = 0; i < m_Button.size(); i++)
	{
		if (m_Button[i]->Click_Intersect())
		{
			Action(i);
			return true;
		}
	}
	return false;
}

void CUIScene::Action(_int EventNum)
{
}

void CUIScene::Free()
{
	for (auto iter : m_UI)
		Safe_Release(iter);

	m_UI.clear();

	for (auto iter : m_Button)
		Safe_Release(iter);

	m_Button.clear();


	Safe_Release(m_pGameInstance);
	Safe_Release(m_pUIManager);
}
