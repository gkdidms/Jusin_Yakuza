#include "UIMoney.h"

CUIMoney::CUIMoney()
	:CUIScene{}
{
}

CUIMoney::CUIMoney(const CUIMoney& rhs)
	:CUIScene{rhs}
{
}

HRESULT CUIMoney::Initialize()
{
	return S_OK;
}

HRESULT CUIMoney::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
	return S_OK;
}

HRESULT CUIMoney::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	return S_OK;
}

CUIMoney* CUIMoney::Create()
{
	CUIMoney* pInstance = new CUIMoney();
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIMoney::Free()
{
	__super::Free();
}
