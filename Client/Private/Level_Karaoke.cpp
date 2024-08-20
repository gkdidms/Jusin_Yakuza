#include "Level_Karaoke.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "FileTotalMgr.h"
#include "Collision_Manager.h"
#include "QuestManager.h"

#include "PlayerCamera.h"
#include "CineCamera.h"
#include "DebugCamera.h"

#include "Level_Loading.h"

#include "KaraokeManager.h"

CLevel_Karaoke::CLevel_Karaoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext },
    m_pSystemManager{ CSystemManager::GetInstance() },
    m_pFileTotalManager{ CFileTotalMgr::GetInstance() },
	m_pQuestManager{ CQuestManager::GetInstance() }
{
    Safe_AddRef(m_pSystemManager);
    Safe_AddRef(m_pFileTotalManager);
	Safe_AddRef(m_pQuestManager);
}

HRESULT CLevel_Karaoke::Initialize()
{
    if (FAILED(Ready_Player(TEXT("Layer_Player"))))
        return E_FAIL;

	//m_pQuestManager->Start_Quest(CQuestManager::CHAPTER_3);

    ///* 클라 파싱 */
    m_pFileTotalManager->Set_MapObj_In_Client(STAGE_KARAOKE, LEVEL_KARAOKE);
    m_pFileTotalManager->Set_Lights_In_Client(99);
	//m_pFileTotalManager->Set_Trigger_In_Client(STAGE_KARAOKE, LEVEL_KARAOKE);
    //m_pFileTotalManager->Set_Collider_In_Client(STAGE_KARAOKE, LEVEL_KARAOKE);

	m_pKaraokeManager = CKaraokeManager::Create();
	if (nullptr == m_pKaraokeManager)
		return E_FAIL;


	if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	m_pFileTotalManager->Load_Cinemachine(5, LEVEL_KARAOKE);
	m_pFileTotalManager->Setting_Start_Cinemachine(5);

	m_pSystemManager->Set_Camera(CAMERA_CINEMACHINE);
	CCineCamera* pCutSceneCamera = dynamic_cast<CCineCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CINEMACHINE));
	pCutSceneCamera->Set_Loop(true);

    return S_OK;
}

void CLevel_Karaoke::Tick(const _float& fTimeDelta)
{
	m_pKaraokeManager->Tick(fTimeDelta);

	//if (m_pQuestManager->Execute())
	//{
	//	m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TOKOSTREET));
	//}
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("가라오케 맵"));
#endif
}

HRESULT CLevel_Karaoke::Ready_Camera(const wstring& strLayerTag)
{
	/* 카메라 추가 시 Debug Camera를 첫번째로 놔두고 추가해주세요 (디버깅 툴에서 사용중)*/
	const _float4x4* pPlayerFloat4x4 = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(LEVEL_KARAOKE, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

	/* 0. 디버그용 카메라 */
	CDebugCamera::DEBUG_CAMERA_DESC		CameraDesc{};
	CameraDesc.fSensor = 0.1f;
	CameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	CameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovY = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 200.f;
	CameraDesc.fSpeedPecSec = 10.f;
	CameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	CameraDesc.pPlayerMatrix = pPlayerFloat4x4;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_KARAOKE, TEXT("Prototype_GameObject_DebugCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	/* 초기화 할때는 -1 */
	/* 1. 씬용 카메라 */
	CCineCamera::CINE_CAMERA_DESC		cineDesc;
	cineDesc.iFileNum = -1;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_KARAOKE, TEXT("Prototype_GameObject_CCineCamera"), strLayerTag, &cineDesc)))
		return E_FAIL;

	/* 2. 플레이어 카메라 */
	CPlayerCamera::PLAYER_CAMERA_DESC		PlayerCameraDesc{};
	PlayerCameraDesc.fSensor = 5.f;
	PlayerCameraDesc.vEye = _float4(0.f, 2.0f, -3.f, 1.f);
	PlayerCameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	PlayerCameraDesc.fFovY = XMConvertToRadians(60.0f);
	PlayerCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	PlayerCameraDesc.fNear = 0.1f;
	PlayerCameraDesc.fFar = 200.f;
	PlayerCameraDesc.fSpeedPecSec = 20.f;
	PlayerCameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	PlayerCameraDesc.pPlayerMatrix = pPlayerFloat4x4;
	PlayerCameraDesc.iCurLevel = LEVEL_KARAOKE;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_KARAOKE, TEXT("Prototype_GameObject_PlayerCamera"), strLayerTag, &PlayerCameraDesc)))
		return E_FAIL;

	/* 3. 컷신용 카메라 */
	CCamera::CAMERA_DESC		CutSceneCameraDesc{};
	CutSceneCameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	CutSceneCameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CutSceneCameraDesc.fFovY = XMConvertToRadians(60.0f);
	CutSceneCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CutSceneCameraDesc.fNear = 0.1f;
	CutSceneCameraDesc.fFar = 300.f;
	CutSceneCameraDesc.fSpeedPecSec = 10.f;
	CutSceneCameraDesc.fRotatePecSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_KARAOKE, TEXT("Prototype_GameObject_CutSceneCamera"), strLayerTag, &CutSceneCameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Karaoke::Ready_Player(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc{};
	Desc.fSpeedPecSec = 10.f;
	Desc.fRotatePecSec = XMConvertToRadians(180.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_KARAOKE, TEXT("Prototype_GameObject_Kiryu_Karaoke"), strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Karaoke* CLevel_Karaoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Karaoke* pInstance = new CLevel_Karaoke(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
        Safe_Release(pInstance);

    return pInstance;
}

void CLevel_Karaoke::Free()
{
    __super::Free();

    Safe_Release(m_pSystemManager);
    Safe_Release(m_pFileTotalManager);
    Safe_Release(m_pKaraokeManager);
	Safe_Release(m_pQuestManager);
}
