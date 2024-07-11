#include "UILife.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
#include"InventoryManager.h"
CUILife::CUILife()
	:CUIScene{}
{
}

CUILife::CUILife(const CUILife& rhs)
	:CUIScene{rhs}
{
}

HRESULT CUILife::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CUILife::Tick(const _float& fTimeDelta)
{
	if (!m_EventUI.empty())
		m_EventUI[m_iBts]->Tick(fTimeDelta);
	__super::Tick(fTimeDelta);

	return S_OK;
}

HRESULT CUILife::Late_Tick(const _float& fTimeDelta)
{
	if (!m_EventUI.empty())
		m_EventUI[m_iBts]->Late_Tick(fTimeDelta);
	__super::Late_Tick(fTimeDelta);

	return S_OK;
}

CUILife* CUILife::Create(void* pArg)
{
	CUILife* pInstance = new CUILife();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUILife::Free()
{
	__super::Free();
}
