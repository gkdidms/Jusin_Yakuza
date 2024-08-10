#include "CarChase_Sedan.h"

#include "GameInstance.h"
#include "Highway_Taxi.h"

#include "AI_Sedan.h"

CCarChase_Sedan::CCarChase_Sedan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCarChase_Monster{ pDevice, pContext }
{
}

CCarChase_Sedan::CCarChase_Sedan(const CCarChase_Sedan& rhs)
	: CCarChase_Monster{ rhs }
{
}

HRESULT CCarChase_Sedan::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_Sedan::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iDir = DirFromPlayerPos();

	return S_OK;
}

void CCarChase_Sedan::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChase_Sedan::Tick(const _float& fTimeDelta)
{
	if (m_iWeaponType == DRV)
		m_iDir = Change_Dir();

	__super::Tick(fTimeDelta);
}

void CCarChase_Sedan::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CCarChase_Sedan::Change_Animation()
{
	m_isAnimLoop = false;

	switch (m_iState)
	{
	case CARCHASE_CURVA_L:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_car_drv_sit_curvel";

		break;
	}
	case CARCHASE_CURVA_R:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_car_drv_sit_curver";
		break;
	}
	case CARCHASE_SIT_DWN:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_car_drv_sit_dwn";
		break;
	}
	case CARCHASE_SIT_LP:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_car_drv_sit_lp";
		break;
	}
	case CARCHASE_AIML_CURVE_L:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_curvel";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_curvel";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_curvel";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_curvel";
		}
			
		break;
	}
	case CARCHASE_AIML_CURVE_R:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_curver";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_curver";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_curver";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_curver";
		}
			
		break;
	}
	case CARCHASE_AIML_DAM:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_dam";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_dam";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_dam";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_dam";
		}
			
		break;
	}
	case CARCHASE_AIML_DAD:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_ded";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_ded";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_ded";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_ded";
		}
			
		break;
	}
	case CARCHASE_AIML_DWN:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_dwn";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_dwn";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_dwn";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_dwn";
		}
			
		break;
	}
	case CARCHASE_AIML_HI:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_hi";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_hi";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_hi";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_hi";
		}
			
		break;
	}
	case CARCHASE_AIML_L45:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_l45";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_l45";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_l45";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_l45";
		}
			
		break;
	}
	case CARCHASE_AIML_LP:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_lp";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_lp";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_lp";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_lp";
		}
			
		break;
	}
	case CARCHASE_AIML_SHOT:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_shot";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_shot";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_shot";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_shot";
		}
			
		break;
	}
	case CARCHASE_AIML_STD_LP:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_std_lp";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_std_lp";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_std_lp";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_std_lp";
		}
			
		break;
	}
	case CARCHASE_AIML_BTOL:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_aiml_btol";
			
		break;
	}
	case CARCHASE_AIML_FTOL:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_aiml_ftol";
		break;
	}
	case CARCHASE_AIML_LTOB:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_aiml_ltol";
		break;
	}
	case CARCHASE_AIML_LTOF:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_aiml_ltol";
		break;
	}
	case CARCHASE_AIMR_CURVE_L:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_curvel";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_curvel";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_curvel";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_curvel";
		}

		break;
	}
	case CARCHASE_AIMR_CURVE_R:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_curver";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_curver";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_curver";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_curver";
		}

		break;
	}
	case CARCHASE_AIMR_DAM:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_dam";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_b_dam";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_dam";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_dam";
		}

		break;
	}
	case CARCHASE_AIMR_DAD:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_ded";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aimr_f_ded";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aimr_l_ded";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aimr_r_ded";
		}

		break;
	}
	case CARCHASE_AIMR_DWN:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_dwn";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_dwn";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_dwn";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aimr_r_dwn";
		}

		break;
	}
	case CARCHASE_AIMR_HI:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_hi";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_hi";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_hi";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_hi";
		}

		break;
	}
	case CARCHASE_AIMR_L45:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_l45";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_l45";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_l45";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_l45";
		}

		break;
	}
	case CARCHASE_AIMR_LP:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_lp";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_lp";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_lp";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_lp";
		}

		break;
	}
	case CARCHASE_AIMR_SHOT:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_shot";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_shot";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_shot";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_shot";
		}

		break;
	}
	case CARCHASE_AIMR_STD_LP:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_std_lp";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_std_lp";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_std_lp";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_std_lp";
		}

		break;
	}
	case CARCHASE_AIMR_BTOL:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_aimr_btol";

		break;
	}
	case CARCHASE_AIMR_FTOL:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_aimr_ftol";
		break;
	}
	case CARCHASE_AIMR_LTOB:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_aimr_ltol";
		break;
	}
	case CARCHASE_AIMR_LTOF:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_aimr_ltol";
		break;
	}
	case CARCHASE_SITL_EN:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_sitl_en";
		break;
	}
	case CARCHASE_SITL_LP:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_sitl_lp";
		break;
	}
	case CARCHASE_SITL_ST:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_sitl_st";
		break;
	}
	case CARCHASE_SITR_EN:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_sitr_en";
		break;
	}
	case CARCHASE_SITR_LP:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_sitr_lp";
		break;
	}
	case CARCHASE_SITR_ST:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_sitr_st";
		break;
	}
	default:
		break;
	}

	Setup_Animation();
}

HRESULT CCarChase_Sedan::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Shakedown"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CAI_Sedan::AI_CARCHASE_DESC Desc{};
	memcpy(Desc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	Desc.pCurrentAnimType = &m_iCurrentAnimType;
	Desc.pDir = &m_iDir;
	Desc.pState = &m_iState;
	Desc.pThis = this;
	Desc.pWeaponType = &m_iWeaponType;

	m_pTree = dynamic_cast<CAI_Sedan*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Sedan"), &Desc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

_uint CCarChase_Sedan::Change_Dir()
{
	//몬스터가 플레이어 왼쪽에 있다면
	//왼쪽에 존재하는 몬스터의 앞에 있는지, 오른쪽에 있는지
	CHighway_Taxi* pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Taxi"), 0));
	_vector vPlayerPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerLook = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vLeft = XMVector3Cross(vPlayerLook, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vPlayerLook);

	_vector vPos = vMonsterPos - vPlayerPos;

	_float vLeftDot = XMVectorGetX(XMVector3Dot(vPos, vLeft));
	_float vRightDot = XMVectorGetX(XMVector3Dot(vPos, vRight));
	_float vUpDot = XMVectorGetX(XMVector3Dot(vPos, vPlayerLook));
	_float vDownDot = XMVectorGetX(XMVector3Dot(vPos, vPlayerLook * -1.f));

	if (m_iWeaponType == GUN_L)
	{
		if (vLeftDot > 0)
		{
			if (acos(vDownDot) < XMConvertToRadians(45.f) * 0.5f)
			{
				return DIR_B;
			}
			else
				m_iDir = DIR_L;
		}
		else if (vRightDot > 0)
		{
			if (acos(vUpDot) < XMConvertToRadians(45.f) * 0.5f)
			{
				return DIR_F;
			}

			m_iDir = DIR_R;
		}
	}
	else if (m_iWeaponType == GUN_R)
	{
		if (vLeftDot > 0)
		{
			if (acos(vUpDot) < XMConvertToRadians(45.f) * 0.5f)
			{
				return DIR_F;
			}
			else
				m_iDir = DIR_L;
		}
		else if (vRightDot > 0)
		{
			if (acos(vDownDot) < XMConvertToRadians(45.f) * 0.5f)
			{
				return DIR_B;
			}

			m_iDir = DIR_R;
		}
	}

	return DIR_END;
}

CCarChase_Sedan* CCarChase_Sedan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarChase_Sedan* pInstance = new CCarChase_Sedan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CCarChase_Sedan::Clone(void* pArg)
{
	CCarChase_Sedan* pInstance = new CCarChase_Sedan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCarChase_Sedan::Free()
{
	__super::Free();
}
