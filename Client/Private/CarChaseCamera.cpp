#include "CarChaseCamera.h"

#include "GameInstance.h"
#include "SystemManager.h"

CCarChaseCamera::CCarChaseCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CCarChaseCamera::CCarChaseCamera(const CCarChaseCamera& rhs)
	: CCamera{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CCarChaseCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChaseCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CARCHASE_CAMERA_DESC* pDesc = static_cast<CARCHASE_CAMERA_DESC*>(pArg);
	m_fSensor = pDesc->fSensor;
	m_pPlayerMatrix = pDesc->pPlayerMatrix;
	m_pPlayerBoneMatrix = pDesc->pPlayerBoneMatrix;

	return S_OK;
}

void CCarChaseCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChaseCamera::Tick(const _float& fTimeDelta)
{
	if (m_pSystemManager->Get_Camera() != CAMERA_CARCHASE) return;

	_vector vPlayerPos = XMLoadFloat4x4(m_pPlayerMatrix).r[3];

	//Adjust_Camera_Angle();

	Targeting(fTimeDelta);

	if (m_pGameInstance->GetKeyState(DIK_RSHIFT) == TAP)
	{
		m_isShoulderView = !m_isShoulderView;
	}

	_float4 vCameraPosition;

	if (!m_isShoulderView)
	{
		vCameraPosition.x = XMVectorGetX(vPlayerPos) + cosf(XMConvertToRadians(m_fCamAngleX)) * cosf(XMConvertToRadians(m_fCamAngleY)) * m_fCamDistance,
		vCameraPosition.y = XMVectorGetY(vPlayerPos) + m_fCamDistance * sinf(XMConvertToRadians(m_fCamAngleY)) + m_fHeight;
		vCameraPosition.z = XMVectorGetZ(vPlayerPos) + sinf(XMConvertToRadians(m_fCamAngleX)) * cosf(XMConvertToRadians(m_fCamAngleY)),
		vCameraPosition.w = 1.f;
	}
	else
	{
		vCameraPosition.x = XMVectorGetX(vPlayerPos) + cosf(XMConvertToRadians(m_fCamAngleX)) * cosf(XMConvertToRadians(m_fCamAngleY)) * m_fCamDistance_X,
		vCameraPosition.y = XMVectorGetY(vPlayerPos) + m_fCamDistance_Y * sinf(XMConvertToRadians(m_fCamAngleY)) + m_fHeight;
		vCameraPosition.z = XMVectorGetZ(vPlayerPos) + sinf(XMConvertToRadians(m_fCamAngleX)) * cosf(XMConvertToRadians(m_fCamAngleY)) * m_fCamDistance_Z,
		vCameraPosition.w = 1.f;
	}


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vCameraPosition));

	
	__super::Tick(fTimeDelta);
}

void CCarChaseCamera::Late_Tick(const _float& fTimeDelta)
{
	
}

HRESULT CCarChaseCamera::Render()
{
	return S_OK;
}

void CCarChaseCamera::Targeting(const _float& fTimeDelta)
{
	vector<CGameObject*> Reactors = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Reactor"));
	if (Reactors.size() >= 2)
		m_isTargetPlayer = false;

	_vector vTargetingPos;
	if (Reactors.empty() || m_isTargetPlayer)
	{
		m_fHeight = 4.f;
		XMStoreFloat4x4(&m_pPlayerWorldMatrix, XMLoadFloat4x4(m_pPlayerBoneMatrix) * XMLoadFloat4x4(m_pPlayerMatrix));
		vTargetingPos = XMLoadFloat4((_float4*)&m_pPlayerMatrix->m[3]);

		//선형보간
		_vector vCamDirection = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		_vector vCamTargetDirection = XMVector3Normalize(vTargetingPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		_vector vLerpDir = XMVectorLerp(vCamDirection, vCamTargetDirection, fTimeDelta * 2.f);
		m_pTransformCom->LookAt(vLerpDir, true);

		//카메라 이동값 용
		_vector vCamMoveDir = XMLoadFloat4((_float4*)&m_pPlayerWorldMatrix.m[0]) * -1.f;
		m_fCamAngleX = XMConvertToDegrees(atan2f(XMVectorGetZ(vCamMoveDir), XMVectorGetX(vCamMoveDir)));
		m_fCamAngleY = XMConvertToDegrees(asinf(XMVectorGetY(vCamMoveDir) / m_fCamDistance));
	}
	else {
		CGameObject* pTarget = Reactors.back();

		if (pTarget->isObjectDead())
		{
			if (Reactors.size() <= 1)
			{
				if (pTarget->isDead())
					m_isTargetPlayer = true;
				return;
			}

			pTarget = Reactors[Reactors.size() - 2];
		}

		m_fHeight = 2.f;
		vTargetingPos = pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		//선형보간
		_vector vCamDirection = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		_vector vCamTargetDirection = XMVector3Normalize(vTargetingPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		_vector vLerpDir = XMVectorLerp(vCamDirection, vCamTargetDirection, fTimeDelta * 2.f);
		m_pTransformCom->LookAt(vLerpDir, true);

		//카메라 이동값 용
		_vector vCamMoveDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vTargetingPos);
		m_fCamAngleX = XMConvertToDegrees(atan2f(XMVectorGetZ(vCamMoveDir), XMVectorGetX(vCamMoveDir)));
		m_fCamAngleY = XMConvertToDegrees(asinf(XMVectorGetY(vCamMoveDir) / m_fCamDistance));
	}
}

CCarChaseCamera* CCarChaseCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarChaseCamera* pInstance = new CCarChaseCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CCarChaseCamera::Clone(void* pArg)
{
	CCarChaseCamera* pInstance = new CCarChaseCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCarChaseCamera::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
}