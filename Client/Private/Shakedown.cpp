#include "Shakedown.h"

#include "GameInstance.h"
#include "Mesh.h"

#include "AI_Shakedown.h"

CShakedown::CShakedown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster { pDevice, pContext}
{
}

CShakedown::CShakedown(const CShakedown& rhs)
	: CMonster { rhs }
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

	if (nullptr != pArg)
	{
		MONSTER_IODESC* gameobjDesc = (MONSTER_IODESC*)pArg;
		m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
		m_wstrModelName = gameobjDesc->wstrModelName;
	}


	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(1, 0.5);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 0.f, 5.f, 1.f));

	return S_OK;
}

void CShakedown::Priority_Tick(const _float& fTimeDelta)
{
}

void CShakedown::Tick(const _float& fTimeDelta)
{
	m_pTree->Tick(fTimeDelta);

	Change_Animation(); //애니메이션 변경

	m_pModelCom->Play_Animation(fTimeDelta, m_pAnimCom, m_isAnimLoop);

	Synchronize_Root(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CShakedown::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CShakedown::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Jimu"),
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
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	//행동트리 저장
	CAI_Shakedown::AI_MONSTER_DESC AIDesc{};
	AIDesc.pAnim = m_pAnimCom;
	AIDesc.pState = &m_iState;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_Shakedown*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Shakedown"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

HRESULT CShakedown::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CShakedown::Change_Animation()
{
	_uint iAnim = { 0 };
	m_isAnimLoop = false;

	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		//e_kta_stand[e_kta_stand]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_stand[e_kta_stand]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_F:
	{
		//e_kta_shift_f[e_kta_shift_f]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_shift_f[e_kta_shift_f]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		//e_kta_shift_l[e_kta_shift_l]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_shift_l[e_kta_shift_l]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		//e_kta_shift_r[e_kta_shift_r]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_shift_r[e_kta_shift_r]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		//e_kta_shift_b[e_kta_shift_b]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_shift_b[e_kta_shift_b]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SWAY_B:
	{
		//e_kta_sway_b[e_kta_sway_b]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_sway_b[e_kta_sway_b]");
		break;
	}
	case MONSTER_SWAY_F:
	{
		//e_kta_sway_f[e_kta_sway_f]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_sway_f[e_kta_sway_f]");
		break;
	}
	case MONSTER_SWAY_L:
	{
		//e_kta_sway_l[e_kta_sway_l]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_sway_l[e_kta_sway_l]");
		break;
	}
	case MONSTER_SWAY_R:
	{
		//e_kta_sway_r[e_kta_sway_r]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_sway_r[e_kta_sway_r]");
		break;
	}
	case MONSTER_ATK_DOWN:
	{
		//e_kta_atk_down[e_kta_atk_down]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_atk_down[e_kta_atk_down]");
		break;
	}
	case MONSTER_RARIATTO:
	{
		//e_kta_atk_rariatto[e_kta_atk_rariatto]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_atk_rariatto[e_kta_atk_rariatto]");
		break;
	}
	case MONSTER_GUARD_RUN:
	{
		//e_kta_atk_gurad_run[e_kta_atk_gurad_run]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_atk_gurad_run[e_kta_atk_gurad_run]");
		break;
	}
	case MONSTER_CMD_1:
	{
		//e_kta_cmb_a_01[e_kta_cmb_a_01]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_cmb_a_01[e_kta_cmb_a_01]");
		break;
	}
	case MONSTER_CMD_2:
	{
		//e_kta_cmb_a_02[e_kta_cmb_a_02]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_cmb_a_02[e_kta_cmb_a_02]");
		break;
	}
	case MONSTER_CMD_3:
	{
		//e_kta_cmb_a_03[e_kta_cmb_a_03]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_kta_cmb_a_03[e_kta_cmb_a_03]");
		break;
	}
	case MONSTER_DEATH:
	{
		break;
	}
	default:
		break;
	}

	if (iAnim == -1)
		return;

	m_pModelCom->Set_AnimationIndex(iAnim, m_pAnimCom->Get_Animations(), m_fChangeInterval);
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

	Safe_Release(m_pTree);
}
