#include "UIFightScore.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUIFightScore::CUIFightScore()
	:CUIScene{}
{
}

CUIFightScore::CUIFightScore(const CUIFightScore& rhs)
	:CUIScene{ rhs }
{
}

HRESULT CUIFightScore::Add_UIData(CUI_Object* pUIObject, wstring wstrPrototypeTag)
{
	if (pUIObject->Get_Event())
	{
		if (m_Bonus.size() < 3)
			m_Bonus.push_back(pUIObject);
		else if (nullptr == m_Money)
			m_Money = static_cast<CGroup*>(pUIObject);
		else if (nullptr == m_AddMoney)
			m_AddMoney = static_cast<CGroup*>(pUIObject);
		else
			m_EventUI.push_back(pUIObject);
		return S_OK;
	}
	else
	{
		m_UI.push_back(pUIObject);
	}

	return S_OK;
}

HRESULT CUIFightScore::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIFightScore::Tick(const _float& fTimeDelta)
{
	if(!m_isEnd)
	{
		m_iCurrentTime += fTimeDelta;

		if (m_iCurrentTime > 5.f)
		{
			m_isEnd = true;
			for (auto& iter : m_EventUI)
				iter->Close_UI();
		}
	}

	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	for (auto& iter : m_EventUI)
		iter->Tick(fTimeDelta);

	m_Money->Tick(fTimeDelta);

	return S_OK;
}

HRESULT CUIFightScore::Late_Tick(const _float& fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	for (auto& iter : m_EventUI)
		iter->Late_Tick(fTimeDelta);

	m_Money->Late_Tick(fTimeDelta);

	return S_OK;
}

_bool CUIFightScore::Click_InterSect()
{
	return _bool();
}

_bool CUIFightScore::Over_InterSect()
{
	return _bool();
}

void CUIFightScore::Action()
{
}

void CUIFightScore::OverAction()
{
}

void CUIFightScore::Set_Money()
{
}

CUIFightScore* CUIFightScore::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUIFightScore* pInstance = new CUIFightScore();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIFightScore::Free()
{
	__super::Free();
}
