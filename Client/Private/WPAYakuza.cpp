#include "WPAYakuza.h"

#include "GameInstance.h"
#include "Collision_Manager.h"
#include "AI_WPAYakuza.h"
#include "Mesh.h"

#include "SocketCollider.h"

CWPAYakuza::CWPAYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CWPAYakuza::CWPAYakuza(const CWPAYakuza& rhs)
	: CMonster{ rhs }
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

	if (nullptr != pArg)
	{
		MONSTER_IODESC* gameobjDesc = (MONSTER_IODESC*)pArg;
		m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
		m_wstrModelName = gameobjDesc->wstrModelName;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_wstrModelName = TEXT("Jimu");

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	return S_OK;
}

void CWPAYakuza::Priority_Tick(const _float& fTimeDelta)
{
}

void CWPAYakuza::Tick(const _float& fTimeDelta)
{
	m_pTree->Tick(fTimeDelta);

	Change_Animation(); //애니메이션 변경

	m_pModelCom->Play_Animation(fTimeDelta, m_pAnimCom, m_isAnimLoop);

	Synchronize_Root(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	__super::Tick(fTimeDelta);
}

void CWPAYakuza::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	m_pCollisionManager->Add_ImpulseResolution(this);

	// 현재 켜져있는 Attack용 콜라이더 삽입
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::ATTACK && pPair.second->IsOn())
			m_pCollisionManager->Add_AttackCollider(pPair.second, CCollision_Manager::ENEMY);
	}

	// 현재 켜져있는 Hit용 콜라이더 삽입 (아직까지는 Hit용 콜라이더는 항상 켜져있음)
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::HIT && pPair.second->IsOn())
			m_pCollisionManager->Add_HitCollider(pPair.second, CCollision_Manager::ENEMY);
	}


	__super::Late_Tick(fTimeDelta);
}

HRESULT CWPAYakuza::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, m_wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.3, 0.8, 0.3);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Anim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	//행동트리 저장
	CAI_WPAYakuza::AI_MONSTER_DESC AIDesc{};
	AIDesc.pState = &m_iState;
	AIDesc.pAnim = m_pAnimCom;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_WPAYakuza*>(m_pGameInstance->Add_BTNode(LEVEL_TEST, TEXT("Prototype_BTNode_WPAYakuza"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

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
	__super::Change_Animation();

	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		m_strAnimName = "e_wpa_stand_btl[e_wpa_stand_btl]";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_F:
	{
		//e_wpa_shift_f[e_wpa_shift_f]
		m_strAnimName = "e_wpa_shift_f[e_wpa_shift_f]";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		//e_wpa_shift_l[e_wpa_shift_l]
		m_strAnimName = "e_wpa_shift_l[e_wpa_shift_l]";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		//e_wpa_shift_r[e_wpa_shift_r]
		m_strAnimName = "e_wpa_shift_r[e_wpa_shift_r]";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		//e_wpa_shift_b[e_wpa_shift_b]
		m_strAnimName = "e_wpa_shift_b[e_wpa_shift_b]";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SWAY_B:
	{
		//e_wpa_sway_b[e_wpa_sway_b]
		m_strAnimName = "e_wpa_sway_b[e_wpa_sway_b]";
		break;
	}
	case MONSTER_SWAY_F:
	{
		//e_wpa_sway_f[e_wpa_sway_f]
		m_strAnimName = "e_wpa_sway_f[e_wpa_sway_f]";
		break;
	}
	case MONSTER_SWAY_R:
	{
		//e_wpa_sway_r[e_wpa_sway_r]
		m_strAnimName = "e_wpa_sway_r[e_wpa_sway_r]";
		break;
	}
	case MONSTER_SWAY_L:
	{
		//e_wpa_sway_l[e_wpa_sway_l]
		m_strAnimName = "e_wpa_sway_l[e_wpa_sway_l]";
		break;
	}
	case MONSTER_CMD_1:
	{
		//e_wpa_cmb_01[e_wpa_cmb_01]
		m_strAnimName = "e_wpa_cmb_01[e_wpa_cmb_01]";
		break;
	}
	case MONSTER_CMD_2:
	{
		//e_wpa_cmb_02[e_wpa_cmb_02]
		m_strAnimName = "e_wpa_cmb_02[e_wpa_cmb_02]";
		break;
	}

	default:
		break;
	}

	m_iAnim = m_pAnimCom->Get_AnimationIndex(m_strAnimName.c_str());

	if (m_iAnim == -1)
		return;

	m_pModelCom->Set_AnimationIndex(m_iAnim, m_pAnimCom->Get_Animations(), m_fChangeInterval);
	m_pData->Set_CurrentAnimation(m_strAnimName);
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

	Safe_Release(m_pTree);
}
