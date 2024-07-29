#include "UISkillHolligan.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"	
#include"GameInstance.h"
CUISkillHolligan::CUISkillHolligan()
	:CUIScene{}
{
}

CUISkillHolligan::CUISkillHolligan(const CUISkillHolligan& rhs)
	:CUIScene{ rhs }
{
}

HRESULT CUISkillHolligan::Add_UIData(CUI_Object* pUIObject)
{
	if (pUIObject->Get_Event())
	{
		if(m_pBall.empty()||m_pBall.size()<8)
			m_pBall.push_back(pUIObject);
		else
			m_EventUI.push_back(pUIObject);

		return S_OK;
	}
	else
	{
		m_UI.push_back(pUIObject);
	}

	if (CUI_Object::TYPE_BTN == pUIObject->Get_TypeIndex())
	{
		Safe_AddRef(pUIObject);
		m_Button.push_back(dynamic_cast<CBtn*>(pUIObject));
	}


	return S_OK;
}

HRESULT CUISkillHolligan::Show_Scene()
{
	__super::Show_Scene();

	for (auto& iter : m_pBall)
		iter->Close_UI();

	for (auto& iter : m_EventUI)
		iter->Close_UI();
	return S_OK;
}

HRESULT CUISkillHolligan::Close_Scene()
{
	__super::Close_Scene();

	return S_OK;
}

HRESULT CUISkillHolligan::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISkillHolligan::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& iter : m_pBall)
		iter->Tick(fTimeDelta);


	if (-1 != m_iCurButton)
	{
		m_EventUI[0]->Tick(fTimeDelta);
		m_EventUI[m_iCurButton + 1]->Tick(fTimeDelta);

		if (m_isSkill[m_iCurButton])
			m_EventUI[9]->Tick(fTimeDelta);
	}


	
	return S_OK;
}

HRESULT CUISkillHolligan::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto& iter : m_pBall)
		iter->Late_Tick(fTimeDelta);

	if(-1!=m_iCurButton)
	{
		m_EventUI[0]->Late_Tick(fTimeDelta);
		m_EventUI[m_iCurButton + 1]->Late_Tick(fTimeDelta);

		if (m_isSkill[m_iCurButton])
			m_EventUI[9]->Late_Tick(fTimeDelta);

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

_bool CUISkillHolligan::Click_InterSect()
{
	for (size_t i = 0; i < m_pBall.size(); i++)
	{
		if (m_pBall[i]->Click_Intersect(0))
		{
			m_iCurButton = i;
			Action();
			return true;
		}
	}
	return false;
}

_bool CUISkillHolligan::Over_InterSect()
{
	for (size_t i = 0; i < m_pBall.size(); i++)
	{
		if (m_pBall[i]->Click_Intersect(0))
		{
			m_iCurButton = i;
			if (m_iPrevButton != m_iCurButton)
			{
				OverAction();
				m_iPrevButton = m_iCurButton;
				return true;
			}
			else
				return false;
		}
	}
	return false;
}

void CUISkillHolligan::Action()
{

	m_isSkill[m_iCurButton] = true;	
	m_pBall[m_iCurButton]->Show_UI();	
	
}

void CUISkillHolligan::OverAction()
{
	if (m_iCurButton != m_iPrevButton)
	{
		m_EventUI[0]->Show_UI();
		m_EventUI[m_iCurButton+1]->Show_UI();
		if (-1 != m_iPrevButton)
		{
			m_EventUI[m_iPrevButton + 1]->Close_UI();
		}
		if (m_isSkill[m_iCurButton])
		{
			m_EventUI[9]->Show_UI();
		}
	}

}

void CUISkillHolligan::Set_Skill(_int Index)
{

}

CUISkillHolligan* CUISkillHolligan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUISkillHolligan* pInstance = new CUISkillHolligan();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		MSG_BOX("Make Failed :CUISkillHolligan");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUISkillHolligan::Free()
{
	__super::Free();
	for (auto& iter : m_pBall)
		Safe_Release(iter);
}
