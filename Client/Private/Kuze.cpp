#include "Kuze.h"

#include "GameInstance.h"
#include "Collision_Manager.h"
#include "AI_Kuze.h"
#include "Mesh.h"

#include "SocketCollider.h"

CKuze::CKuze(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CKuze::CKuze(const CKuze& rhs)
	: CMonster{ rhs }
{
}

HRESULT CKuze::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKuze::Initialize(void* pArg)
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

	// 네비 설정
	if (nullptr != pArg)
	{
		MONSTER_IODESC* gameobjDesc = (MONSTER_IODESC*)pArg;

		m_pNavigationCom->Set_Index(gameobjDesc->iNaviNum);
	}

	m_wstrModelName = TEXT("Jimu");

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(1, 0.5);
	m_pTransformCom->Set_Scale(0.95f, 0.95f, 0.95f);

	m_Info.iMaxHP = 500.f;
	m_Info.iHp = m_Info.iMaxHP;

	return S_OK;
}

void CKuze::Priority_Tick(const _float& fTimeDelta)
{
}

void CKuze::Tick(const _float& fTimeDelta)
{
	m_pTree->Tick(fTimeDelta);

	Change_Animation(); //애니메이션 변경

	m_pModelCom->Play_Animation(fTimeDelta, m_pAnimCom, m_isAnimLoop);

	Synchronize_Root(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	__super::Tick(fTimeDelta);
}

void CKuze::Late_Tick(const _float& fTimeDelta)
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

void CKuze::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
	//하는역활 -> 충돌이 일어났을때?
	m_isColl = true;
	m_fHitDamage = fDamage;

	//데미지 처리하기
	if (!m_isObjectDead)
	{
		m_Info.iHp -= fDamage;

		//무지성이라 변경해야함.
		if (m_iPage == ONE && m_Info.iHp <= 250)
		{
			m_iPage = TWO;
		}

		if (m_Info.iHp <= 0.f)
			m_isObjectDead = true;
	}
}

HRESULT CKuze::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Kuze_Fight"),
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
	CAI_Kuze::AI_MONSTER_DESC AIDesc{};
	AIDesc.pAnim = m_pAnimCom;
	AIDesc.pState = &m_iState;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_Kuze*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Kuze"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

void CKuze::Change_Animation()
{
	m_isAnimLoop = false;

	__super::Change_Animation();

  	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		//e_kuz_stand_blend[e_kuz_stand_blend]
		m_strAnimName = "e_kuz_stand_blend";
		m_isAnimLoop = true;
		m_isDown = false;
		break;
	}
	case MONSTER_SHIFT_F:
	{
		//e_kuz_shift_f[e_kuz_shift_f]
		m_strAnimName = "e_kuz_shift_f";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		//e_kuz_shift_l[e_kuz_shift_l]
		m_strAnimName = "e_kuz_shift_l";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		//e_kuz_shift_r[e_kuz_shift_r]
		m_strAnimName = "e_kuz_shift_r";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		//e_kuz_shift_b[e_kuz_shift_b]
		m_strAnimName = "e_kuz_shift_b";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SWAY_B:
	{
		//e_kuz_sway_b[e_kuz_sway_b]
		m_strAnimName = "e_kuz_sway_b";
		break;
	}
	case MONSTER_SWAY_F:
	{
		//e_kuz_sway_f[e_kuz_sway_f]
		m_strAnimName = "e_kuz_sway_f";
		break;
	}
	case MONSTER_SWAY_L:
	{
		//e_kuz_sway_l[e_kuz_sway_l]
		m_strAnimName = "e_kuz_sway_l";
		break;
	}
	case MONSTER_SWAY_R:
	{
		//e_kuz_sway_r[e_kuz_sway_r]
		m_strAnimName = "e_kuz_sway_r";
		break;
	}
	case MONSTER_ATK_DOWN:
	{
		//e_kuz_atk_down[e_kuz_atk_down]
		m_strAnimName = "e_kuz_atk_down";
		break;
	}
	case MONSTER_JAB:
	{
		//e_kuz_atk_jab[e_kuz_atk_jab]
		m_strAnimName = "e_kuz_atk_jab";
		break;
	}
	case MONSTER_CMD_A_1:
	{
		//e_kuz_cmb_a_01[e_kuz_cmb_a_01]
		m_strAnimName = "e_kuz_cmb_a_01";
		break;
	}
	case MONSTER_CMD_A_2:
	{
		//e_kuz_cmb_a_02[e_kuz_cmb_a_02]
		m_strAnimName = "e_kuz_cmb_a_02";
		break;
	}
	case MONSTER_CMD_A_3:
	{
		//e_kuz_cmb_a_03[e_kuz_cmb_a_03]
		m_strAnimName = "e_kuz_cmb_a_03";
		break;
	}
	case MONSTER_CMD_B_1:
	{
		//e_kuz_cmb_b_01[e_kuz_cmb_b_01]
		m_strAnimName = "e_kuz_cmb_b_01";
		break;
	}
	case MONSTER_CMD_B_2:
	{
		//e_kuz_cmb_b_02[e_kuz_cmb_b_02]
		m_strAnimName = "e_kuz_cmb_b_02";
		break;
	}
	case MONSTER_CMD_B_3:
	{
		//e_kuz_cmb_b_03[e_kuz_cmb_b_03]
		m_strAnimName = "e_kuz_cmb_b_03";
		break;
	}
	case MONSTER_CMD_HEADBUTT_1:
	{
		//e_kuz_cmb_headbutt_01[e_kuz_cmb_headbutt_01]
		m_strAnimName = "e_kuz_cmb_headbutt_01";
		break;
	}
	case MONSTER_CMD_HEADBUTT_2:
	{
		//e_kuz_cmb_headbutt_02[e_kuz_cmb_headbutt_02]
		m_strAnimName = "e_kuz_cmb_headbutt_02";
		break;
	}
	case MONSTER_CMD_RENDA_1:
	{
		//e_kuz_cmb_renda_01[e_kuz_cmb_renda_01]
		m_strAnimName = "e_kuz_cmb_renda_01";
		break;
	}
	case MONSTER_CMD_RENDA_2:
	{
		//e_kuz_cmb_renda_02_l[e_kuz_cmb_renda_02_l]
		m_strAnimName = "e_kuz_cmb_renda_02_l";
		break;
	}
	case MONSTER_CMD_RENDA_3:
	{
		//e_kuz_cmb_renda_02_l_fin[e_kuz_cmb_renda_02_l_fin]
		m_strAnimName = "e_kuz_cmb_renda_02_l_fin";
		break;
	}
	case MONSTER_HEAVY_ATTACK:
	{
		//e_kuz_atk_heavy[e_kuz_atk_heavy]
		m_strAnimName = "e_kuz_atk_heavy";
		break;
	}
	case MONSTER_HIJI_2REN:
	{
		//e_kuz_atk_hiji_2ren[e_kuz_atk_hiji_2ren]
		m_strAnimName = "e_kuz_atk_hiji_2ren";
		break;
	}
	case MONSTER_STEP_B:
	{
		m_strAnimName = "e_kuz_step_b";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_STEP_L:
	{
		m_strAnimName = "e_kuz_step_l";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_STEP_R:
	{
		m_strAnimName = "e_kuz_step_r";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_STEP_F:
	{
		m_strAnimName = "e_kuz_step_f";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_RUN:
	{
		m_strAnimName = "e_kuz_run";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_GURAD_START:
	{
		m_strAnimName = "e_kuz_guard_st";
		break;
	}
	case MONSTER_GURAD_LOOP:
	{
		m_strAnimName = "e_kuz_guard_lp";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_GURAD_END:
	{
		m_strAnimName = "e_kuz_guard_en";
		break;
	}
	case MONSTER_GURAD_FLOAT:
	{
		m_strAnimName = "e_kuz_guard";
		break;
	}
	default:
		break;
	}

	m_iAnim = m_pAnimCom->Get_AnimationIndex(m_strAnimName.c_str());

	if (m_iAnim == -1)
		return;

	// 실제로 애니메이션 체인지가 일어났을 때 켜져있던 어택 콜라이더를 전부 끈다
	if (m_pModelCom->Set_AnimationIndex(m_iAnim, m_pAnimCom->Get_Animations(), m_fChangeInterval))
		Off_Attack_Colliders();
	m_pData->Set_CurrentAnimation(m_strAnimName);
}

CKuze* CKuze::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKuze* pInstance = new CKuze(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CKuze::Clone(void* pArg)
{
	CKuze* pInstance = new CKuze(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CKuze::Free()
{
	__super::Free();

	Safe_Release(m_pTree);
}
