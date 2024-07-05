#include "Level_Office2F.h"

#include "SystemManager.h"
#include "FileTotalMgr.h"

CLevel_Office2F::CLevel_Office2F(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext},
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pFileTotalManager{ CFileTotalMgr::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pFileTotalManager);
}

HRESULT CLevel_Office2F::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	return S_OK	;
}

void CLevel_Office2F::Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("»ç¹«½Ç 2F"));
#endif
}

HRESULT CLevel_Office2F::Ready_Light()
{
	return S_OK;
}

CLevel_Office2F* CLevel_Office2F::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Office2F* pInstance = new CLevel_Office2F(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
		Safe_Release(pInstance);

	return pInstance;
}

void CLevel_Office2F::Free()
{
	__super::Free();

	Safe_Release(m_pFileTotalManager);
	Safe_Release(m_pSystemManager);
}
