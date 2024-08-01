#include "CarChaseCamera.h"

#include "GameInstance.h"
#include "SystemManager.h"

CCarChaseCamera::CCarChaseCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera { pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CCarChaseCamera::CCarChaseCamera(const CCarChaseCamera& rhs)
	: CCamera { rhs },
	m_pSystemManager{ rhs.m_pSystemManager}
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

	XMStoreFloat4x4(&m_OrbitMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_LookMatrix, XMMatrixIdentity());

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

	_float4 vCameraPosition;
	vCameraPosition.x = XMVectorGetX(vPlayerPos) + cosf(XMConvertToRadians(m_fCamAngleY)) * cosf(XMConvertToRadians(m_fCamAngleX)) * m_fCamDistance,
	vCameraPosition.y = XMVectorGetY(vPlayerPos) + m_fCamDistance * sinf(XMConvertToRadians(m_fCamAngleX)) + 2.f;
	vCameraPosition.z = XMVectorGetZ(vPlayerPos) + sinf(XMConvertToRadians(m_fCamAngleY)) * cosf(XMConvertToRadians(m_fCamAngleX)),
	vCameraPosition.w = 1.f;

	_vector vLerpedCamPosition = XMVectorLerp(m_vPrevCamPosition, XMLoadFloat4(&vCameraPosition), fTimeDelta * 3);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vCameraPosition));

	Targeting();

	__super::Tick(fTimeDelta);
}

void CCarChaseCamera::Late_Tick(const _float& fTimeDelta)
{
	m_vPrevCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

HRESULT CCarChaseCamera::Render()
{
	return S_OK;
}

void CCarChaseCamera::Targeting()
{
	vector<CGameObject*> Reactors = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Reactor"));

	_vector vTargetingPos;
	if (Reactors.empty())
	{
		vTargetingPos = XMLoadFloat4((_float4*)&m_pPlayerMatrix->m[3]);
		m_pTransformCom->LookAt(vTargetingPos);
		return;
	}
	else {
		vTargetingPos = Reactors[0]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	}

	_vector vCamDirection = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vTargetingPos);

	// 새로운 각도 계산
	m_fCamAngleY = XMConvertToDegrees(atan2f(XMVectorGetZ(vCamDirection), XMVectorGetX(vCamDirection)));
	m_fCamAngleX = XMConvertToDegrees(asinf(XMVectorGetY(vCamDirection) / m_fCamDistance));

	m_pTransformCom->LookAt(vTargetingPos);
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
