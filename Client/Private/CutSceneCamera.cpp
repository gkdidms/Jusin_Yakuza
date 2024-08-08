#include "CutSceneCamera.h"

#include "GameInstance.h"
#include "FileTotalMgr.h"
#include "SystemManager.h"

#include "Player.h"

CCutSceneCamera::CCutSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext },
	m_pSystemManager { CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CCutSceneCamera::CCutSceneCamera(const CCutSceneCamera& rhs)
	: CCamera{ rhs },
	m_pSystemManager { rhs.m_pSystemManager }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CCutSceneCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCutSceneCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCutSceneCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CCutSceneCamera::Tick(const _float& fTimeDelta)
{
	if (m_pSystemManager->Get_Camera() != CAMERA_CUTSCENE) return;

	ShowCursor(false);

	// �÷��̾�� ���� Ÿ�̸Ӹ� ����Ѵ�
	__super::Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));
}

void CCutSceneCamera::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CCutSceneCamera::Render()
{
	return S_OK;
}

void CCutSceneCamera::Return_PrevWorld(const _float& fTimeDelta)
{
	if (!m_isReturn) return;

	// ��� �ð� ������Ʈ
	m_fElapsedTime += fTimeDelta;
	m_fLerpRatio = m_fElapsedTime / m_fTotalLerpTime;

	// ���� ������ 1�� �ʰ����� �ʵ��� ����
	if (m_fLerpRatio >= 1.0f)
	{
		m_fLerpRatio = 1.0f;
		m_isReturn = false; // ���� �Ϸ�
	}

	// ���� ��İ� ���� ����� �����ؼ� ����
	_matrix PrevMat = XMLoadFloat4x4(&m_PrevMatrix);
	_vector vPrevScale, vPrevRot, vPrevPos;
	XMMatrixDecompose(&vPrevScale, &vPrevRot, &vPrevPos, PrevMat);

	_matrix StartMat = XMLoadFloat4x4(&m_StartMatrix);
	_vector vStartScale, vStartRot, vStartPos;
	XMMatrixDecompose(&vStartScale, &vStartRot, &vStartPos, StartMat);

	// �� ���п� ���� ���� ����
	_vector vScaleLerp = XMVectorLerp(vPrevScale, vStartScale, m_fLerpRatio);
	_vector vTransLerp = XMVectorLerp(vPrevPos, vStartPos, m_fLerpRatio);
	_vector vRotLerp = XMQuaternionSlerp(vPrevRot, vStartRot, m_fLerpRatio); // ȸ���� ���� ���� ����(Slerp)�� ���

	// ������ ���� ������ ��� ����
	_matrix M = XMMatrixScalingFromVector(vScaleLerp) *
		XMMatrixRotationQuaternion(vRotLerp) *
		XMMatrixTranslationFromVector(vTransLerp);

	XMStoreFloat4x4(&m_WorldMatrix, M);
	m_fFovY = LerpFloat(m_fStartFov, m_fDefaultFovY, m_fLerpRatio);

	// ���⼭ false�� ���´ٴ°���, �Ϸ� ���Ķ�� ������ �ʱ�ȭ������Ѵ�.
	if (!m_isReturn)
	{
		Reset_ReturnVariables();
	}
}

void CCutSceneCamera::Reset_ReturnVariables()
{
	m_isReturn = { false };
	m_fLerpRatio = { 0.f };
	m_fElapsedTime = 0.0f; 
	m_fTotalLerpTime = 0.5f; 
	m_fStartFov = 0.0f; 
	m_fFovY = m_fDefaultFovY;
}

void CCutSceneCamera::Play_FovLerp(const _float& fTimeDelta)
{
	// ��� �ð� ������Ʈ
	m_fElapsedTime += fTimeDelta;
	m_fLerpRatio = m_fElapsedTime / m_fTotalLerpTime;

	// ���� ������ 1�� �ʰ����� �ʵ��� ����
	if (m_fLerpRatio >= 1.0f)
	{
		m_fLerpRatio = 1.0f;
		m_isReturn = false; // ���� �Ϸ�
	}

	m_fFovY = LerpFloat(m_fStartFov, m_fDefaultFovY, m_fLerpRatio);
	// ���⼭ false�� ���´ٴ°���, �Ϸ� ���Ķ�� ������ �ʱ�ȭ������Ѵ�.
	if (!m_isReturn)
	{
		Reset_ReturnVariables();
	}
}

CCutSceneCamera* CCutSceneCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCutSceneCamera* pInstance = new CCutSceneCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCutSceneCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCutSceneCamera::Clone(void* pArg)
{
	CCutSceneCamera* pInstance = new CCutSceneCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCutSceneCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCutSceneCamera::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
}
