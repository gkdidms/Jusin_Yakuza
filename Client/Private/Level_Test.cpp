#include "Level_Test.h"

#include "GameInstance.h"

#include "PlayerCamera.h"

CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Test::Initialize()
{
	if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Test::Tick(const _float& fTimeDelta)
{

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("테스트 레벨"));
#endif
}

HRESULT CLevel_Test::Ready_Light(const wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Test::Ready_Camera(const wstring& strLayerTag)
{
	//CPlayerCamera::PLAYER_CAMERA_DESC		CameraDesc{};

	//CameraDesc.fSensor = 0.1f;
	//CameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	//CameraDesc.vAt = _float4(0.f, 0.0f, 0.0f, 1.f);
	//CameraDesc.fFovy = XMConvertToRadians(60.0f);
	//CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	//CameraDesc.fNear = 0.1f;
	//CameraDesc.fFar = 3000.f;
	//CameraDesc.fSpeedPerSec = 20.f;
	//CameraDesc.fRotationPerSec = XMConvertToRadians(90.f);

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_FreeCamera"), &CameraDesc)))
	//	return E_FAIL;

	//return S_OK;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Player(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_Player"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Map(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_Map"), strLayerTag)))
		return E_FAIL;

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
}
