#include "../Public/CineCamera.h"

#include "GameInstance.h"

CCineCamera::CCineCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCineCamera::CCineCamera(const CCineCamera& rhs)
	: CCamera{ rhs }
{
}

HRESULT CCineCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCineCamera::Initialize(void* pArg)
{
	PLAYER_CAMERA_DESC* pDesc = static_cast<PLAYER_CAMERA_DESC*>(pArg);

	if (nullptr != pDesc)
	{
		m_fSensor = pDesc->fSensor;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CCineCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CCineCamera::Tick(const _float& fTimeDelta)
{
	/* 마우스 좌표 고정 */
	//SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f); // 마우스 좌표 적용해주기
	//ShowCursor(false);

	if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		m_pTransformCom->Go_Left(fTimeDelta);
	if (GetKeyState('D') & 0x8000)
		m_pTransformCom->Go_Right(fTimeDelta);
	if (GetKeyState('W') & 0x8000)
		m_pTransformCom->Go_Straight(fTimeDelta);
	if (GetKeyState('S') & 0x8000)
		m_pTransformCom->Go_Backward(fTimeDelta);


	if (m_pGameInstance->Get_DIKeyState(DIK_TAB) & 0x80)
	{
		if (false == m_bCameraFix)
		{
			m_bCameraFix = true;
		}
		else
		{
			m_bCameraFix = false;
		}
	}

	if (false == m_bCameraFix)
	{
		Mouse_Fix();
	}


	_long		MouseMove = { 0 };

	if (false == m_bCameraFix)
	{
		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fSensor * MouseMove);
		}

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * m_fSensor * MouseMove);
		}
	}


	__super::Tick(fTimeDelta);
}

void CCineCamera::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CCineCamera::Render()
{
	return S_OK;
}

void CCineCamera::Mouse_Fix()
{
	POINT	pt{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

CCineCamera* CCineCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCineCamera* pInstance = new CCineCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCineCamera::Clone(void* pArg)
{
	CCineCamera* pInstance = new CCineCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCineCamera::Free()
{
	__super::Free();
}
