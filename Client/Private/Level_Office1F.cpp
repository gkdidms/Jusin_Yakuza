#include "Level_Office1F.h"

#include "SystemManager.h"
#include "FileTotalMgr.h"

CLevel_Office1F::CLevel_Office1F(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pFileTotalManager{ CFileTotalMgr::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pFileTotalManager);
}

HRESULT CLevel_Office1F::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	/* 클라 파싱 */
	m_pFileTotalManager->Set_MapObj_In_Client(0, LEVEL_OFFICE_1F);
	m_pFileTotalManager->Set_Lights_In_Client(0);
	m_pFileTotalManager->Set_Collider_In_Client(0, LEVEL_TEST);

	return S_OK;
}

void CLevel_Office1F::Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("사무실 1F"));
#endif
}

HRESULT CLevel_Office1F::Ready_Light()
{

	return S_OK;
}

CLevel* CLevel_Office1F::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Office1F* pInstance = new CLevel_Office1F{ pDevice, pContext };

	if (FAILED(pInstance->Initialize()))
		Safe_Release(pInstance);

	return pInstance;
}

void CLevel_Office1F::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
	Safe_Release(m_pFileTotalManager);
}
