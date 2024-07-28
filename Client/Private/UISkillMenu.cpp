#include "UISkillMenu.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"	
#include"GameInstance.h"
CUISkillMenu::CUISkillMenu()
	:CUIScene{}
{
}

CUISkillMenu::CUISkillMenu(const CUISkillMenu& rhs)
	:CUIScene{ rhs }
{
}

HRESULT CUISkillMenu::Show_Scene()
{
	__super::Show_Scene();

	for (size_t i = 0; i < m_Button.size(); i++)
	{
		_matrix ButtonWorld = m_Button[i]->Get_TransformCom()->Get_WorldMatrix();
		_vector Position = ButtonWorld.r[3];
		ButtonWorld = XMMatrixTranslation(XMVectorGetX(Position), XMVectorGetY(Position), 0.f);


		m_EventUI[i * 2]->Close_UI();
		m_EventUI[i * 2]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);

		m_EventUI[i * 2 + 1]->Close_UI();
		m_EventUI[i * 2 + 1]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);
	}

	return S_OK;
}

HRESULT CUISkillMenu::Close_Scene()
{
	__super::Close_Scene();

	return S_OK;
}

HRESULT CUISkillMenu::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISkillMenu::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& iter : m_EventUI)
	{
		iter->Tick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CUISkillMenu::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto& iter : m_EventUI)
	{
		iter->Late_Tick(fTimeDelta);
	}

	if (!m_isAnimFin)
		Check_AimFin();
	else
	{
		if (Over_InterSect())
		{
			OverAction();
		}
	}

	return S_OK;
}

void CUISkillMenu::Action()
{
	switch (m_iCurButton)
	{
	case 0 ://불한당스킬창
		CUIManager::GetInstance()->Close_Scene();
		break;
	case 1://러쉬 스킬창
		break;
	case 2://파괴자 스킬창
		break;
	default:
		break;
	}
}

void CUISkillMenu::OverAction()
{

	if (m_iCurButton != m_iPrevButton)
	{
		m_EventUI[m_iCurButton * 2]->Show_UI();
		m_EventUI[m_iCurButton * 2 + 1]->Show_UI();
		if (-1 != m_iPrevButton)
		{
			m_EventUI[m_iPrevButton * 2]->Close_UI();
			m_EventUI[m_iPrevButton * 2 + 1]->Close_UI();
		}
	}

}

CUISkillMenu* CUISkillMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUISkillMenu* pInstance = new CUISkillMenu();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		MSG_BOX("Make Failed :CUISkillMenu");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUISkillMenu::Free()
{
	__super::Free();
}
