#include "Monster.h"

#include "GameInstance.h"

#include "CharacterData.h"
#include "SocketCollider.h"
#include "SocketEffect.h"
#include "Collision_Manager.h"

#include "Mesh.h"


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

	return S_OK;
}

void CMonster::Priority_Tick(const _float& fTimeDelta)
{
}

void CMonster::Tick(const _float& fTimeDelta)
{
	//�浹ó�� �ʱ�ȭ
	m_isColl = false;

	for (auto& pCollider : m_pColliders)
		pCollider.second->Tick(fTimeDelta);

	for (auto& pEffect : m_pEffects)
		pEffect.second->Tick(fTimeDelta);

	Animation_Event();

	// ���� �����ִ� Attack�� �ݶ��̴� ����
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::ATTACK && pPair.second->IsOn())
			m_pCollisionManager->Add_AttackCollider(pPair.second, CCollision_Manager::ENEMY);
	}

	// ���� �����ִ� Hit�� �ݶ��̴� ���� (���������� Hit�� �ݶ��̴��� �׻� ��������)
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::HIT && pPair.second->IsOn())
			m_pCollisionManager->Add_HitCollider(pPair.second, CCollision_Manager::ENEMY);
	}
}

void CMonster::Late_Tick(const _float& fTimeDelta)
{
	//���̰� �¿��
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


		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		_bool isRS = true;
		_bool isRD = true;
		if (!strcmp(pMesh->Get_Name(), "[l0]face_kiryu"))
		{
			isRS = false;
			isRD = false;
		}

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
			isRS = false;
		m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;

		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		if (pMesh->Get_AlphaApply())
			m_pShaderCom->Begin(1);     //����
		else
			m_pShaderCom->Begin(0);		//����Ʈ

		m_pModelCom->Render(i);

		i++;
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CMonster::Render_LightDepth()
{
	const _float4x4* ViewMatrixArray = m_pGameInstance->Get_Shadow_Transform_View_Float4x4();
	const _float4x4* ProjMatrixArray = m_pGameInstance->Get_Shadow_Transform_Proj_Float4x4();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_ViewMatrixArray", ViewMatrixArray, 3)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_ProjMatrixArray", ProjMatrixArray, 3)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			continue;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

/*
1���� : ��� �¾Ҵ��� ���޹��� �� ���� (CSocketCollider::COLLIDER_PART_TYPE) 
2���� : ���� ƽ�� ���� ƽ�� ����
3���� : ������ ��
4���� : ������ ���°� ������?
5���� : �Ѿ����� �����ΰ�?
*/

void CMonster::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
	//�ϴ¿�Ȱ -> �浹�� �Ͼ����?
	m_isColl = true;
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
				cout << "���� ���" << endl;
				break;
			case 3:
				cout << "����Ʈ ���" << endl;
				break;
			}
		}

	}
}

string CMonster::Get_CurrentAnimationName()
{
	return m_pAnimCom->Get_AnimationName(m_pAnimCom->Get_CurrentAnimIndex());
}

void CMonster::Synchronize_Root(const _float& fTimeDelta)
{
	_vector vFF = XMVector3TransformNormal(XMVectorSetZ(XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove(m_pAnimCom)), 0), m_pTransformCom->Get_WorldMatrix());

	// m_pModelCom->Get_AnimChanged()  ���������� ��������
	// m_pModelCom->Get_AnimLerp() ���������� �ʿ��� �ִϸ��̼�����
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

			//Y�� �̵��� ���� �������� �����ؾ��Ѵ�.
			XMStoreFloat4(&fMoveDir, XMVectorSetY(XMVector3Normalize(vFF - XMLoadFloat4(&m_vPrevMove)), 0.f));
			
			if (0.01 > m_fPrevSpeed)
				m_fPrevSpeed = 0.f;

			m_pTransformCom->Go_Straight_CustumSpeed(m_fPrevSpeed, 1);
			m_pTransformCom->Go_Move_Custum(fMoveDir, m_fPrevSpeed, 1, m_pNavigationCom);
			m_fPrevSpeed = fMoveSpeed;

			XMStoreFloat4(&m_vPrevMove, vFF);
		}
	}
	else
	{
		// �����������϶��� ������ �ʱ�ȭ
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
	return S_OK;
}

void CMonster::Change_Animation()
{
	//��Ʈ, ������ ���� ���� �ִϸ��̼�
	switch (m_iState)
	{
	case MONSTER_DWN_DNF_BOUND:
	{
		m_strAnimName = "c_dwn_dnb_bound[c_dwn_dnb_bound]";
		break;
	}
	case MONSTER_DWN_DNB_BOUND:
	{
		m_strAnimName = "c_dwn_dnf_bound[c_dwn_dnf_bound]";
		break;
	}
	case MONSTER_DAM_HEAD_LV01_R:
	{
		m_strAnimName = "c_dam_head_lv01_r[c_dam_head_lv01_r]";
		break;
	}
	case MONSTER_DAM_HEAD_LV01_L:
	{
		m_strAnimName = "c_dam_head_lv01_l[c_dam_head_lv01_l]";
		break;
	}
	case MONSTER_DAM_HEAD_LV02_R:
	{
		m_strAnimName = "c_dam_head_lv02_r[c_dam_head_lv02_r]";
		break;
	}
	case MONSTER_DAM_HEAD_LV02_L:
	{
		m_strAnimName = "c_dam_head_lv02_l[c_dam_head_lv02_l]";
		break;
	}
	case MONSTER_DWN_BODY_F:
	{
		m_strAnimName = "c_dwn_body_f[c_dwn_body_f]";
		break;
	}
	case MONSTER_DWN_BODY_F_SP:
	{
		m_strAnimName = "c_dwn_body_f_sp[c_dwn_body_f_sp]";
		break;
	}
	case MONSTER_DWN_EXPLODE_F:
	{
		m_strAnimName = "c_dwn_explode_f[c_dwn_explode_f]";
		break;
	}
	case MONSTER_STANDUP_DNF_FAST:
	{
		m_strAnimName = "c_standup_dnf_fast[c_standup_dnf_fast]";
		break;
	}
	case MONSTER_DED_L:
	{
		m_strAnimName = "c_ded_l[c_ded_l]";
		break;
	}

	case MONSTER_ANGRY_START:
	{
		//e_angry_typec[e_angry_typec]
		m_strAnimName = "e_angry_typec[e_angry_typec]";
		break;
	}
	case MONSTER_ANGRY_CHOP:
	{
		//e_knk_atk_chop[e_knk_atk_chop]
		m_strAnimName = "e_knk_atk_chop[e_knk_atk_chop]";
		break;
	}
	case MONSTER_ANGRY_KICK:
	{
		//e_knk_atk_kick[e_knk_atk_kick]
		m_strAnimName = "e_knk_atk_kick[e_knk_atk_kick]";
		break;
	}
	case MONSTER_DEATH:
	{
		break;
	}
	}

}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pAnimCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
}
