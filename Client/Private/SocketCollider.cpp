#include "SocketCollider.h"
#include "GameInstance.h"
#include "SystemManager.h"

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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSocketCollider::Priority_Tick(const _float& fTimeDelta)
{
}

void CSocketCollider::Tick(const _float& fTimeDelta)
{
	if(m_pColliderCom)
		m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
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

HRESULT CSocketCollider::Add_Components(void* pArg)
{
	SOKET_COLLIDER_DESC* pDesc = static_cast<SOKET_COLLIDER_DESC*>(pArg);

	switch (pDesc->ColliderState.iType)
	{
	case 0:			//AABB
	{
		CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::COLLIDER_AABB;
		ColliderDesc.vExtents = pDesc->ColliderState.vValue;
		ColliderDesc.vCenter = pDesc->ColliderState.vCenter;

		if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Collider"),
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

		if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Collider"),
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

		if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Collider"),
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
