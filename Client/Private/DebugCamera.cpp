#include "DebugCamera.h"

CDebugCamera::CDebugCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CDebugCamera::CDebugCamera(const CDebugCamera& rhs)
	: CCamera{ rhs }
{
}

HRESULT CDebugCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDebugCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CDebugCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CDebugCamera::Tick(const _float& fTimeDelta)
{
}

void CDebugCamera::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CDebugCamera::Render()
{
	return S_OK;
}

CDebugCamera* CDebugCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebugCamera* pInstance = new CDebugCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDebugCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDebugCamera::Clone(void* pArg)
{
	CDebugCamera* pInstance = new CDebugCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDebugCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebugCamera::Free()
{
	__super::Free();
}
