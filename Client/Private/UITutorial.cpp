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

HRESULT CUITutorial::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& iter : m_EventUI)
		iter->Tick(fTimeDelta);


	return S_OK;
}

HRESULT CUITutorial::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto& iter : m_EventUI)
		iter->Late_Tick(fTimeDelta);

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
