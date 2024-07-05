#include "Player.h"

#include "GameInstance.h"
#include "SystemManager.h"

#include "CharacterData.h"
#include "SoketCollider.h"

#include "BehaviorAnimation.h"
#include "Mesh.h"

#include "BTNode.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Componenets()))
		return E_FAIL;

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	Ready_AnimationTree();
	ZeroMemory(&m_MoveDirection, sizeof(_bool) * MOVE_DIRECTION_END);
	ZeroMemory(&m_InputDirection, sizeof(_bool) * MOVE_DIRECTION_END);

	return S_OK;
}

void CPlayer::Priority_Tick(const _float& fTimeDelta)
{
	if (m_pGameInstance->GetKeyState(DIK_0) == TAP)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0, 0, 0, 1));
	}
	if (m_pGameInstance->GetKeyState(DIK_UP) == TAP)
	{
		Style_Change(KRS);
	}
	if (m_pGameInstance->GetKeyState(DIK_DOWN) == TAP)
	{
		Style_Change(ADVENTURE);
	}

	Synchronize_Root(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	if (m_isAnimStart)
		m_pModelCom->Play_Animation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pCollider : m_pColliders)
		pCollider.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));


	KeyInput(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
	m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	Animation_Event();
}

void CPlayer::Tick(const _float& fTimeDelta)
{

}

void CPlayer::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow용 렌더 추가

	for (auto& pCollider : m_pColliders)
		pCollider.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));
}

HRESULT CPlayer::Render()
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

HRESULT CPlayer::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	/* 광원 기준의 뷰 변환행렬. */
	_vector vViewPos = m_pSystemManager->Get_ShadowViewPos();
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(vViewPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 1000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CPlayer::Ready_AnimationTree()
{
	for (size_t i = 0; i < (_uint)ADVENTURE_BEHAVIOR_STATE::ADVENTURE_BEHAVIOR_END; i++)
	{
		m_AnimationTree[ADVENTURE].emplace(i, CBehaviorAnimation::Create(ADVENTURE, i, this));
	}

	for (size_t i = 0; i < (_uint)KRS_BEHAVIOR_STATE::KRS_BEHAVIOR_END; i++)
	{
		m_AnimationTree[KRS].emplace(i, CBehaviorAnimation::Create(KRS, i, this));
	}

}

// 현재 애니메이션의 y축을 제거하고 사용하는 상태이다 (혹시 애니메이션의 y축 이동도 적용이 필요하다면 로직 수정이 필요함
void CPlayer::Synchronize_Root(const _float& fTimeDelta)
{
	_vector vFF = XMVector3TransformNormal(XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove()), m_pTransformCom->Get_WorldMatrix());
	vFF = XMVectorSet(XMVectorGetX(vFF), XMVectorGetZ(vFF), XMVectorGetY(vFF), 1.f);

	// 월드 행렬
	_matrix worldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float4 vQuaternion = *m_pModelCom->Get_AnimationCenterRotation();

	_vector scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, worldMatrix);

	_vector resultQuaternionVector = XMQuaternionMultiply(XMLoadFloat4(&vQuaternion), rotation);

	// m_pModelCom->Get_AnimChanged()  선형보간이 끝났는지
	// m_pModelCom->Get_AnimLerp() 선형보간이 필요한 애니메이션인지
	if (m_pModelCom->Get_AnimChanged() || !m_pModelCom->Get_AnimLerp())
	{
		if (m_pModelCom->Get_AnimRestart())
		{
			XMStoreFloat4(&m_vPrevMove, XMVectorZero());
			m_fPrevSpeed = 0.f;
		}	
		else
		{
			// 쿼터니언 회전값 적용은 중단 (추후 마저 진행예정)
			//_float4 v;
			//_vector diffQuaternionVector = XMQuaternionMultiply(resultQuaternionVector, XMQuaternionConjugate(XMLoadFloat4(&m_vPrevRotation)));
			//XMStoreFloat4(&v, diffQuaternionVector);
			//m_pTransformCom->Change_Rotation_Quaternion(v);

			//_float4 vb;
			//XMStoreFloat4(&vb, vFF - XMLoadFloat4(&m_vPrevMove));
			//m_pTransformCom->Go_Straight_CustumDir(vb, fTimeDelta);
			_float fMoveSpeed = XMVectorGetX(XMVector3Length(vFF - XMLoadFloat4(&m_vPrevMove)));
			m_pTransformCom->Go_Straight_CustumSpeed(m_fPrevSpeed, 1);
			m_fPrevSpeed = fMoveSpeed;
		}
	}
	else
	{
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat4(&m_vPrevMove));
		//m_pTransformCom->Go_Straight_CustumSpeed(m_fPrevSpeed);
	}
	
	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4(&m_vPrevMove, vFF);
	//m_vPrevRotation = vQuaternion;
	XMStoreFloat4(&m_vPrevRotation, resultQuaternionVector);
}

void CPlayer::Animation_Event()
{
	auto& pCurEvents = m_pData->Get_CurrentEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition());
		_double Duration = *(m_pModelCom->Get_AnimationDuration());

		if (CurPos >= pEvent.fPlayPosition && CurPos < Duration)
		{
			CSoketCollider* pCollider = m_pColliders.at(pEvent.iBoneIndex);

			switch (pEvent.iType)
			{
			case 0:
				pCollider->On();
				break;
			case 1:
				pCollider->Off();
				break;
			case 2:
				cout << "사운드 재생" << endl;
				break;
			case 3:
				cout << "이펙트 재생" << endl;
				break;
			}
		}
		
	}
}

void CPlayer::KeyInput(const _float& fTimeDelta)
{
	switch (m_eCurrentStyle)
	{
	case CPlayer::ADVENTURE:
		Adventure_KeyInput(fTimeDelta);
		break;
	case CPlayer::KRS:
		KRS_KeyInput(fTimeDelta);
		break;
	case CPlayer::KRH:
		KRH_KeyInput(fTimeDelta);
		break;
	case CPlayer::KRC:
		KRC_KeyInput(fTimeDelta);
		break;
	}



}

void CPlayer::Adventure_KeyInput(const _float& fTimeDelta)
{

	if (m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationEnd())
		m_iCurrentBehavior = (_uint)ADVENTURE_BEHAVIOR_STATE::IDLE;

	_bool isShift = { false };
	_bool isMove = { false };

	if (m_pGameInstance->GetKeyState(DIK_LSHIFT) == HOLD)
		isShift = true;

	if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
	{
		_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamLook());
		m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
		m_pTransformCom->LookAt_For_LandObject(vLookPos);
		isMove = true;
	}

	if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
	{
		_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamLook());
		m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
		m_pTransformCom->LookAt_For_LandObject(vLookPos);
		isMove = true;
	}

	if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
	{
		_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamRight());
		m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
		m_pTransformCom->LookAt_For_LandObject(vLookPos);
		isMove = true;
	}

	if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
	{
		_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamRight());
		m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
		m_pTransformCom->LookAt_For_LandObject(vLookPos);
		isMove = true;
	}

	if (!isMove && m_iCurrentBehavior != (_uint)ADVENTURE_BEHAVIOR_STATE::IDLE)
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stop();

}

void CPlayer::KRS_KeyInput(const _float& fTimeDelta)
{
	if (m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationEnd())
	{
		if((_uint)KRS_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
		m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::IDLE;
	}

	_bool isShift = { false };
	_bool isMove = { false };

	if (m_pGameInstance->GetKeyState(DIK_LSHIFT) == HOLD)
	{
		isShift = true;
	}

	if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
	{
		// 기존 행동을 초기화하고 어택으로 바꿔준다.
		if(m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::ATTACK)
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

		m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::ATTACK;
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
	}
	if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
	{
		// 현재 어택상태인지를 구분해서 마무리 액션을 실행시키거나
		// 그에 맞는 커맨드 액션을 실행시ㅕ켜야 한다.

		m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::ATTACK;
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count(true);
	}

	if (m_iCurrentBehavior < (_uint)KRS_BEHAVIOR_STATE::ATTACK)
	{
		if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamLook());

			_vector vLookDir = XMVectorSet(XMVectorGetX(vLookPos), XMVectorGetY(vLookPos), XMVectorGetZ(vLookPos), 0.f);
			_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::Shift : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;

			isMove = true;

			//m_MoveDirection[B] = true;

			////양수인 경우 같은 방향, 음수인 경우 반대방향, 0인경우 수직
			if (XMVectorGetX(XMVector3Dot(vLookDir, vPlayerLook)) < 0)
				m_MoveDirection[B] = true;
			else if (XMVectorGetX(XMVector3Dot(vLookDir, vPlayerLook)) > 0)
				m_MoveDirection[F] = true;


			m_InputDirection[F] = true;
			Compute_MoveDirection_FB();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}

		if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
			
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamLook());
			m_iCurrentBehavior = isShift ? (_uint)KRS_BEHAVIOR_STATE::WALK : (_uint)KRS_BEHAVIOR_STATE::RUN;

			m_InputDirection[B] = true;
			Compute_MoveDirection_FB();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);

			m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::Shift : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
			m_MoveDirection[B] = true;

			isMove = true;
		}

		if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamRight());
			m_iCurrentBehavior = isShift ? (_uint)KRS_BEHAVIOR_STATE::WALK : (_uint)KRS_BEHAVIOR_STATE::RUN;

			m_InputDirection[L] = true;
			Compute_MoveDirection_RL();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);

			m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::Shift : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
			m_MoveDirection[L] = true;

			isMove = true;
		}

		if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamRight());
			m_iCurrentBehavior = isShift ? (_uint)KRS_BEHAVIOR_STATE::WALK : (_uint)KRS_BEHAVIOR_STATE::RUN;

			m_InputDirection[R] = true;
			Compute_MoveDirection_RL();

			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::Shift : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
			m_MoveDirection[R] = true;

			isMove = true;
		}

		if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
		{
			// 움직임 관련 키 입력이 없을 때에는 무조건 Back방향으로 이동해야하기 때문에 키입력여부 체크해서 방향 초기화
			if (!isMove)
				Reset_MoveDirection();

			m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::SWAY;
		}
	}

	if (!isMove && m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN || m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::WALK)
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stop();
}

void CPlayer::KRH_KeyInput(const _float& fTimeDelta)
{
}

void CPlayer::KRC_KeyInput(const _float& fTimeDelta)
{
}

HRESULT CPlayer::Add_Componenets()
{
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Model_Kiryu"),
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

	return S_OK;
}

HRESULT CPlayer::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ModelWorldMatrix)))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer::Add_CharacterData()
{
	m_pData = CCharacterData::Create(TEXT("Kiryu"));

	if (nullptr == m_pData)
		return E_FAIL;

	Apply_ChracterData();

	return S_OK;
}

void CPlayer::Apply_ChracterData()
{
	auto& pAlphaMeshes = m_pData->Get_AlphaMeshes();
	auto pMeshes = m_pModelCom->Get_Meshes();

	for (size_t i = 0; i < pMeshes.size(); i++)
	{
		for (auto& iMeshIndex : pAlphaMeshes)
		{
			if (i == iMeshIndex)
				pMeshes[i]->Set_AlphaApply(true);
		}
	}

	auto& pLoopAnimations = m_pData->Get_LoopAnimations();
	for (auto& iAnimIndex : pLoopAnimations)
	{
		m_pModelCom->Set_AnimLoop(iAnimIndex, true);
	}

	auto& pColliders = m_pData->Get_Colliders();

	for (auto& Collider : pColliders)
	{
		CSoketCollider::SOKET_COLLIDER_DESC Desc{};
		Desc.pParentMatrix = &m_ModelWorldMatrix;
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix_AtIndex(Collider.first);
		Desc.iBoneIndex = Collider.first;
		Desc.ColliderState = Collider.second;

		CGameObject* pSoketCollider = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SoketCollider"), &Desc);
		if (nullptr == pSoketCollider)
			return;
	
		auto [it, success] = m_pColliders.emplace(Collider.first, static_cast<CSoketCollider*>(pSoketCollider));

		//생성한 모든 콜라이더는 일단 꺼둔다.
		// 몸체에 붙일 (플레이어가 피격당할) 콜라이더는 항시 켜져있어야하므로 툴에서찍지않음
		it->second->Off();
	}

}

void CPlayer::Change_Animation(_uint iIndex, _float fInterval)
{
	if (m_pModelCom->Set_AnimationIndex(iIndex, fInterval))
	{
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
		m_fPrevSpeed = 0.f;
	}
			
	
	string strAnimName = string(m_pModelCom->Get_AnimationName(iIndex));
	strAnimName = m_pGameInstance->Extract_String(strAnimName, '[', ']');
	m_pData->Set_CurrentAnimation(strAnimName);
}

void CPlayer::Style_Change(BATTLE_STYLE eStyle)
{
	// 설정한 스타일의 첫번째 액션을 실행시킨다 (배틀모드들은 무조건 첫번째에 배틀 시작 액션을 둘 예정)
	m_eCurrentStyle = eStyle;
	m_iCurrentBehavior = 0;
	m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
}

void CPlayer::Reset_MoveDirection()
{
	ZeroMemory(m_MoveDirection, sizeof(_bool) * MOVE_DIRECTION_END);
	ZeroMemory(m_InputDirection, sizeof(_bool) * MOVE_DIRECTION_END);
}

void CPlayer::Compute_MoveDirection_FB()
{
	_vector vCamLook = m_pGameInstance->Get_CamLook();
	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	
	// 방향벡터간 내적의 결과값이 양수면 90도미만(같은방향), 음수면 90도 초과(반대방향), 0이면 직교한다.
	
	// 지금 입력한 키가 앞이면 앞으로 스웨이할건데
	if (m_InputDirection[F])
	{
		// 이 때 캐릭터가 카메라랑 같은 방향을 보고있다면 Front 스웨이
		if (XMVectorGetX(XMVector3Dot(vCamLook, vPlayerLook)) < 0)
			m_MoveDirection[F] = true;
		// 캐릭터가 카메라랑 반대방향을 보고 있다면 Back 스웨이
		else
			m_MoveDirection[B] = true;
	}
	// 지금 입력한 키가 뒤면 뒤로 스웨이할건데, 
	if (m_InputDirection[B])
	{
		//캐릭터가 카메라랑 같은 방향을 보고있다면 Back방향 스웨이
		if (XMVectorGetX(XMVector3Dot(vCamLook, vPlayerLook)) < 0)
			m_MoveDirection[B] = true;
		// 캐릭터가 카메라랑 반대방향을 보고있다면 Front 스웨이
		else
			m_MoveDirection[F] = true;
	}


}

void CPlayer::Compute_MoveDirection_RL()
{
	_vector vCamRight = m_pGameInstance->Get_CamRight();
	_vector vPlayerRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	// 지금 입력한 키가 오른쪽
	if (m_InputDirection[L])
	{
		// 오른쪽으로 스웨이할건데, 캐릭터의 오른쪽이 카메라의 오른쪽과 같은 방향이라면 그냥 오른쪽 스웨이모션
		if (XMVectorGetX(XMVector3Dot(vCamRight, vPlayerRight)) < 0)
			m_MoveDirection[R] = true;
		// 캐릭터의 오른쪽이 카메라의 오른쪽과 반대 방향이라면 왼쪽 스웨이모션
		else if (XMVectorGetX(XMVector3Dot(vCamRight, vPlayerRight)) > 0)
			m_MoveDirection[L] = true;

	}
	// 지금 입력한 키가 뒤면 뒤로 스웨이할건데, 
	if (m_InputDirection[R])
	{
		if (XMVectorGetX(XMVector3Dot(vCamRight, vPlayerRight)) < 0)
			m_MoveDirection[L] = true;
		else if (XMVectorGetX(XMVector3Dot(vCamRight, vPlayerRight)) > 0)
			m_MoveDirection[R] = true;
	}

}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& pCollider : m_pColliders)
		Safe_Release(pCollider.second);
	m_pColliders.clear();
	Safe_Release(m_pData);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}