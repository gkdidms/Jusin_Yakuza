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

	if (FAILED(Add_Componenets()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(1, 0.5);

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

HRESULT CShakedown::Render()
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

HRESULT CShakedown::Add_Componenets()
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
	CAI_Shakedown::AI_MONSTER_DESC AIDesc{};
	AIDesc.pAnim = m_pAnimCom;
	AIDesc.pState = &m_iState;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_Shakedown*>(m_pGameInstance->Add_BTNode(LEVEL_TEST, TEXT("Prototype_BTNode_Shakedown"), &AIDesc));
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
		iAnim = m_pAnimCom->Get_AnimationIndex("e_pnc_stand[e_pnc_stand]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_F:
	{
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_f[p_krh_shift_f]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_l[p_krh_shift_l]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_r[p_krh_shift_r]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_b[p_krh_shift_b]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SWAY_B:
	{
		break;
	}
	case MONSTER_SWAY_F:
	{
		break;
	}
	case MONSTER_SWAY_L:
	{
		break;
	}
	case MONSTER_SWAY_R:
	{
		break;
	}
	case MONSTER_ATK_DOWN:
	{
		break;
	}
	case MONSTER_RARIATTO:
	{
		break;
	}
	case MONSTER_GUARD_RUN:
	{
		break;
	}
	case MONSTER_CMD_1:
	{
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_01[p_krh_cmb_01]");
		break;
	}
	case MONSTER_CMD_2:
	{
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_02[p_krh_cmb_02]");
		break;
	}
	case MONSTER_CMD_3:
	{
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_03[p_krh_cmb_03]");
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
