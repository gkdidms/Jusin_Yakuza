#include "Level_Edit.h"
#include "GameInstance.h"

#pragma region "객체 원형"
#include "FreeCamera.h"
#pragma endregion 
CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Edit::Initialize()
{
	if (FAILED(Ready_Object(TEXT("Layer_EditObject"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Edit::Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("제작 레벨"));
#endif
}

HRESULT CLevel_Edit::Ready_Object(const wstring& strLayerTag)
{

	LIGHT_DESC			LightDesc = {};	

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	m_pGameInstance->Add_Light(LightDesc);


	CFreeCamera::FREE_CAMERA_DESC CameraDesc{};

	CameraDesc.vEye = _float4(0.0f, 0.f, -1.f, 1.f);
	CameraDesc.vFocus = _float4(0.0f, 0.f, 0.f, 1.f);
	CameraDesc.fFovY = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;	
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	CameraDesc.fSpeedPecSec = 10.f;

	XMStoreFloat4x4(&CameraDesc.vPrePosition, XMMatrixTranslation(0.f, 10.f, 0.f));

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Prototype_GameObject_FreeCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;




	return S_OK;
}

CLevel_Edit* CLevel_Edit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Edit* pInstance = new CLevel_Edit(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Edit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Edit::Free()
{
	__super::Free();
}
