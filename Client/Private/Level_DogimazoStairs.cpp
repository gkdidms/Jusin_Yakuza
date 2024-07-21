#include "Level_DogimazoStairs.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "FileTotalMgr.h"

#include "PlayerCamera.h"
#include "CineCamera.h"
#include "DebugCamera.h"

#include "Level_Loading.h"
#include "Trigger.h"

CLevel_DogimazoStairs::CLevel_DogimazoStairs(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pFileTotalManager{ CFileTotalMgr::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pFileTotalManager);
}

HRESULT CLevel_DogimazoStairs::Initialize()
{
	if (FAILED(Ready_Player(TEXT("Layer_Player"))))
		return E_FAIL;



	/* 클라 파싱 */
	m_pFileTotalManager->Set_MapObj_In_Client(STAGE_DOGIMAZO_STAIRS, LEVEL_DOGIMAZO_STAIRS);
	m_pFileTotalManager->Set_Lights_In_Client(STAGE_DOGIMAZO_STAIRS);
	m_pFileTotalManager->Set_Collider_In_Client(STAGE_DOGIMAZO_STAIRS, LEVEL_DOGIMAZO_STAIRS);
	m_pFileTotalManager->Set_Trigger_In_Client(STAGE_DOGIMAZO_STAIRS, LEVEL_DOGIMAZO_STAIRS);

	if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_DogimazoStairs::Tick(const _float& fTimeDelta)
{


	// 트리거 체크 - 씬 이동
	vector<CGameObject*> pTriggers = m_pGameInstance->Get_GameObjects(LEVEL_DOGIMAZO_STAIRS, TEXT("Layer_Trigger"));

	for (int i = 0; i < pTriggers.size(); i++)
	{
		int		iLevelNum;
		if (true == dynamic_cast<CTrigger*>(pTriggers[i])->Move_Scene(iLevelNum))
		{
			m_bSceneChange = true;
			m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL)iLevelNum));
		}
	}

	if (false == m_bSceneChange)
	{
		if (m_pGameInstance->GetKeyState(DIK_RETURN) == TAP)
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO_LOBBY))))
				return;
		}
	}

	

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("도지마조 계단"));
#endif
}

HRESULT CLevel_DogimazoStairs::Ready_Camera(const wstring& strLayerTag)
{
	/* 카메라 추가 시 Debug Camera를 첫번째로 놔두고 추가해주세요 (디버깅 툴에서 사용중)*/
	const _float4x4* pPlayerFloat4x4 = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(LEVEL_DOGIMAZO_STAIRS, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

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

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO_STAIRS, TEXT("Prototype_GameObject_DebugCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	/* 초기화 할때는 -1 */
	/* 1. 씬용 카메라 */
	CCineCamera::CINE_CAMERA_DESC		cineDesc;
	cineDesc.iFileNum = -1;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO_STAIRS, TEXT("Prototype_GameObject_CCineCamera"), strLayerTag, &cineDesc)))
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
	PlayerCameraDesc.iCurLevel = LEVEL_DOGIMAZO_STAIRS;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO_STAIRS, TEXT("Prototype_GameObject_PlayerCamera"), strLayerTag, &PlayerCameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_DogimazoStairs::Ready_Player(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc{};
	Desc.fSpeedPecSec = 10.f;
	//Desc.fRotatePecSec = XMConvertToRadians(0.f);
	Desc.fRotatePecSec = XMConvertToRadians(180.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO_STAIRS, TEXT("Prototype_GameObject_Player"), strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

CLevel_DogimazoStairs* CLevel_DogimazoStairs::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_DogimazoStairs* pInstance = new CLevel_DogimazoStairs(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
		Safe_Release(pInstance);

	return pInstance;
}

void CLevel_DogimazoStairs::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
	Safe_Release(m_pFileTotalManager);
}
