#include "PlayerCamera.h"

#include "GameInstance.h"

CPlayerCamera::CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera& rhs)
	: CCamera { rhs }
{
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
	/* ���콺 ��ǥ ���� */
	//SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f); // ���콺 ��ǥ �������ֱ�
	//ShowCursor(false);

	_float MouseMove = { 0.f };
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
		//Turn(&m_OrbitMatrix, XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fSensor);
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Z))
		Zoom(fTimeDelta * MouseMove * m_fSensor);

	__super::Tick(fTimeDelta);
}

void CPlayerCamera::Late_Tick(const _float& fTimeDelta)
{
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
}
