#include "RushYakuza.h"

#include "GameInstance.h"
#include "Collision_Manager.h"
#include "AI_RushYakuza.h"
#include "SocketCollider.h"

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

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_wstrModelName = TEXT("Jimu");

	if (FAILED(Add_CharacterData()))
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

	__super::Tick(fTimeDelta);
}

void CRushYakuza::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	m_pCollisionManager->Add_ImpulseResolution(this);

	// 현재 켜져있는 Attack용 콜라이더 삽입
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::ATTACK && pPair.second->IsOn())
			m_pCollisionManager->Add_AttackCollider(pPair.second, CCollision_Manager::PLAYER);
	}

	// 현재 켜져있는 Hit용 콜라이더 삽입 (아직까지는 Hit용 콜라이더는 항상 켜져있음)
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::HIT && pPair.second->IsOn())
			m_pCollisionManager->Add_HitCollider(pPair.second, CCollision_Manager::ENEMY);
	}

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
	ColliderDesc.vExtents = _float3(0.3, 0.8, 0.3);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	//행동트리 저장
	CAI_RushYakuza::AI_MONSTER_DESC Desc{};
	Desc.pState = &m_iState;
	Desc.pAnim = m_pAnimCom;
	Desc.pThis = this;

	m_pTree = dynamic_cast<CAI_RushYakuza*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_RushYakuza"), &Desc));
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

	string strAnimName = "";

	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		strAnimName = "e_pnc_stand[e_pnc_stand]";
		iAnim = m_pAnimCom->Get_AnimationIndex("e_pnc_stand[e_pnc_stand]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_F:
	{
		//p_krh_shift_f[p_krh_shift_f]
		strAnimName = "p_krh_shift_f[p_krh_shift_f]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_f[p_krh_shift_f]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		////p_krh_shift_l[p_krh_shift_l]
		strAnimName = "p_krh_shift_l[p_krh_shift_l]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_l[p_krh_shift_l]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		//p_krh_shift_r[p_krh_shift_r]
		strAnimName = "p_krh_shift_r[p_krh_shift_r]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_r[p_krh_shift_r]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		//p_krh_shift_b[p_krh_shift_b]
		strAnimName = "p_krh_shift_b[p_krh_shift_b]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_b[p_krh_shift_b]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_FR:
	{
		//p_krh_shift_fr[p_krh_shift_fr]
		strAnimName = "p_krh_shift_fr[p_krh_shift_fr]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_fr[p_krh_shift_fr]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_FL:
	{
		//p_krh_shift_fl[p_krh_shift_fl]
		strAnimName = "p_krh_shift_fl[p_krh_shift_fl]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_fl[p_krh_shift_fl]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_BR:
	{
		//p_krh_shift_br[p_krh_shift_br]
		strAnimName = "p_krh_shift_br[p_krh_shift_br]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_br[p_krh_shift_br]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_BL:
	{
		//p_krh_shift_bl[p_krh_shift_bl]
		strAnimName = "p_krh_shift_bl[p_krh_shift_bl]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_shift_bl[p_krh_shift_bl]");
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_CMD_1:
	{
		//p_krh_cmb_01[p_krh_cmb_01]
		strAnimName = "p_krh_cmb_01[p_krh_cmb_01]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_01[p_krh_cmb_01]");
		break;
	}
	case MONSTER_CMD_2:
	{
		//p_krh_cmb_02[p_krh_cmb_02]
		strAnimName = "p_krh_cmb_02[p_krh_cmb_02]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_02[p_krh_cmb_02]");
		break;
	}
	case MONSTER_CMD_3:
	{
		//p_krh_cmb_03[p_krh_cmb_03]
		strAnimName = "p_krh_cmb_03[p_krh_cmb_03]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_03[p_krh_cmb_03]");
		break;
	}
	case MONSTER_CMD_4:
	{
		//p_krh_cmb_04[p_krh_cmb_04]
		strAnimName = "p_krh_cmb_04[p_krh_cmb_04]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_04[p_krh_cmb_04]");
		break;
	}
	case MONSTER_CMD_5:
	{
		//p_krh_cmb_05[p_krh_cmb_05]
		strAnimName = "p_krh_cmb_05[p_krh_cmb_05]";
		iAnim = m_pAnimCom->Get_AnimationIndex("p_krh_cmb_05[p_krh_cmb_05]");
		break;
	}
	case MONSTER_ANGRY_START:
	{
		//e_angry_typec[e_angry_typec]
		strAnimName = "e_angry_typec[e_angry_typec]";
		iAnim = m_pAnimCom->Get_AnimationIndex("e_angry_typec[e_angry_typec]");
		break;
	}
	case MONSTER_ANGRY_CHOP:
	{
		//e_knk_atk_chop[e_knk_atk_chop]
		strAnimName = "e_knk_atk_chop[e_knk_atk_chop]";
		iAnim = m_pAnimCom->Get_AnimationIndex("e_knk_atk_chop[e_knk_atk_chop]");
		break;
	}
	case MONSTER_ANGRY_KICK:
	{
		//e_knk_atk_kick[e_knk_atk_kick]
		strAnimName = "e_knk_atk_kick[e_knk_atk_kick]";
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
	m_pData->Set_CurrentAnimation(strAnimName);
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
