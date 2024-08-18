#include "Level_Tutorial.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "FileTotalMgr.h"
#include "Collision_Manager.h"
#include "TutorialManager.h"
#include "FightManager.h"
#include "QuestManager.h"
#include "ScriptManager.h"

#include "PlayerCamera.h"
#include "CineCamera.h"
#include "DebugCamera.h"

#include "Level_Loading.h"

CLevel_Tutorial::CLevel_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pFileTotalManager{ CFileTotalMgr::GetInstance() },
	m_pFightManager{ CFightManager::GetInstance()},
	m_pQuestManager{ CQuestManager::GetInstance()}
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pFileTotalManager);
	Safe_AddRef(m_pFightManager);
	Safe_AddRef(m_pQuestManager);
}

HRESULT CLevel_Tutorial::Initialize()
{
	if (FAILED(Ready_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(m_pQuestManager->Initialize()))
		return E_FAIL;

	m_pQuestManager->Start_Quest(CQuestManager::CHAPTER_1);
	m_pFightManager->Set_StreetFight(true);

	m_pTutorialManager = CTutorialManager::Create();
	if (nullptr == m_pTutorialManager)
		return E_FAIL;

	/* Ŭ�� �Ľ� */
	m_pFileTotalManager->Set_MapObj_In_Client(STAGE_TUTORIAL, LEVEL_TUTORIAL);
	m_pFileTotalManager->Set_Lights_In_Client(99);
	//m_pFileTotalManager->Set_Collider_In_Client(STAGE_TUTORIAL, LEVEL_TUTORIAL);

	if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	m_pSystemManager->Set_Camera(CAMERA_PLAYER);

	return S_OK;
}

void CLevel_Tutorial::Tick(const _float& fTimeDelta)
{
	if (m_pQuestManager->Execute())
	{
		//true �̸� ���� ���������� �̵�
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_NISHIKIWALK));
	}

	m_pFightManager->Tick(fTimeDelta);

	m_pTutorialManager->Tick();

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("�Ѱ��� ��"));
#endif
}

HRESULT CLevel_Tutorial::Ready_Camera(const wstring& strLayerTag)
{
	/* ī�޶� �߰� �� Debug Camera�� ù��°�� ���ΰ� �߰����ּ��� (����� ������ �����)*/
	const _float4x4* pPlayerFloat4x4 = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(LEVEL_TUTORIAL, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

	/* 0. ����׿� ī�޶� */
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

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TUTORIAL, TEXT("Prototype_GameObject_DebugCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	/* �ʱ�ȭ �Ҷ��� -1 */
	/* 1. ���� ī�޶� */
	CCineCamera::CINE_CAMERA_DESC		cineDesc;
	cineDesc.iFileNum = -1;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TUTORIAL, TEXT("Prototype_GameObject_CCineCamera"), strLayerTag, &cineDesc)))
		return E_FAIL;

	/* 2. �÷��̾� ī�޶� */
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
	PlayerCameraDesc.iCurLevel = LEVEL_TUTORIAL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TUTORIAL, TEXT("Prototype_GameObject_PlayerCamera"), strLayerTag, &PlayerCameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Player(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc{};
	Desc.fSpeedPecSec = 10.f;
	//Desc.fRotatePecSec = XMConvertToRadians(0.f);
	Desc.fRotatePecSec = XMConvertToRadians(180.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TUTORIAL, TEXT("Prototype_GameObject_Player"), strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Tutorial* CLevel_Tutorial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tutorial* pInstance = new CLevel_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
		Safe_Release(pInstance);

	return pInstance;
}

void CLevel_Tutorial::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
	Safe_Release(m_pFileTotalManager);
	Safe_Release(m_pTutorialManager);
	Safe_Release(m_pFightManager);
	Safe_Release(m_pQuestManager);
}
