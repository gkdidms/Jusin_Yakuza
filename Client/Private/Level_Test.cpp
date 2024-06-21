#include "Level_Test.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "DebugManager.h"

#include "DebugCamera.h"

#include "Client_MapDataMgr.h"


CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pClientMapDataMgr{ CClient_MapDataMgr::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pClientMapDataMgr);
}

HRESULT CLevel_Test::Initialize()
{
	m_pSystemManager->Set_Level(LEVEL_TEST);

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Player(TEXT("Layer_Player"))))
		return E_FAIL;
	
	//if (FAILED(Ready_Map(TEXT("Layer_Map"))))
	//	return E_FAIL;

	/* 클라 파싱 */
	m_pClientMapDataMgr->Set_MapObj_In_Client(0, LEVEL_TEST);

	return S_OK;
}

void CLevel_Test::Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("테스트 레벨"));
#endif
}

HRESULT CLevel_Test::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	m_pGameInstance->Add_Light(LightDesc);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Camera(const wstring& strLayerTag)
{
	CDebugCamera::PLAYER_CAMERA_DESC		CameraDesc{};

	CameraDesc.fSensor = 0.1f;
	CameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	CameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovY = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPecSec = 20.f;
	CameraDesc.fRotatePecSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_DebugCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Player(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc{};
	Desc.fSpeedPecSec = 5.f;
	Desc.fRotatePecSec = XMConvertToRadians(180.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_Player"), strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Map(const wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_Map"), strLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

CLevel_Test* CLevel_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Test* pInstance = new CLevel_Test(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Test");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Test::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
	Safe_Release(m_pClientMapDataMgr);
}
