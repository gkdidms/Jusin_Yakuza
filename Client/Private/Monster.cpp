#include "Monster.h"

#include "GameInstance.h"
#include "SystemManager.h"

#include "Camera.h"
#include "Mesh.h"
#include "AI_Monster.h";

#include "CharacterData.h"
#include "SocketCollider.h"
#include "SocketEffect.h"
#include "Collision_Manager.h"


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& rhs)
	: CLandObject { rhs }
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//테스트 데이터
	m_Info.iMaxHP = 100.f;
	m_Info.iHp = m_Info.iMaxHP;

	return S_OK;
}

void CMonster::Priority_Tick(const _float& fTimeDelta)
{
}

void CMonster::Tick(const _float& fTimeDelta)
{
	//충돌처리 초기화
	m_isColl = false;

	for (auto& pCollider : m_pColliders)
		pCollider.second->Tick(fTimeDelta);

	for (auto& pEffect : m_pEffects)
		pEffect.second->Tick(fTimeDelta);

	Animation_Event();

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
}

void CMonster::Late_Tick(const _float& fTimeDelta)
{
	//높이값 태우기
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	float fHeight = m_pNavigationCom->Compute_Height(vCurrentPos);

	vCurrentPos = XMVectorSetY(vCurrentPos, fHeight);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);


	for (auto& pCollider : m_pColliders)
		pCollider.second->Late_Tick(fTimeDelta);

	for (auto& pEffect : m_pEffects)
		pEffect.second->Late_Tick(fTimeDelta);
}

HRESULT CMonster::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		if (!strcmp("[l0]jacketw1", pMesh->Get_Name()))
		{
			if (m_isRimLight)
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_bool))))
					return E_FAIL;
		}
		else if (!strcmp("[l0]body_naked1", pMesh->Get_Name()))
		{
			if (m_isRimLight)
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_bool))))
					return E_FAIL;
		}
		else
		{
			_bool isfalse = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &isfalse, sizeof(_bool))))
				return E_FAIL;
		}

		_bool isCloth = true;
		string strMeshName = string(pMesh->Get_Name());
		if (strMeshName.find("hair") != string::npos || strMeshName.find("face") != string::npos ||
			strMeshName.find("foot") != string::npos || strMeshName.find("body") != string::npos ||
			strMeshName.find("eye") != string::npos)
			isCloth = false;

		m_pShaderCom->Bind_RawValue("g_isCloth", &isCloth, sizeof(_bool));

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		_bool isRS = true;
		_bool isRD = true;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
			isRS = false;
		m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;

		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

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

/*
1인자 : 어디에 맞았는지 전달받을 수 있음 (CSocketCollider::COLLIDER_PART_TYPE) 
2인자 : 이전 틱과 현재 틱의 방향
3인자 : 데미지 값
4인자 : 때리는 상태가 누군가?
5인자 : 넘어지는 어택인가?
*/

void CMonster::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
	//스웨이를 사용하고 있을 경우 충돌 x
	if (m_pTree->isSway())
		return;

	//하는역활 -> 충돌이 일어났을때?
	m_isColl = true;
	m_fHitDamage = fDamage;

	
	//데미지 처리하기 (가드사용하고있지 않을때)
	if (!m_isObjectDead && !m_pTree->isGuard())
	{
		m_Info.iHp -= fDamage;
		if (m_Info.iHp <= 0.f)
			m_isObjectDead = true;
	}
}

void CMonster::Animation_Event()
{
	auto& pCurEvents = m_pData->Get_CurrentEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pAnimCom->Get_AnimPosition());
		_double Duration = *(m_pAnimCom->Get_AnimDuration());

		if (CurPos >= pEvent.fPlayPosition && CurPos < Duration)
		{
			CSocketCollider* pCollider = m_pColliders.at(pEvent.iBoneIndex);

			switch (pEvent.iType)
			{
			case 0:
				pCollider->On();
				break;
			case 1:
				pCollider->Off();
				break;
			case 2:
#ifdef _DEBUG
				cout << "사운드 재생" << endl;
#endif // _DEBUG
				break;
			case 3:
#ifdef _DEBUG
				cout << "이펙트 재생" << endl;
#endif // _DEBUG
				break;
			}
		}

	}
}

string CMonster::Get_CurrentAnimationName()
{
	return m_pAnimCom->Get_AnimationName(m_pAnimCom->Get_CurrentAnimIndex());
}

void CMonster::Shaking(_float fRatio, _float fShakeDuration, _float fShakeMagnitude)
{
	if (!m_isShaked && Checked_Animation_Ratio(fRatio))
	{
		m_isShaked = true;
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
		pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, fShakeDuration, fShakeMagnitude);
	}
}

_bool CMonster::Checked_Animation_Ratio(_float fRatio)
{
	if (fRatio < *m_pModelCom->Get_AnimationCurrentPosition(m_pAnimCom) / *m_pModelCom->Get_AnimationDuration(m_pAnimCom))
		return true;

	return false;
}

void CMonster::Synchronize_Root(const _float& fTimeDelta)
{
	_vector vFF = XMVectorSetZ(XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove(m_pAnimCom)), 0);

	// m_pModelCom->Get_AnimChanged()  선형보간이 끝났는지
	// m_pModelCom->Get_AnimLerp() 선형보간이 필요한 애니메이션인지
	if (m_pAnimCom->Get_AnimChanged() || !m_pAnimCom->Get_AnimLerp(m_fChangeInterval))
	{
		if (m_pAnimCom->Get_AnimRestart(m_isAnimLoop))
		{
			XMStoreFloat4(&m_vPrevMove, XMVectorZero());
			m_fPrevSpeed = 0.f;
		}
		else
		{
			_float4 fMoveDir;
			_float fMoveSpeed = XMVectorGetX(XMVector3Length(vFF - XMLoadFloat4(&m_vPrevMove)));

			//Y값 이동을 죽인 방향으로 적용해야한다.
			_vector vTemp = XMVector3Normalize((vFF - XMLoadFloat4(&m_vPrevMove)));

			//Z가 Y처럼 쓰임
			vTemp = XMVectorSetZ(vTemp, XMVectorGetY(vTemp));
			XMStoreFloat4(&fMoveDir, XMVector3TransformNormal(XMVectorSetY(vTemp, 0.f), m_pTransformCom->Get_WorldMatrix()));

			if (0.01 > m_fPrevSpeed)
				m_fPrevSpeed = 0.f;

			m_pTransformCom->Go_Move_Custum(fMoveDir, m_fPrevSpeed, 1, m_pNavigationCom);
			m_fPrevSpeed = fMoveSpeed;

			XMStoreFloat4(&m_vPrevMove, vFF);
		}
	}
	else
	{
		// 선형보간중일때는 무조건 초기화
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
	}

	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CMonster::Add_Components()
{
	return S_OK;
}

HRESULT CMonster::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float isRimLight = { 0.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &isRimLight, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CMonster::Change_Animation()
{
	m_isAnimLoop = false;
	//히트, 데미지 관련 공통 애니메이션
	switch (m_iState)
	{
		case MONSTER_DWN_DNF_BOUND:
		{
			m_strAnimName = "c_dwn_dnb_bound";
			break;
		}
		case MONSTER_DWN_DNB_BOUND:
		{
			m_strAnimName = "c_dwn_dnf_bound";
			break;
		}
		case MONSTER_DWN_DNF_BOUND_G:
		{
			m_strAnimName = "c_dwn_dnb_bound_g";
			break;
		}
		case MONSTER_DWN_DNB_BOUND_G:
		{
			m_strAnimName = "c_dwn_dnf_bound_g";
			break;
		}
		case MONSTER_DAM_HEAD_LV01_R:
		{
			m_strAnimName = "c_dam_head_lv01_r";
			break;
		}
		case MONSTER_DAM_HEAD_LV01_L:
		{
			m_strAnimName = "c_dam_head_lv01_l";
			break;
		}
		case MONSTER_DAM_HEAD_LV01_F:
		{
			m_strAnimName = "c_dam_head_lv01_f";
			break;
		}
		case MONSTER_DAM_HEAD_LV01_B:
		{
			m_strAnimName = "c_dam_head_lv01_b";
			break;
		}
		case MONSTER_DAM_HEAD_LV02_R:
		{
			m_strAnimName = "c_dam_head_lv02_r";
			break;
		}
		case MONSTER_DAM_HEAD_LV02_L:
		{
			m_strAnimName = "c_dam_head_lv02_l";
			break;
		}
		case MONSTER_DAM_HEAD_LV02_F:
		{
			m_strAnimName = "c_dam_head_lv02_f";
			break;
		}
		case MONSTER_DAM_HEAD_LV02_B:
		{
			m_strAnimName = "c_dam_head_lv02_b";
			break;
		}
		case MONSTER_DAM_BODY_LV01_F:
		{
			m_strAnimName = "c_dam_body_lv01_f";
			break;
		}
		case MONSTER_DAM_BODY_LV01_B:
		{
			m_strAnimName = "c_dam_body_lv01_b";
			break;
		}
		case MONSTER_DAM_BODY_LV01_D:
		{
			m_strAnimName = "c_dam_body_lv01_b";
			break;
		}
		case MONSTER_DAM_BODY_LV02_F:
		{
			m_strAnimName = "c_dam_body_lv02_f";
			break;
		}
		case MONSTER_DAM_BODY_LV02_B:
		{
			m_strAnimName = "c_dam_body_lv02_b";
			break;
		}
		case MONSTER_DAM_BODY_LV02_D:
		{
			m_strAnimName = "c_dam_body_lv02_b";
			break;
		}
		case MONSTER_DWN_DIRECT_B:
		{
			m_strAnimName = "c_dwn_direct_b";
			break;
		}
		case MONSTER_DWN_DIRECT_F:
		{
			m_strAnimName = "c_dwn_direct_f";
			break;
		}
		case MONSTER_DWN_BODY_F:
		{
			m_strAnimName = "c_dwn_body_f";
			break;
		}
		case MONSTER_DWN_BODY_F_SP:
		{
			m_strAnimName = "c_dwn_body_f_sp";
			break;
		}
		case MONSTER_DWN_EXPLODE_F:
		{
			m_strAnimName = "c_dwn_explode_f";
			break;
		}
		case MONSTER_DWN_BODY_B:
		{
			m_strAnimName = "c_dwn_body_b";
			break;
		}
		case MONSTER_DWN_BODY_B_SP:
		{
			m_strAnimName = "c_dwn_body_b_sp";
			break;
		}
		case MONSTER_DWN_EXPLODE_B:
		{
			m_strAnimName = "c_dwn_explode_b";
			break;
		}
		case MONSTER_DWN_DIRECT_F_BOUND_G:
		{
			m_strAnimName = "c_dwn_direct_f_bound_g";
			break;
		}
		case MONSTER_DWN_DIRECT_B_BOUND_G:
		{
			m_strAnimName = "c_dwn_direct_b_bound_g";
			break;
		}
		case MONSTER_STANDUP_DNF_FAST:
		{
			m_strAnimName = "c_standup_dnf_fast";
			break;
		}
		case MONSTER_STANDUP_DNB_FAST:
		{
			m_strAnimName = "c_standup_dnb_fast";
			break;
		}
		case MONSTER_ANGRY_START:
		{
			//e_angry_typec[e_angry_typec]
			m_strAnimName = "e_angry_typec";
			break;
		}
		case MONSTER_ANGRY_CHOP:
		{
			//e_knk_atk_chop[e_knk_atk_chop]
			m_strAnimName = "e_knk_atk_chop";
			Shaking(0.3, 0.2, 0.3);
			break;
		}
		case MONSTER_ANGRY_KICK:
		{
			//e_knk_atk_kick[e_knk_atk_kick]
			m_strAnimName = "e_knk_atk_kick";
			Shaking(0.3, 0.2, 0.3);
			break;
		}
		case MONSTER_DED_L:
		{
			m_strAnimName = "c_ded_l";
			break;
		}
		case MONSTER_DED_R:
		{
			m_strAnimName = "c_ded_r";
			break;
		}
		case MONSTER_DED_F_1:
		{
			m_strAnimName = "c_ded_f_01";
			break;
		}
		case MONSTER_DED_F_2:
		{
			m_strAnimName = "c_ded_f_02";
			break;
		}
		case MONSTER_DED_F_3:
		{
			m_strAnimName = "c_ded_f_03";
			break;
		}
		case MONSTER_DED_B_1:
		{
			m_strAnimName = "c_ded_b_01";
			break;
		}
		case MONSTER_DED_B_2:
		{
			m_strAnimName = "c_ded_b_02";
			break;
		}
	}

}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pAnimCom);
	Safe_Release(m_pNavigationCom);
}
