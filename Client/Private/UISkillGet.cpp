#include "UISkillGet.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"	
#include"GameInstance.h"
CUISkillGet::CUISkillGet()
	:CUIScene{}
{
}

CUISkillGet::CUISkillGet(const CUISkillGet& rhs)
	:CUIScene{ rhs }
{
}

HRESULT CUISkillGet::Show_Scene()
{
	for (auto& iter : m_UI)
	{
		if (CUI_Object::TYPE_BTN != iter->Get_TypeIndex())
			iter->Show_UI();
		else
			iter->Close_UI();
	}

	//for (auto& iter : m_EventUI)
	//{
	//	if (CUI_Object::TYPE_BTN != iter->Get_TypeIndex())
	//		iter->Show_UI();
	//	else
	//		iter->Close_UI();
	//}
	m_isAnimFin = false;
	m_isClose = false;


	return S_OK;
}

HRESULT CUISkillGet::Close_Scene()
{
	for (auto& iter : m_UI)
	{
		iter->Close_UI();
	}

	//for (auto& iter : m_EventUI)
	//{
	//	if (CUI_Object::TYPE_BTN != iter->Get_TypeIndex())
	//		iter->Close_UI();
	//	else
	//		iter->Show_UI();
	//}
	m_isAnimFin = false;
	m_isClose = true;
	m_iCurButton = -1;
	m_iPrevButton = -1;
	return S_OK;
}

HRESULT CUISkillGet::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISkillGet::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
	return S_OK;
}

HRESULT CUISkillGet::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (!m_isAnimFin)
		Check_AimFin();
	return S_OK;
}

_bool CUISkillGet::Click_InterSect()
{
	return _bool();
}

_bool CUISkillGet::Over_InterSect()
{
	return _bool();
}

void CUISkillGet::Action()
{
}

void CUISkillGet::OverAction()
{
}

CUISkillGet* CUISkillGet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUISkillGet* pInstance = new CUISkillGet();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		MSG_BOX("Make Failed :CUISkillGet");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUISkillGet::Free()
{
	__super::Free();
}
