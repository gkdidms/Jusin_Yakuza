#include "Player.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Collision_Manager.h"

#include "CharacterData.h"
#include "SocketCollider.h"
#include "SocketEffect.h"

#include "BehaviorAnimation.h"
#include "Mesh.h"

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
	m_wstrModelName = TEXT("Kiryu");

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Ready_AnimationTree();

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	ZeroMemory(&m_MoveDirection, sizeof(_bool) * MOVE_DIRECTION_END);
	ZeroMemory(&m_InputDirection, sizeof(_bool) * MOVE_DIRECTION_END);

	return S_OK;
}

void CPlayer::Priority_Tick(const _float& fTimeDelta)
{
	if (m_pModelCom->Get_AnimFinished())
	{
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSetY(XMLoadFloat4(&m_vPrevMove), 0));
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
	}
}

void CPlayer::Tick(const _float& fTimeDelta)
{
	m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
	m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

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

	for (auto& pEffect : m_pEffects)
		pEffect.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));


	KeyInput(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	Animation_Event();
}

void CPlayer::Late_Tick(const _float& fTimeDelta)
{
	//m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	//m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow�� ���� �߰�
	m_pCollisionManager->Add_ImpulseResolution(this);

	for (auto& pCollider : m_pColliders)
		pCollider.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pEffects)
		pEffect.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	// ���� �����ִ� Attack�� �ݶ��̴� ����
	for (auto& pPair : m_pColliders)
	{
		if(pPair.second->Get_CollierType() == CSocketCollider::ATTACK && pPair.second->IsOn())
			m_pCollisionManager->Add_AttackCollider(pPair.second, CCollision_Manager::PLAYER);
	}

	// ���� �����ִ� Hit�� �ݶ��̴� ���� (���������� Hit�� �ݶ��̴��� �׻� ��������)
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::HIT && pPair.second->IsOn())
			m_pCollisionManager->Add_HitCollider(pPair.second, CCollision_Manager::PLAYER);
	}
	
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

HRESULT CPlayer::Render_LightDepth(_uint iIndex)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	/* ���� ������ �� ��ȯ���. */
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

_bool CPlayer::Intersect(CLandObject* pTargetObject)
{
	return _bool();
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

// ���� �ִϸ��̼��� y���� �����ϰ� ����ϴ� �����̴� (Ȥ�� �ִϸ��̼��� y�� �̵��� ������ �ʿ��ϴٸ� ���� ������ �ʿ���
void CPlayer::Synchronize_Root(const _float& fTimeDelta)
{
	_vector vFF = XMVector3TransformNormal(XMVectorSetZ(XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove()), 0), m_pTransformCom->Get_WorldMatrix());

	// ���� ���
	_matrix worldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float4 vQuaternion = *m_pModelCom->Get_AnimationCenterRotation();

	_vector scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, worldMatrix);

	_vector resultQuaternionVector = XMQuaternionMultiply(XMLoadFloat4(&vQuaternion), rotation);

	// m_pModelCom->Get_AnimChanged()  ���������� ��������
	// m_pModelCom->Get_AnimLerp() ���������� �ʿ��� �ִϸ��̼�����
	if (m_pModelCom->Get_AnimChanged() || !m_pModelCom->Get_AnimLerp())
	{
		if (m_pModelCom->Get_AnimRestart())
		{
			XMStoreFloat4(&m_vPrevMove, XMVectorZero());
			m_pTransformCom->Go_Straight_CustumSpeed(m_fPrevSpeed, 1);
		}
		else
		{
			// ���ʹϾ� ȸ���� ������ �ߴ� (���� ���� ���࿹��)
			_float4 v;
			_vector diffQuaternionVector = XMQuaternionMultiply(resultQuaternionVector, XMQuaternionConjugate(XMLoadFloat4(&m_vPrevRotation)));
			XMStoreFloat4(&v, diffQuaternionVector);
			m_pTransformCom->Change_Rotation_Quaternion(v);

			//_float4 vb;
			//XMStoreFloat4(&vb, vFF - XMLoadFloat4(&m_vPrevMove));
			//m_pTransformCom->Go_Straight_CustumDir(vb, fTimeDelta);
			_float4 fMoveDir;
			_float fMoveSpeed = XMVectorGetX(XMVector3Length(vFF - XMLoadFloat4(&m_vPrevMove)));
			
			//Y�� �̵��� ���� �������� �����ؾ��Ѵ�.
			XMStoreFloat4(&fMoveDir, XMVectorSetY(XMVector3Normalize(vFF - XMLoadFloat4(&m_vPrevMove)), 0.f));

			if (0.01 > m_fPrevSpeed)
				m_fPrevSpeed = 0.f;

			m_pTransformCom->Go_Straight_CustumSpeed(m_fPrevSpeed, 1);
			m_pTransformCom->Go_Move_Custum(fMoveDir, m_fPrevSpeed, 1);
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
#ifdef _DEBUG
	if (m_pSystemManager->Get_Camera() == CAMERA_DEBUG)
		return;
#endif // _DEBUG

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

		if(m_iCurrentBehavior == (_uint)ADVENTURE_BEHAVIOR_STATE::WALK || m_iCurrentBehavior ==(_uint)ADVENTURE_BEHAVIOR_STATE::RUN)
			m_pTransformCom->LookAt_For_LandObject(vLookPos);

		isMove = true;
	}

	if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
	{
		_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamLook());
		m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;

		if (m_iCurrentBehavior == (_uint)ADVENTURE_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)ADVENTURE_BEHAVIOR_STATE::RUN)
			m_pTransformCom->LookAt_For_LandObject(vLookPos);

		isMove = true;
	}

	if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
	{
		_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamRight());
		m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
		
		if (m_iCurrentBehavior == (_uint)ADVENTURE_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)ADVENTURE_BEHAVIOR_STATE::RUN)
			m_pTransformCom->LookAt_For_LandObject(vLookPos);

		isMove = true;
	}

	if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
	{
		_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamRight());
		m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
		
		if (m_iCurrentBehavior == (_uint)ADVENTURE_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)ADVENTURE_BEHAVIOR_STATE::RUN)
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
		// ���� �ൿ�� �ʱ�ȭ�ϰ� �������� �ٲ��ش�.
		if(m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::ATTACK)
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

		m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::ATTACK;
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
	}
	if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
	{
		// ���� ���û��������� �����ؼ� ������ �׼��� �����Ű�ų�
		// �׿� �´� Ŀ�ǵ� �׼��� ����ä��Ѿ� �Ѵ�.

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
			m_pGameInstance->Get_CamLook();
			m_iCurrentBehavior = isShift ? (_uint)KRS_BEHAVIOR_STATE::WALK : (_uint)KRS_BEHAVIOR_STATE::RUN;

			m_InputDirection[F] = true;
			Compute_MoveDirection_FB();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}

		if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
		//_bool isStop = { false };
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamLook());
			m_iCurrentBehavior = isShift ? (_uint)KRS_BEHAVIOR_STATE::WALK : (_uint)KRS_BEHAVIOR_STATE::RUN;
		//	isShift = true;
			m_InputDirection[B] = true;
			Compute_MoveDirection_FB();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
		//	m_pTransformCom->Go_Straight(fTimeDelta);
		if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamRight());
			m_iCurrentBehavior = isShift ? (_uint)KRS_BEHAVIOR_STATE::WALK : (_uint)KRS_BEHAVIOR_STATE::RUN;
		//	m_MoveDirection[F] = false;
			m_InputDirection[L] = true;
			Compute_MoveDirection_RL();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
		//	m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
		if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
		//	m_MoveDirection[B] = false;
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamRight());
			m_iCurrentBehavior = isShift ? (_uint)KRS_BEHAVIOR_STATE::WALK : (_uint)KRS_BEHAVIOR_STATE::RUN;
		//	m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
			m_InputDirection[R] = true;
			Compute_MoveDirection_RL();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
		//	m_MoveDirection[L] = false;
		//}
		//	m_MoveDirection[L] = false;
		//}

		if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
		{
			// ������ ���� Ű �Է��� ���� ������ ������ Back�������� �̵��ؾ��ϱ� ������ Ű�Է¿��� üũ�ؼ� ���� �ʱ�ȭ
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

HRESULT CPlayer::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Model_Kiryu"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.3, 0.8, 0.3);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

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
	// ������ ��Ÿ���� ù��° �׼��� �����Ų�� (��Ʋ������ ������ ù��°�� ��Ʋ ���� �׼��� �� ����)
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
	
	// ���⺤�Ͱ� ������ ������� ����� 90���̸�(��������), ������ 90�� �ʰ�(�ݴ����), 0�̸� �����Ѵ�.
	
	// ���� �Է��� Ű�� ���̸� ������ �������Ұǵ�
	if (m_InputDirection[B])
	{
		// �� �� ĳ���Ͱ� ī�޶�� ���� ������ �����ִٸ� Front ������
		if (XMVectorGetX(XMVector3Dot(vCamLook, vPlayerLook)) < 0)
			m_MoveDirection[F] = true;
		// ĳ���Ͱ� ī�޶�� �ݴ������ ���� �ִٸ� Back ������
		else
			m_MoveDirection[B] = true;
	}
	// ���� �Է��� Ű�� �ڸ� �ڷ� �������Ұǵ�, 
	if (m_InputDirection[F])
	{
		//ĳ���Ͱ� ī�޶�� ���� ������ �����ִٸ� Back���� ������
		if (XMVectorGetX(XMVector3Dot(vCamLook, vPlayerLook)) < 0)
			m_MoveDirection[B] = true;
		// ĳ���Ͱ� ī�޶�� �ݴ������ �����ִٸ� Front ������
		else
			m_MoveDirection[F] = true;
	}


}

void CPlayer::Compute_MoveDirection_RL()
{
	_vector vCamRight = m_pGameInstance->Get_CamRight();
	_vector vPlayerRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	// ���� �Է��� Ű�� ������
	if (m_InputDirection[L])
	{
		// ���������� �������Ұǵ�, ĳ������ �������� ī�޶��� �����ʰ� ���� �����̶�� �׳� ������ �����̸��
		if (XMVectorGetX(XMVector3Dot(vCamRight, vPlayerRight)) < 0)
			m_MoveDirection[R] = true;
		// ĳ������ �������� ī�޶��� �����ʰ� �ݴ� �����̶�� ���� �����̸��
		else if (XMVectorGetX(XMVector3Dot(vCamRight, vPlayerRight)) > 0)
			m_MoveDirection[L] = true;

	}
	// ���� �Է��� Ű�� �ڸ� �ڷ� �������Ұǵ�, 
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

	Safe_Release(m_pShaderCom);
}