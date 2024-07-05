#include "RushYakuza.h"

#include "GameInstance.h"
#include "AI_RushYakuza.h"

#include "Bounding_OBB.h"
#include "Mesh.h"

CRushYakuza::CRushYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{pDevice, pContext}
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

	if (nullptr != pArg)
	{
		MONSTER_IODESC* gameobjDesc = (MONSTER_IODESC*)pArg;
		m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
		m_wstrModelName = gameobjDesc->wstrModelName;
		m_iShaderPassNum = 0;
	}

	if (FAILED(Add_Componenets()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(1, 0.5);
	//m_pModelCom->Set_AnimLoop(1, true);
	return S_OK;
}

void CRushYakuza::Priority_Tick(const _float& fTimeDelta)
{
}

void CRushYakuza::Tick(const _float& fTimeDelta)
{
	m_pTree->Tick(fTimeDelta);


	Change_Animation(); //애니메이션 변경

	m_pModelCom->Play_Animation(fTimeDelta, m_pAnimCom, m_isAnimLoop);

	Synchronize_Root(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CRushYakuza::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CRushYakuza::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		if (pMesh->Get_AlphaApply())
			m_pShaderCom->Begin(1);     //블랜드
		else
			m_pShaderCom->Begin(0);		//디폴트

		m_pModelCom->Render(i);

		i++;
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}


HRESULT CRushYakuza::Add_Componenets()
{
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, m_wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	//if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Model_Jimu"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

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
	CAI_RushYakuza::AI_OFFICE_YAKUZA_DESC Desc{};
	Desc.pModel = m_pModelCom;
	Desc.pState = &m_iState;
	Desc.pAnim = m_pAnimCom;

	m_pTree = CAI_RushYakuza::Create(&Desc);
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

HRESULT CRushYakuza::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CRushYakuza::Change_Animation()
{
	_uint iAnim = { 0 };
	m_isAnimLoop = false;

	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		iAnim = m_pAnimCom->Get_AnimationIndex("e_pnc_stand[e_pnc_stand]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_F:
	{
		//p_krh_shift_f[p_krh_shift_f]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_f[p_krh_shift_f]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		//p_krh_shift_l[p_krh_shift_l]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_l[p_krh_shift_l]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		//p_krh_shift_r[p_krh_shift_r]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_r[p_krh_shift_r]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		//p_krh_shift_b[p_krh_shift_b]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_b[p_krh_shift_b]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_FR:
	{
		//p_krh_shift_fr[p_krh_shift_fr]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_fr[p_krh_shift_fr]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_FL:
	{
		//p_krh_shift_fl[p_krh_shift_fl]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_fl[p_krh_shift_fl]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_BR:
	{
		//p_krh_shift_br[p_krh_shift_br]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_br[p_krh_shift_br]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_BL:
	{
		//p_krh_shift_bl[p_krh_shift_bl]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_bl[p_krh_shift_bl]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_CMD_1:
	{
		//p_krh_cmb_01[p_krh_cmb_01]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_01[p_krh_cmb_01]");
		break;
	}
	case MONSTER_CMD_2:
	{
		//p_krh_cmb_02[p_krh_cmb_02]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_02[p_krh_cmb_02]");
		break;
	}
	case MONSTER_CMD_3:
	{
		//p_krh_cmb_03[p_krh_cmb_03]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_03[p_krh_cmb_03]");
		break;
	}
	case MONSTER_CMD_4:
	{
		//p_krh_cmb_04[p_krh_cmb_04]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_04[p_krh_cmb_04]");
		break;
	}
	case MONSTER_CMD_5:
	{
		//p_krh_cmb_05[p_krh_cmb_05]
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_05[p_krh_cmb_05]");
		break;
	}
	case MONSTER_ANGRY_START:
	{
		//e_angry_typec[e_angry_typec]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_angry_typec[e_angry_typec]");
		break;
	}
	case MONSTER_ANGRY_CHOP:
	{
		//e_knk_atk_chop[e_knk_atk_chop]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_knk_atk_chop[e_knk_atk_chop]");
		break;
	}
	case MONSTER_ANGRY_KICK:
	{
		//e_knk_atk_kick[e_knk_atk_kick]
		iAnim = m_pAnimCom->Get_AnimationIndex("e_knk_atk_kick[e_knk_atk_kick]");
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

	Safe_Release(m_pTree);
}
