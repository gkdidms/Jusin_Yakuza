#include "SocketCollider.h"
#include "GameInstance.h"

#include "Effect.h"

#include "LandObject.h"

CSocketCollider::CSocketCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CSocketObject{ pDevice, pContext }
{
}

CSocketCollider::CSocketCollider(const CSocketCollider & rhs)
	: CSocketObject{ rhs }
{
}

HRESULT CSocketCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSocketCollider::Initialize(void * pArg)
{
	SOCKET_COLLIDER_DESC* pDesc = static_cast<SOCKET_COLLIDER_DESC*>(pArg);
	m_pParentObject = pDesc->pParentObject;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	// ATTACK, HIT
	if (pDesc->ColliderState.iActionType == 0)
	{
		// HAND_A, FOOT_A, JOINT_A, HEAD_A
		switch (pDesc->ColliderState.iPartType)
		{
		case 0:
		{
			m_fDamage = 10.f;

			break;
		}
		case 1:
		{
			m_fDamage = 30.f;

			break;
		}
		case 2:
		{
			m_fDamage = 20.f;

			break;
		}
		case 3:
		{
			m_fDamage = 20.f;
			break;
		}
		}
	}
	else if (pDesc->ColliderState.iActionType == 1)
	{
		// 히트용 콜라이더라면 부위별로 딜 받는 수치를 m_fDamage에 저장해두자 (사용할지는 모르겟음)
		// 사용할거라면 머리를 히트당했다면 기존 데미지보다 2배 딜들어오게 몸은 1배 다리는 0.7배 이런식으로 사용예정
		// HEAD_H = 10, BODY_H, LEG_H
		switch (pDesc->ColliderState.iPartType)
		{
		case 10:
		{
			m_fDamage = 2.f;
			break;
		}
		case 11:
		{
			m_fDamage = 1.f;
			break;
		}
		case 12:
		{
			m_fDamage = 0.8f;
			break;
		}
		}
	}



	return S_OK;
}

void CSocketCollider::Priority_Tick(const _float& fTimeDelta)
{
}

void CSocketCollider::Tick(const _float& fTimeDelta)
{
	Filtering_Timer(fTimeDelta);

	m_vPrevMovePos = m_vMovePos;

	if(m_pColliderCom)
		m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

	memcpy(&m_vMovePos, m_WorldMatrix.m[CTransform::STATE_POSITION], sizeof(_float3));

	_vector vDirection = XMVector3Normalize(XMLoadFloat3(&m_vPrevMovePos) - XMLoadFloat3(&m_vMovePos));
	XMStoreFloat3(&m_vMoveDir, vDirection);
}

void CSocketCollider::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_float4 vParentPosition;
	memcpy(&vParentPosition, m_pParentMatrix->m[3], sizeof(_float4));

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CSocketCollider::Render()
{
#ifdef _DEBUG
	if(m_pColliderCom && m_isOn)
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	return S_OK;
}

_bool CSocketCollider::Intersect(CCollider* pTargetObject, _float fDistance)
{
	if (m_isFiltered) return false;

	m_isFiltered = true;
	return pTargetObject->Intersect(m_pColliderCom, fDistance);
}

void CSocketCollider::Filtering_Timer(_float fTimeDelta)
{
	if (!m_isFiltered) return; 

	m_fFilteringTimer += fTimeDelta;

	if (FILTERING_TIME <= m_fFilteringTimer)
	{
		m_fFilteringTimer = 0.f;
		m_isFiltered = false;
	}
}

void CSocketCollider::ParentObject_Hit(const _float3& vDir, _float fDamage, CLandObject* pParentObject, _bool isBlowAttack)
{
	//_matrix ParentMatrix = pParentObject->Get_TransformCom()->Get_WorldMatrix();
	//// m_eColliderPartType는 본인이 헤드인지, 바디인지, 레그인지를 가지고있다
	//// TODO: 피격 파티클로 교체가 필요하다.
	//CEffect::EFFECT_DESC EffectDesc;


	//_matrix WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	//// 테스트용으로 Dir을 고정시켜봤으나 파티클 터지는 방향 그대로임
	//_vector vRight = XMVectorSet(1, 0, 0, 0);
	//_vector vUp = XMVectorSet(0, 1, 0, 0);
	//_vector vLook = XMVectorSet(0, 0, 1, 0);

	//WorldMatrix.r[CTransform::STATE_RIGHT] = XMVector4Normalize(vRight);
	//WorldMatrix.r[CTransform::STATE_UP] = XMVector4Normalize(vUp);
	//WorldMatrix.r[CTransform::STATE_LOOK] = XMVector4Normalize(vLook	);

	//_float4x4 matrix;
	//XMStoreFloat4x4(&matrix, ParentMatrix);

	//EffectDesc.pWorldMatrix = &matrix;

	//m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Glow0"), TEXT("Layer_Particle"), &EffectDesc);
	//m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part0"), TEXT("Layer_Particle"), &EffectDesc);
	//m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part1"), TEXT("Layer_Particle"), &EffectDesc);
	//m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part2"), TEXT("Layer_Particle"), &EffectDesc);
	//m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part3"), TEXT("Layer_Particle"), &EffectDesc);
	//m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part4"), TEXT("Layer_Particle"), &EffectDesc);

	m_pParentObject->Take_Damage(m_eColliderPartType, vDir, fDamage, pParentObject, isBlowAttack);
}

HRESULT CSocketCollider::Add_Components(void* pArg)
{
	SOCKET_COLLIDER_DESC* pDesc = static_cast<SOCKET_COLLIDER_DESC*>(pArg);

	//pDesc->iType는 소켓 콜라이더 타입
	m_eColliderActionType = static_cast<COLLIDER_ACTION_TYPE>(pDesc->ColliderState.iActionType);
	m_eColliderPartType = static_cast<COLLIDER_PART_TYPE>(pDesc->ColliderState.iPartType);

	// ColliderState.iType는 바운딩 구조체 생성용 타입
	switch (pDesc->ColliderState.iType)
	{
	case 0:			//AABB
	{
		CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::COLLIDER_AABB;
		ColliderDesc.vExtents = pDesc->ColliderState.vValue;
		ColliderDesc.vCenter = pDesc->ColliderState.vCenter;

		if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
		break;
	}
	case 1:			//OBB
	{
		CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::COLLIDER_OBB;
		ColliderDesc.vExtents = pDesc->ColliderState.vValue;
		ColliderDesc.vCenter = pDesc->ColliderState.vCenter;
		ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);

		if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
		break;
	}
	case 2:			//SPHERE
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::COLLIDER_SPHERE;
		ColliderDesc.fRadius = pDesc->ColliderState.vValue.x;
		ColliderDesc.vCenter = pDesc->ColliderState.vCenter;

		if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
		break;
	}
	}


	return S_OK;
}

HRESULT CSocketCollider::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CSocketCollider * CSocketCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSocketCollider*		pInstance = new CSocketCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSocketCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSocketCollider::Clone(void * pArg)
{
	CSocketCollider*		pInstance = new CSocketCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSocketCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSocketCollider::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
