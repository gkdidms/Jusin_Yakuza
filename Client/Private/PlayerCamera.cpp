#include "PlayerCamera.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Collision_Manager.h"
#include "UIManager.h"

CPlayerCamera::CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pCollisionManager{ CCollision_Manager::GetInstance() },
	m_pUIManager{ CUIManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
	Safe_AddRef(m_pUIManager);
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera& rhs)
	: CCamera { rhs },
	m_pSystemManager { rhs.m_pSystemManager },
	m_pCollisionManager{ rhs.m_pCollisionManager },
	m_pUIManager{ rhs.m_pUIManager }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
	Safe_AddRef(m_pUIManager);
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

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CPlayerCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CPlayerCamera::Tick(const _float& fTimeDelta)
{

	if (m_pSystemManager->Get_Camera() != CAMERA_PLAYER) return;

	//Compute_View(fTimeDelta);

	if (m_pGameInstance->GetKeyState(DIK_T) == HOLD)
	{
		if (true == m_pCollisionManager->Map_Collision(m_pColliderCom))
		{
			int a = 0;
		}

	}
	
	
	__super::Tick(fTimeDelta);
}

void CPlayerCamera::Late_Tick(const _float& fTimeDelta)
{
	if (m_pSystemManager->Get_Camera() != CAMERA_PLAYER) return;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	
	Compute_View(fTimeDelta);

	m_bCamCollision = m_pCollisionManager->Map_Collision(m_pColliderCom);


	_vector vPlayerPosition;
	memcpy(&vPlayerPosition, m_pPlayerMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));


	if (false == m_bBlock && true == m_bCamCollision)
	{
		m_vCamCollisionPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_fCamDistance = XMVectorGetX(XMVector3Length(m_vCamCollisionPos - vPlayerPosition));
	}
	else if (false == m_bCamCollision)
	{
		m_bBlock = false;
		if (MAX_DISTANCE > m_fCamDistance)
			m_fCamDistance += 0.01;
		else
			m_fCamDistance = MAX_DISTANCE;
	}

	if (m_fCamDistance > MAX_DISTANCE)
		m_fCamDistance = MAX_DISTANCE;

	if (1 > m_fCamDistance)
	{
		m_fCamDistance = 1;
	}


	__super::Tick(fTimeDelta);

	
#ifdef _DEBUG
	// 테스트용으로 추가함
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_PRIORITY, this);
#endif
}

HRESULT CPlayerCamera::Render()
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	return S_OK;
}

void CPlayerCamera::Compute_View(const _float& fTimeDelta)
{
	_float a = m_pTransformCom->Get_WorldMatrix().r[1].m128_f32[0];
	if (isnan(a))
		int h = 99;
	
	if (m_pUIManager->isInvenClose())
	{
		SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f); // 마우스 좌표 적용해주기
		ShowCursor(false);
	}

	// 플레이어 위치 가져오기
	_vector vPlayerPosition;
	memcpy(&vPlayerPosition, m_pPlayerMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));

	if (2.5f < m_fCamDistance && false == m_bBlock && true == m_bCamCollision)
	{
		m_fCamDistance -= 0.01;
	}
	else if (2.5f >= m_fCamDistance && true == m_bCamCollision)
	{
		m_bBlock = true;
	}

	if (false == m_bBlock)
	{
		// 마우스 입력을 이용한 카메라 회전
		_long MouseMoveX = m_pGameInstance->Get_DIMouseMove(DIMS_X);
		_long MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y);

		m_fCamAngleY -= fTimeDelta * m_fSensor * MouseMoveX;
		m_fCamAngleX += fTimeDelta * m_fSensor * MouseMoveY;

		// 카메라 각도 제한 (수직 각도 제한)
		if (m_fCamAngleX > 80.0f)  // 캐릭터를 아래서 보지 않도록 최대 각도를 45도로 제한
			m_fCamAngleX = 80.0f;
		if (m_fCamAngleX < 20) // 카메라가 수직 아래로 향하지 않도록 최소 각도를 -89도로 제한
			m_fCamAngleX = 20;
	}


	// 이전 카메라 포지션 저장
	_vector vPrevCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// 카메라 포지션 계산
	_vector vCamPosition = XMVectorSet(
		m_fCamDistance * cosf(XMConvertToRadians(m_fCamAngleY)) * cosf(XMConvertToRadians(m_fCamAngleX)),
		m_fCamDistance * sinf(XMConvertToRadians(m_fCamAngleX)),
		m_fCamDistance * sinf(XMConvertToRadians(m_fCamAngleY)) * cosf(XMConvertToRadians(m_fCamAngleX)),
		1.f
	);


	vCamPosition += XMVectorSet(XMVectorGetX(vPlayerPosition), XMVectorGetY(vPlayerPosition), XMVectorGetZ(vPlayerPosition), 0);

	// 이전 카메라 포지션과 새로운 카메라 포지션 사이의 선형보간
	_vector vLerpedCamPosition = XMVectorLerp(vPrevCamPosition, vCamPosition, fTimeDelta * 5.f);


	_vector vLookAt = XMVectorSet(XMVectorGetX(vPlayerPosition), XMVectorGetY(vPlayerPosition) + 1.f, XMVectorGetZ(vPlayerPosition), 1);

	// 카메라가 플레이어를 바라보도록 설정
	m_pTransformCom->LookAt(vLookAt);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpedCamPosition);

	// 월드 매트릭스 업데이트
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CPlayerCamera::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_SPHERE;
	ColliderDesc.fRadius = 0.05f;
	ColliderDesc.vCenter = _float3(0, 0, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

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

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pSystemManager);
	Safe_Release(m_pCollisionManager);
	Safe_Release(m_pUIManager);
}
