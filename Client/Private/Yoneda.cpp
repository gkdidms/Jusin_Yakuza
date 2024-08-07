#include "Yoneda.h"

#include "GameInstance.h"
#include "Mesh.h"

#include "AI_Yoneda.h"


CYoneda::CYoneda(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext}
{
}

CYoneda::CYoneda(const CYoneda& rhs)
	: CMonster { rhs }
{
}

HRESULT CYoneda::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CYoneda::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_wstrModelName = TEXT("Jimu");

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	m_iWeaponType = BAREHAND;

	return S_OK;
}

void CYoneda::Priority_Tick(const _float& fTimeDelta)
{
}

void CYoneda::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CYoneda::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CYoneda::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Yoneda"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_OBB;
	ColliderDesc.vExtents = _float3(0.8, 0.8, 0.8);
	ColliderDesc.vCenter = _float3(0, 0.f, 0);
	ColliderDesc.vRotation = _float3(0, 0.f, 0.f);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom[DEFAULT]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_SyncAnim"),
		TEXT("Com_SyncAnim"), reinterpret_cast<CComponent**>(&m_pAnimCom[CUTSCENE]))))
		return E_FAIL;

	//행동트리 저장
	CAI_Yoneda::AI_YONEDA_DESC AIDesc{};
	AIDesc.pState = &m_iState;
	memcpy(AIDesc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	AIDesc.pCurrentAnimType = &m_iCurrentAnimType;
	AIDesc.pThis = this;
	AIDesc.pWeaponType = &m_iWeaponType;

	m_pTree = dynamic_cast<CAI_Yoneda*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT(""), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

void CYoneda::Change_Animation()
{
	m_isAnimLoop = false;

	__super::Change_Animation();

	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_stand";
		else
			m_strAnimName = "e_wpb_knife_stand_btl";

		m_isAnimLoop = true;
		m_isDown = false;
		break;
	}
	case MONSTER_SHIFT_F:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sae_shift_f";
		else
			m_strAnimName = "e_wpb_knife_shift_f";

		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sae_shift_l";
		else
			m_strAnimName = "e_wpb_knife_shift_l";

		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sae_shift_r";
		else
			m_strAnimName = "e_wpb_knife_shift_r";

		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sae_shift_b";
		else
			m_strAnimName = "e_wpb_knife_shift_b";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SWAY_B:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sway_b";
		else
			m_strAnimName = "e_wpb_knife_sway_b";

		break;
	}
	case MONSTER_SWAY_F:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sway_f";
		else
			m_strAnimName = "e_wpb_knife_sway_f";

		break;
	}
	case MONSTER_SWAY_R:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sway_r";
		else
			m_strAnimName = "e_wpb_knife_sway_r";

		break;
	}
	case MONSTER_SWAY_L:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sway_l";
		else
			m_strAnimName = "e_wpb_knife_sway_l";

		break;
	}
	case MONSTER_CMB_1:
	{
		if (m_iWeaponType == BAREHAND)
		{
			m_strAnimName = "e_sae_cmb_01";
			Shaking(0.3f, 0.2f, 0.2f);
		}
		else
		{
			m_strAnimName = "e_wpb_knife_cmb_01";
			Shaking(0.3f, 0.2f, 0.2f);
		}

		break;
	}
	case MONSTER_CMB_2:
	{
		if (m_iWeaponType == BAREHAND)
		{
			m_strAnimName = "e_sae_cmb_02";
			Shaking(0.3f, 0.2f, 0.2f);
		}
		else
		{
			m_strAnimName = "e_wpb_knife_cmb_02";
			Shaking(0.3f, 0.2f, 0.2f);
		}

		break;
	}
	case MONSTER_CMB_3:
	{
		m_strAnimName = "e_sae_cmb_03";
		Shaking(0.3f, 0.2f, 0.2f);
		break;
	}
	case MONSTER_HEAVY_ATTACK:
	{
		m_strAnimName = "e_sae_atk_heavy_f";
		Shaking(0.3f, 0.3f, 0.3f);
		break;
	}
	case MONSTER_HEAVY_RENDA:
	{
		m_strAnimName = "e_sae_atk_heavy_renda";
		Shaking(0.3f, 0.3f, 0.3f);
		break;
	}
	case MONSTER_KICK:
	{
		m_strAnimName = "e_sae_atk_kick_f";
		Shaking(0.3f, 0.3f, 0.3f);
		break;
	}
	case MONSTER_RUN_ATK:
	{
		m_strAnimName = "e_sae_atk_run";
		Shaking(0.3f, 0.3f, 0.3f);
		break;
	}
	case MONSTER_3REN:
	{
		m_strAnimName = "e_wpb_knife_atk_sting_3ren";
		break;
	}
	case MONSTER_ATK_NORAML:
	{
		m_strAnimName = "e_wpb_knife_atk";
		break;
	}
	default:
		break;
	}

	if (FAILED(Setup_Animation()))
		return;
}

CYoneda* CYoneda::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CYoneda* pInstance = new CYoneda(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CYoneda::Clone(void* pArg)
{
	CYoneda* pInstance = new CYoneda(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CYoneda::Free()
{
	__super::Free();

	Safe_Release(m_pTree);
}
