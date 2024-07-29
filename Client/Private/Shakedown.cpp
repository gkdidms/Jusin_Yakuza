#include "Shakedown.h"

#include "GameInstance.h"
#include "Collision_Manager.h"
#include "AI_Shakedown.h"
#include "Mesh.h"

#include "SocketCollider.h"

CShakedown::CShakedown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CShakedown::CShakedown(const CShakedown& rhs)
	: CMonster{ rhs }
{
}

HRESULT CShakedown::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShakedown::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_wstrModelName = TEXT("Jimu");

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.2f);

	m_Info.iMaxHP = 30.f;
	m_Info.iHp = m_Info.iMaxHP;


	return S_OK;
}

void CShakedown::Priority_Tick(const _float& fTimeDelta)
{
}

void CShakedown::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CShakedown::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CShakedown::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Shakedown"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.5, 0.8, 0.5);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom[DEFAULT_ANIMAITION]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_SyncAnim"),
		TEXT("Com_SyncAnim"), reinterpret_cast<CComponent**>(&m_pAnimCom[CUTSCENE_ANIMATION]))))
		return E_FAIL;

	//행동트리 저장
	CAI_Shakedown::AI_MONSTER_DESC AIDesc{};
	memcpy(AIDesc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIMATION_COMPONENT_TYPE_END);
	AIDesc.pCurrentAnimType = &m_iCurrentAnimType;
	AIDesc.pState = &m_iState;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_Shakedown*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Shakedown"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

void CShakedown::Change_Animation()
{
	m_isAnimLoop = false;

	__super::Change_Animation();

 	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		//e_kta_stand[e_kta_stand]
		m_strAnimName = "e_kta_stand";
		m_isAnimLoop = true;
		m_isDown = false;
		break;
	}
	case MONSTER_SHIFT_F:
	{
		//e_kta_shift_f[e_kta_shift_f]
		m_strAnimName = "e_kta_shift_f";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		//e_kta_shift_l[e_kta_shift_l]
		m_strAnimName = "e_kta_shift_l";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		//e_kta_shift_r[e_kta_shift_r]
		m_strAnimName = "e_kta_shift_r";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		//e_kta_shift_b[e_kta_shift_b]
		m_strAnimName = "e_kta_shift_b";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SWAY_B:
	{
		//e_kta_sway_b[e_kta_sway_b]
		m_strAnimName = "e_kta_sway_b";
		break;
	}
	case MONSTER_SWAY_F:
	{
		//e_kta_sway_f[e_kta_sway_f]
		m_strAnimName = "e_kta_sway_f";
		break;
	}
	case MONSTER_SWAY_L:
	{
		//e_kta_sway_l[e_kta_sway_l]
		m_strAnimName = "e_kta_sway_l";
		break;
	}
	case MONSTER_SWAY_R:
	{
		//e_kta_sway_r[e_kta_sway_r]
		m_strAnimName = "e_kta_sway_r";
		break;
	}
	case MONSTER_ATK_DOWN:
	{
		//e_kta_atk_down[e_kta_atk_down]
		m_strAnimName = "e_kta_atk_down";
		Shaking(0.3, 0.3, 0.35);
		break;
	}
	case MONSTER_RARIATTO:
	{
		//e_kta_atk_rariatto[e_kta_atk_rariatto]
		m_strAnimName = "e_kta_atk_rariatto";
		Shaking(0.2, 0.5, 0.2);
		break;
	}
	case MONSTER_GUARD_RUN:
	{
		//e_kta_atk_gurad_run[e_kta_atk_gurad_run]
		m_strAnimName = "e_kta_atk_gurad_run";

		// 첫 쉐이킹
		if (m_iShakedCount < 1)
		{
			m_iShakedCount++;
			Shaking(0.5, 0.2, 0.2);			// 돌진 시작할 때 한번
		}
		else
		{
			Shaking(0.8, 0.2, 0.2);			// 돌진 종료 시 한번
		}
		break;
	}
	case MONSTER_CMD_1:
	{
		//e_kta_cmb_a_01[e_kta_cmb_a_01]
		m_strAnimName = "e_kta_cmb_a_01";
		Shaking(0.3, 0.2, 0.2);
		break;
	}
	case MONSTER_CMD_2:
	{
		//e_kta_cmb_a_02[e_kta_cmb_a_02]
		m_strAnimName = "e_kta_cmb_a_02";
		Shaking(0.3, 0.2, 0.2);
		break;
	}
	case MONSTER_CMD_3:
	{
		//e_kta_cmb_a_03[e_kta_cmb_a_03]
		m_strAnimName = "e_kta_cmb_a_03";
		Shaking(0.55, 0.3, 0.3);
		break;
	}
	case MONSTER_GURAD_START:
	{
		m_strAnimName = "e_kta_guard_st";
		break;
	}
	case MONSTER_GURAD_LOOP:
	{
		m_strAnimName = "e_kta_guard_lp";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_GURAD_END:
	{
		m_strAnimName = "e_kta_guard_en";
		break;
	}
	case MONSTER_GURAD_FLOAT:
	{
		m_strAnimName = "e_kta_guard";
		break;
	}
	default:
		break;
	}

	if (FAILED(Setup_Animation()))
		return;
}

CShakedown* CShakedown::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShakedown* pInstance = new CShakedown(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CShakedown::Clone(void* pArg)
{
	CShakedown* pInstance = new CShakedown(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CShakedown::Free()
{
	__super::Free();
}
