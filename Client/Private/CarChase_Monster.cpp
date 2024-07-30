#include "CarChase_Monster.h"

#include "GameInstance.h"

#include "AI_CarChase.h"

CCarChase_Monster::CCarChase_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster { pDevice, pContext }
{
}

CCarChase_Monster::CCarChase_Monster(const CCarChase_Monster& rhs)
	: CMonster{ rhs }
{
}

HRESULT CCarChase_Monster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_Monster::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CARCHASE_MONSTER_DESC* pDesc = static_cast<CARCHASE_MONSTER_DESC*>(pArg);
	m_pParentMatrix = pDesc->pParentMatrix;
	m_iWeaponType = pDesc->iWeaponType;

	m_iCurrentAnimType = CLandObject::DEFAULT;

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

	return S_OK;
}

void CCarChase_Monster::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChase_Monster::Tick(const _float& fTimeDelta)
{
	m_pTree->Tick(fTimeDelta);

	Change_Animation(); //애니메이션 변경

	//_bool isRoot = m_iCurrentAnimType != CUTSCENE;
	m_pModelCom->Play_Animation_Monster(fTimeDelta, m_pAnimCom[m_iCurrentAnimType], m_isAnimLoop, false);
	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
	//충돌처리 초기화
	m_isColl = false;

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_ModelWorldMatrix));
}

void CCarChase_Monster::Late_Tick(const _float& fTimeDelta)
{
	if (m_iPreAnimType == m_iCurrentAnimType)
		m_iPreAnimType = m_iCurrentAnimType;

	//컬링
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

void CCarChase_Monster::Change_Animation()
{
}

HRESULT CCarChase_Monster::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.5, 0.8, 0.5);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_CarChaseAnim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom[CLandObject::DEFAULT]))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_SyncAnim"),
		TEXT("Com_SyncAnim"), reinterpret_cast<CComponent**>(&m_pAnimCom[CLandObject::CUTSCENE]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarChase_Monster::Bind_ResourceData()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ModelWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CCarChase_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pTree);
}
