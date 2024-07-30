#include "RushYakuza.h"

#include "GameInstance.h"
#include "Collision_Manager.h"
#include "AI_RushYakuza.h"
#include "Mesh.h"

#include "SocketCollider.h"

CRushYakuza::CRushYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CRushYakuza::CRushYakuza(const CRushYakuza& rhs)
	: CMonster{ rhs }
{
}

HRESULT CRushYakuza::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRushYakuza::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_wstrModelName = TEXT("Jimu");

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	return S_OK;
}

void CRushYakuza::Priority_Tick(const _float& fTimeDelta)
{
}

void CRushYakuza::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRushYakuza::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CRushYakuza::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_wstrModelName,
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
	CAI_RushYakuza::AI_MONSTER_DESC Desc{};
	Desc.pState = &m_iState;
	memcpy(Desc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIMATION_COMPONENT_TYPE_END);
	Desc.pCurrentAnimType = &m_iCurrentAnimType;
	Desc.pThis = this;

	m_pTree = dynamic_cast<CAI_RushYakuza*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_RushYakuza"), &Desc));
	if (nullptr == m_pTree)
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

void CRushYakuza::Change_Animation()
{
	m_isAnimLoop = false;

	__super::Change_Animation();

	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		m_strAnimName = "e_pnc_stand";
		m_isAnimLoop = true;
		m_isDown = false;
		break;
	}
	case MONSTER_SWAY_F:
	{
		m_strAnimName = "p_krh_sway_f";
		break;
	}
	case MONSTER_SWAY_B:
	{
		m_strAnimName = "p_krh_sway_b";
		break;
	}
	case MONSTER_SWAY_L:
	{
		m_strAnimName = "p_krh_sway_l";
		break;
	}
	case MONSTER_SWAY_R:
	{
		m_strAnimName = "p_krh_sway_r";
		break;
	}
	case MONSTER_SHIFT_F:
	{
		//p_krh_shift_f[p_krh_shift_f]
		m_strAnimName = "p_krh_shift_f";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		////p_krh_shift_l[p_krh_shift_l]
		m_strAnimName = "p_krh_shift_l";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		//p_krh_shift_r[p_krh_shift_r]
		m_strAnimName = "p_krh_shift_r";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		//p_krh_shift_b[p_krh_shift_b]
		m_strAnimName = "p_krh_shift_b";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_FR:
	{
		//p_krh_shift_fr[p_krh_shift_fr]
		m_strAnimName = "p_krh_shift_fr";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_FL:
	{
		//p_krh_shift_fl[p_krh_shift_fl]
		m_strAnimName = "p_krh_shift_fl";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_BR:
	{
		//p_krh_shift_br[p_krh_shift_br]
		m_strAnimName = "p_krh_shift_br";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_BL:
	{
		//p_krh_shift_bl[p_krh_shift_bl]
		m_strAnimName = "p_krh_shift_bl";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_PUNCH:
	{
		m_strAnimName = "p_krh_atk_punch_b";
		break;
	}
	case MONSTER_CMD_1:
	{
		//p_krh_cmb_01[p_krh_cmb_01]
		m_strAnimName = "p_krh_cmb_01";
		break;
	}
	case MONSTER_CMD_2:
	{
		//p_krh_cmb_02[p_krh_cmb_02]
		m_strAnimName = "p_krh_cmb_02";
		break;
	}
	case MONSTER_CMD_3:
	{
		//p_krh_cmb_03[p_krh_cmb_03]
		m_strAnimName = "p_krh_cmb_03";
		break;
	}
	case MONSTER_CMD_4:
	{
		//p_krh_cmb_04[p_krh_cmb_04]
		m_strAnimName = "p_krh_cmb_04";
		break;
	}
	case MONSTER_CMD_5:
	{
		//p_krh_cmb_05[p_krh_cmb_05]
		m_strAnimName = "p_krh_cmb_05";
		Shaking(0.3, 0.2, 0.2);
		break;
	}
	case MONSTER_GURAD_START:
	{
		m_strAnimName = "p_krh_guard";
		break; 
	}
	case MONSTER_GURAD_LOOP:
	{
		m_strAnimName = "p_krh_guard_lp";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_GURAD_END:
	{
		m_strAnimName = "p_krh_guard_en";
		break;
	}
	case MONSTER_GURAD_FLOAT:
	{
		m_strAnimName = "p_krh_guard_float";
		break;
	}
	default:
		break;
	}
	
	if (FAILED(Setup_Animation()))
		return;
}

CRushYakuza* CRushYakuza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRushYakuza* pInstance = new CRushYakuza(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CRushYakuza::Clone(void* pArg)
{
	CRushYakuza* pInstance = new CRushYakuza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CRushYakuza::Free()
{
	__super::Free();
}
