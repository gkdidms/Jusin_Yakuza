#include "DebugCamera.h"

#include "GameInstance.h"

CDebugCamera::CDebugCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CDebugCamera::CDebugCamera(const CDebugCamera& rhs)
	: CCamera{ rhs }
{
}

HRESULT CDebugCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDebugCamera::Initialize(void* pArg)
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

void CDebugCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CDebugCamera::Tick(const _float& fTimeDelta)
{
	/* ���콺 ��ǥ ���� */
	//SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f); // ���콺 ��ǥ �������ֱ�
	//ShowCursor(false);

	if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		m_pTransformCom->Go_Left(fTimeDelta);
	if (GetKeyState('D') & 0x8000)
		m_pTransformCom->Go_Right(fTimeDelta);
	if (GetKeyState('W') & 0x8000)
		m_pTransformCom->Go_Straight(fTimeDelta);
	if (GetKeyState('S') & 0x8000)
		m_pTransformCom->Go_Backward(fTimeDelta);

	_long		MouseMove = { 0 };

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fSensor * MouseMove);
	}

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * m_fSensor * MouseMove);
	}

	__super::Tick(fTimeDelta);
}

void CDebugCamera::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CDebugCamera::Render()
{
	return S_OK;
}

CDebugCamera* CDebugCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebugCamera* pInstance = new CDebugCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDebugCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDebugCamera::Clone(void* pArg)
{
	CDebugCamera* pInstance = new CDebugCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDebugCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebugCamera::Free()
{
	__super::Free();
}
