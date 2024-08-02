#include "UITutorial.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"	
#include"GameInstance.h"
CUITutorial::CUITutorial()
	:CUIScene{}
{
}

CUITutorial::CUITutorial(const CUITutorial& rhs)
	:CUIScene{ rhs }
{
}

_bool CUITutorial::isCloseCurrentUIAnim()
{
	return m_EventUI[m_iState]->Check_AnimFin();
}

void CUITutorial::Set_State(_uint iState)
{
	m_iState = iState;

	m_EventUI[m_iState]->Show_UI();
}

void CUITutorial::Set_Text(wstring strText)
{
	vector<CUI_Object*> Objects = dynamic_cast<CGroup*>(m_EventUI[m_iState])->Get_PartObjects();
	CText* pText = dynamic_cast<CText*>(Objects.back());

	pText->Set_Text(strText);
}

HRESULT CUITutorial::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial::Tick(const _float& fTimeDelta)
{
	//__super::Tick(fTimeDelta);
	
	m_EventUI[m_iState]->Tick(fTimeDelta);

	return S_OK;
}

HRESULT CUITutorial::Late_Tick(const _float& fTimeDelta)
{
	//__super::Late_Tick(fTimeDelta);

	m_EventUI[m_iState]->Late_Tick(fTimeDelta);

	if (!m_isAnimFin)
		Check_AimFin();

	return S_OK;
}

void CUITutorial::Action()
{
}

void CUITutorial::OverAction()
{
}

CUITutorial* CUITutorial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUITutorial* pInstance = new CUITutorial();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		MSG_BOX("Make Failed :CUITutorial");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUITutorial::Free()
{
	__super::Free();
}
