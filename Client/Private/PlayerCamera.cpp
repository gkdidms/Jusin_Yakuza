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

	Compute_View(fTimeDelta);

}

void CPlayerCamera::Late_Tick(const _float& fTimeDelta)
{
	if (m_pSystemManager->Get_Camera() != CAMERA_PLAYER) return;
	Compute_View(fTimeDelta);

	__super::Tick(fTimeDelta);
}

HRESULT CPlayerCamera::Render()
{
	return S_OK;
}

void CPlayerCamera::Compute_View(const _float& fTimeDelta)
{
	SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f); // 마우스 좌표 적용해주기
	ShowCursor(false);

	_vector vPlayerPosition;
	memcpy(&vPlayerPosition, m_pPlayerMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));

	//마우스 입력을 이용한 카메라 회전
	_long MouseMove = { 0 };
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		fCamAngleY -= fTimeDelta * m_fSensor * MouseMove;
	}
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
		fCamAngleX += fTimeDelta * m_fSensor * MouseMove;
	}

	// 카메라 포지션 계산
	_vector vCamPosition = XMVectorSet(
		m_fCamDistance * cosf(XMConvertToRadians(fCamAngleY)) * cosf(XMConvertToRadians(fCamAngleX)),
		m_fCamDistance * sinf(XMConvertToRadians(fCamAngleX)),
		m_fCamDistance * sinf(XMConvertToRadians(fCamAngleY)) * cosf(XMConvertToRadians(fCamAngleX)),
		1.f
	);
	vCamPosition += XMVectorSet(XMVectorGetX(vPlayerPosition), XMVectorGetY(vPlayerPosition), XMVectorGetZ(vPlayerPosition), 0);
	m_pTransformCom->LookAt(XMVectorSet(XMVectorGetX(vPlayerPosition), XMVectorGetY(vPlayerPosition) + 1.f, XMVectorGetZ(vPlayerPosition), 1));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
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
