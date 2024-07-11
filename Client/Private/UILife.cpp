#include "UILife.h"

CUILife::CUILife()
	:CUIScene{}
{
}

CUILife::CUILife(const CUILife& rhs)
	:CUIScene{rhs}
{
}

HRESULT CUILife::Initialize()
{
	return S_OK;
}

HRESULT CUILife::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	return S_OK;
}

HRESULT CUILife::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	return S_OK;
}

CUILife* CUILife::Create()
{
	CUILife* pInstance = new CUILife();
	if (FAILED(pInstance->Initialize()))
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
