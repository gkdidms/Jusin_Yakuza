#include "UILife.h"

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
	__super::Tick(fTimeDelta);

	return S_OK;
}

HRESULT CUILife::Late_Tick(const _float& fTimeDelta)
{
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
