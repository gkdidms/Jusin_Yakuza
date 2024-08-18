#include "UIQTE.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"

CUIQTE::CUIQTE()
	:CUIScene{}
{
}

CUIQTE::CUIQTE(const CUIQTE& rhs)
	:CUIScene{ rhs }
{
}

HRESULT CUIQTE::Add_UIData(CUI_Object* pUIObject, wstring wstrPrototypeTag)
{
	if (pUIObject->Get_Event())
	{
		m_EventUI.push_back(pUIObject);

	}
	else
	{
		m_UI.push_back(pUIObject);
	}

	return S_OK;
}

HRESULT CUIQTE::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQTE::Tick(const _float& fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQTE::Late_Tick(const _float& fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;
	return S_OK;
}

CUIQTE* CUIQTE::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUIQTE* pInstance = new CUIQTE();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIQTE::Free()
{
	__super::Free();
}
