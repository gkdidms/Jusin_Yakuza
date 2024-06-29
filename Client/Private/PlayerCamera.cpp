#include "PlayerCamera.h"

#include "GameInstance.h"
#include "SystemManager.h"

CPlayerCamera::CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera& rhs)
	: CCamera { rhs },
	m_pSystemManager { rhs.m_pSystemManager }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CPlayerCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerCamera::Initialize(void* pArg)
{
	PLAYER_CAMERA_DESC* pDesc = static_cast<PLAYER_CAMERA_DESC*>(pArg);

	if (nullptr != pDesc)
	{
		m_fSensor = pDesc->fSensor;
		m_pPlayerMatrix = pDesc->pPlayerMatrix;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CPlayerCamera::Tick(const _float& fTimeDelta)
{
	if (m_pSystemManager->Get_Camera() != CAMERA_PLAYER) return;
	/* 마우스 좌표 고정 */
	//SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f); // 마우스 좌표 적용해주기
	//ShowCursor(false);

	//if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	//	m_pTransformCom->Go_Left(fTimeDelta);
	//if (GetKeyState('D') & 0x8000)
	//	m_pTransformCom->Go_Right(fTimeDelta);
	//if (GetKeyState('W') & 0x8000)
	//	m_pTransformCom->Go_Straight(fTimeDelta);
	//if (GetKeyState('S') & 0x8000)
	//	m_pTransformCom->Go_Backward(fTimeDelta);

	_long		MouseMove = { 0 };

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fSensor * MouseMove);
	}

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * m_fSensor * MouseMove);
	}

	__super::Tick(fTimeDelta, false);
}

void CPlayerCamera::Late_Tick(const _float& fTimeDelta)
{
	//플레이어 카메라는 플레이어를 따라다닌다.
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pPlayerMatrix));
}

HRESULT CPlayerCamera::Render()
{
	return S_OK;
}

CPlayerCamera* CPlayerCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerCamera* pInstance = new CPlayerCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerCamera::Clone(void* pArg)
{
	CPlayerCamera* pInstance = new CPlayerCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerCamera::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
}
