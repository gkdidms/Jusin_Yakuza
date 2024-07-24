#include "Adventure.h"

#include "GameInstance.h"
#include "Mesh.h"

#include "SocketCollider.h"
#include "AI_Monster.h"

#include "AI_Passersby.h"

CAdventure::CAdventure(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLandObject { pDevice, pContext }
{
}

CAdventure::CAdventure(const CAdventure& rhs)
	: CLandObject { rhs }
{
}

HRESULT CAdventure::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAdventure::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CAdventure::Priority_Tick(const _float& fTimeDelta)
{
}

void CAdventure::Tick(const _float& fTimeDelta)
{
	m_pTree->Tick(fTimeDelta);

	Change_Animation();

	//길찾기 알고리즘
	
	
	if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
	{
		_bool isPicking = false;
		_vector vGoalPos = m_pGameInstance->Picking(&isPicking);
		if (isPicking)
			m_pAStartCom->Start_Root(m_pNavigationCom, vGoalPos);
	}
	
	Move_AStart();

	m_pModelCom->Play_Animation(fTimeDelta, m_pAnimCom, m_isAnimLoop);

	Synchronize_Root(fTimeDelta);
}

void CAdventure::Late_Tick(const _float& fTimeDelta)
{
	//높이값 태우기
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	float fHeight = m_pNavigationCom->Compute_Height(vCurrentPos);

	vCurrentPos = XMVectorSetY(vCurrentPos, fHeight);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
}

HRESULT CAdventure::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
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

void CAdventure::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
	m_isColl = true;
}

void CAdventure::Animation_Event()
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
#endif // DEBUG
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

string CAdventure::Get_CurrentAnimationName()
{
	return m_pAnimCom->Get_AnimationName(m_pAnimCom->Get_CurrentAnimIndex());
}

_bool CAdventure::Checked_Animation_Ratio(_float fRatio)
{
	if (fRatio < *m_pModelCom->Get_AnimationCurrentPosition(m_pAnimCom) / *m_pModelCom->Get_AnimationDuration(m_pAnimCom))
		return true;

	return false;
}

void CAdventure::Change_Animation()
{
	m_isAnimLoop = false;

	switch (m_iState)
	{
	case ADVENTURE_IDLE:
	{
		m_strAnimName = "m_nml_tlk_stand_kamae";
		m_isAnimLoop = true;
		break;
	}
	case ADVENTURE_WALK:
	{
		m_strAnimName = "p_mov_walk";
		m_isAnimLoop = true;
		break;
	}
	case ADVENTURE_WALK_S:
	{
		m_strAnimName = "p_mov_walk_s";
		break;
	}
	case ADVENTURE_WALK_EN:
	{
		m_strAnimName = "p_mov_walk_en";
		break;
	}
	case ADVENTURE_HIT_L:
	{
		m_strAnimName = "m_hml_act_walk_hit_l";
		break;
	}
	case ADVENTURE_HIT_R:
	{
		m_strAnimName = "m_hml_act_walk_hit_r";
		break;
	}
	default:
		break;
	}

	m_iAnim = m_pAnimCom->Get_AnimationIndex(m_strAnimName.c_str());
	
	if (m_iAnim == -1)
		return;

	m_pModelCom->Set_AnimationIndex(m_iAnim, m_pAnimCom->Get_Animations(), m_fChangeInterval);
}

void CAdventure::Synchronize_Root(const _float& fTimeDelta)
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

void CAdventure::Move_AStart()
{
	list<CCell*>& BestCells = m_pAStartCom->Get_BestList();

	if (BestCells.empty())
		return;

	_vector	vDir = BestCells.front()->Get_WayPoint() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDist = XMVectorGetX(XMVector3Length(vDir));
	
	m_pTransformCom->LookAt_For_LandObject(BestCells.front()->Get_WayPoint());
	
	if (1.f >= fDist)
		BestCells.pop_front();
}

HRESULT CAdventure::Add_Components()
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

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_NPC"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_AStart"),
		TEXT("Com_AStart"), reinterpret_cast<CComponent**>(&m_pAStartCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAdventure::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CAdventure::Free()
{
	__super::Free();

	Safe_Release(m_pAnimCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTree);
	Safe_Release(m_pAStartCom);
}
