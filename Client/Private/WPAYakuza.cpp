#include "WPAYakuza.h"

#include "GameInstance.h"

CWPAYakuza::CWPAYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster { pDevice, pContext}
{
}

CWPAYakuza::CWPAYakuza(const CWPAYakuza& rhs)
	: CMonster { rhs }
{
}

HRESULT CWPAYakuza::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWPAYakuza::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CWPAYakuza::Priority_Tick(const _float& fTimeDelta)
{
}

void CWPAYakuza::Tick(const _float& fTimeDelta)
{
}

void CWPAYakuza::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CWPAYakuza::Render()
{
	return S_OK;
}

HRESULT CWPAYakuza::Add_Componenets()
{
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Model_Jimu"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_OBB;
	ColliderDesc.vExtents = _float3(0.8, 0.8, 0.8);
	ColliderDesc.vCenter = _float3(0, 0.f, 0);
	ColliderDesc.vRotation = _float3(0, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Anim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	//행동트리 저장

	return S_OK;
}

HRESULT CWPAYakuza::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CWPAYakuza::Change_Animation()
{
	switch (m_iState)
	{
	case MONSTER_IDLE:
		break;
	default:
		break;
	}
}

CWPAYakuza* CWPAYakuza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWPAYakuza* pInstance = new CWPAYakuza(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CWPAYakuza::Clone(void* pArg)
{
	CWPAYakuza* pInstance = new CWPAYakuza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWPAYakuza::Free()
{
	__super::Free();
}
