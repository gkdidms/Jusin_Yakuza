#include "UIMenu.h"

#include "UIManager.h"
#include"GameInstance.h"
CUIMenu::CUIMenu()
	:CUIScene{}
{
}

CUIMenu::CUIMenu(const CUIMenu& rhs)
	:CUIScene{rhs}
{
}

HRESULT CUIMenu::Initialize()
{

	return S_OK;
}

HRESULT CUIMenu::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);



	return S_OK;
}

HRESULT CUIMenu::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	return S_OK;
}

void CUIMenu::Action(_int EventNum)
{
	switch (EventNum)
	{
	case 0://뒤로
	{
		m_pUIManager->Close_Scene();
		break;
	}
	case 1://소지품
	{
		m_pUIManager->Open_Scene(TEXT("Invectory"));
		break;
	}
	case 2://스킬
	{
		m_pUIManager->Open_Scene(TEXT("SkillSelect"));
		break;
	}
	case 3://설정
	{
		m_pUIManager->Open_Scene(TEXT("Setting"));
		break;
	}
	default:
		break;
	}
}

CUIMenu* CUIMenu::Create()
{
	CUIMenu* pInstance = new CUIMenu();
	if(FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIMenu::Free()
{
	__super::Free();
}
