#include "UIMoney.h"

CUIMoney::CUIMoney()
	:CUIScene{}
{
}

CUIMoney::CUIMoney(const CUIMoney& rhs)
	:CUIScene{rhs}
{
}

HRESULT CUIMoney::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
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

CUIMoney* CUIMoney::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUIMoney* pInstance = new CUIMoney();
	if (FAILED(pInstance->Initialize(pDevice , pContext, pArg)))
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
