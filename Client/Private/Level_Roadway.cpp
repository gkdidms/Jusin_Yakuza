#include "Level_Roadway.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "FileTotalMgr.h"
#include "Collision_Manager.h"
#include "CarChaseManager.h"
#include "UIManager.h"

#include "PlayerCamera.h"
#include "CineCamera.h"
#include "DebugCamera.h"
#include "CarChaseCamera.h"

#include "Level_Loading.h"

CLevel_Roadway::CLevel_Roadway(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext },
    m_pSystemManager{ CSystemManager::GetInstance() },
    m_pFileTotalManager{ CFileTotalMgr::GetInstance() },
	m_pUIManager{CUIManager::GetInstance()}
{
    Safe_AddRef(m_pSystemManager);
    Safe_AddRef(m_pFileTotalManager);
	Safe_AddRef(m_pUIManager);
}

HRESULT CLevel_Roadway::Initialize()
{
	m_pCarChaseManager = CCarChaseManager::Create(m_pDevice, m_pContext);
	if (nullptr == m_pCarChaseManager)
		return E_FAIL;

	m_pUIManager->Open_Scene(TEXT("Carchase"));

    if (FAILED(Ready_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    /* 클라 파싱 */
    m_pFileTotalManager->Set_MapObj_In_Client(STAGE_ROADWAY, LEVEL_ROADWAY);
    m_pFileTotalManager->Set_Lights_In_Client(90);
    //m_pFileTotalManager->Set_Collider_In_Client(STAGE_ROADWAY, LEVEL_ROADWAY);

	if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

    return S_OK;
}

void CLevel_Roadway::Tick(const _float& fTimeDelta)
{
	m_pCarChaseManager->Tick();
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("총격전 맵"));
#endif
}

HRESULT CLevel_Roadway::Ready_Camera(const wstring& strLayerTag)
{
	/* 카메라 추가 시 Debug Camera를 첫번째로 놔두고 추가해주세요 (디버깅 툴에서 사용중)*/
	const _float4x4* pPlayerFloat4x4 = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(LEVEL_ROADWAY, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

	/* 0. 디버그용 카메라 */
	CDebugCamera::DEBUG_CAMERA_DESC		CameraDesc{};
	CameraDesc.fSensor = 0.1f;
	CameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	CameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovY = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPecSec = 10.f;
	CameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	CameraDesc.pPlayerMatrix = pPlayerFloat4x4;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ROADWAY, TEXT("Prototype_GameObject_DebugCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	/* 초기화 할때는 -1 */
	/* 1. 씬용 카메라 */
	CCineCamera::CINE_CAMERA_DESC		cineDesc;
	cineDesc.iFileNum = -1;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ROADWAY, TEXT("Prototype_GameObject_CCineCamera"), strLayerTag, &cineDesc)))
		return E_FAIL;

	/* 2. 플레이어 카메라 */
	CPlayerCamera::PLAYER_CAMERA_DESC		PlayerCameraDesc{};
	PlayerCameraDesc.fSensor = 5.f;
	PlayerCameraDesc.vEye = _float4(0.f, 2.0f, -3.f, 1.f);
	PlayerCameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	PlayerCameraDesc.fFovY = XMConvertToRadians(60.0f);
	PlayerCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	PlayerCameraDesc.fNear = 0.1f;
	PlayerCameraDesc.fFar = 3000.f;
	PlayerCameraDesc.fSpeedPecSec = 20.f;
	PlayerCameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	PlayerCameraDesc.pPlayerMatrix = pPlayerFloat4x4;
	PlayerCameraDesc.iCurLevel = LEVEL_ROADWAY;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ROADWAY, TEXT("Prototype_GameObject_PlayerCamera"), strLayerTag, &PlayerCameraDesc)))
		return E_FAIL;

	/* 3. 컷신용 카메라 */
	CCamera::CAMERA_DESC		CutSceneCameraDesc{};
	CutSceneCameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	CutSceneCameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CutSceneCameraDesc.fFovY = XMConvertToRadians(60.0f);
	CutSceneCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CutSceneCameraDesc.fNear = 0.1f;
	CutSceneCameraDesc.fFar = 3000.f;
	CutSceneCameraDesc.fSpeedPecSec = 10.f;
	CutSceneCameraDesc.fRotatePecSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ROADWAY, TEXT("Prototype_GameObject_CutSceneCamera"), strLayerTag, &CutSceneCameraDesc)))
		return E_FAIL;

	///* 4. 추격전용 카메라 */
	CCarChaseCamera::CARCHASE_CAMERA_DESC		CarChaseCameraDesc{};
	CarChaseCameraDesc.vEye = _float4(3.f, 2.f, 0.f, 1.f);
	CarChaseCameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CarChaseCameraDesc.fFovY = XMConvertToRadians(60.0f);
	CarChaseCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CarChaseCameraDesc.fNear = 0.1f;
	CarChaseCameraDesc.fFar = 3000.f;
	CarChaseCameraDesc.fSpeedPecSec = 10.f;
	CarChaseCameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	CarChaseCameraDesc.fSensor = 0.1f;
	CarChaseCameraDesc.pPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(LEVEL_ROADWAY, TEXT("Layer_Texi"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ROADWAY, TEXT("Prototype_GameObject_CarChaseCamera"), strLayerTag, &CarChaseCameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Roadway::Ready_Player(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc{};
	Desc.fSpeedPecSec = 10.f;
	//Desc.fRotatePecSec = XMConvertToRadians(0.f);
	Desc.fRotatePecSec = XMConvertToRadians(180.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ROADWAY, TEXT("Prototype_GameObject_Player"), strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Roadway* CLevel_Roadway::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Roadway* pInstance = new CLevel_Roadway(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
        Safe_Release(pInstance);

    return pInstance;
}

void CLevel_Roadway::Free()
{
    __super::Free();

    Safe_Release(m_pSystemManager);
    Safe_Release(m_pFileTotalManager);
	Safe_Release(m_pCarChaseManager);
	Safe_Release(m_pUIManager);
}
