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

	return S_OK;
}

void CCarChaseCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChaseCamera::Tick(const _float& fTimeDelta)
{
	if (m_pSystemManager->Get_Camera() != CAMERA_CARCHASE) return;
	
	//_float MouseMove = { 0.f };
	//if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	//	Turn(&m_OrbitMatrix, XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fSensor);
	//if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Z))
	//	Zoom(fTimeDelta * MouseMove * m_fSensor);

	//카메라가 플레이어를 바라보다가 타겟을 바라봄
	//Targeting();

	_vector vPlayerPos = XMLoadFloat4x4(m_pPlayerMatrix).r[3];
	//vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos));
	//m_pTransformCom->LookAt(vPlayerPos);

	_matrix vParentMatrix = XMMatrixIdentity();
	vParentMatrix.r[3] = vPlayerPos;

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * vParentMatrix);

	__super::Tick(fTimeDelta, false);
}

void CCarChaseCamera::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CCarChaseCamera::Render()
{
	return S_OK;
}

void CCarChaseCamera::Targeting()
{
	vector<CGameObject*> Reactors = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Reactor"));

	if (Reactors.empty())
	{
		m_pTransformCom->LookAt(XMLoadFloat4((_float4*)&m_pPlayerMatrix->m[3]));
		return;
	}
	
	// 몬스터 타겟팅
	_vector vTargetingPos = Reactors[0]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

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
