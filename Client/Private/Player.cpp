#include "Player.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Collision_Manager.h"

#include "CharacterData.h"
#include "SocketCollider.h"
#include "SocketEffect.h"

#include "BehaviorAnimation.h"
#include "Mesh.h"

#include "UIManager.h"
#include "RimChecker.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CLandObject{ rhs },
	m_pUIManager{CUIManager::GetInstance()}
{
	Safe_AddRef(m_pUIManager);
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

	CRimChecker::RIM_CHECKER_DESC RimDesc{};
	RimDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	RimDesc.pNeckMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("kubi_c_n");	
	RimDesc.pLHandMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_l_n");
	RimDesc.pRHandMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_r_n");
	RimDesc.pLFootMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("asi4_l_n");
	RimDesc.pRFootMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("asi4_r_n");

	m_RimChecker = dynamic_cast<CRimChecker*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_RimChecker"), &RimDesc));

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

	if (m_pGameInstance->GetKeyState(DIK_LEFT) == TAP)
	{
		Style_Change(KRH);
	}

	if (m_pGameInstance->GetKeyState(DIK_RIGHT) == TAP)
	{
		Style_Change(KRC);
	}

	if (m_pGameInstance->GetKeyState(DIK_DOWN) == TAP)
	{
		Style_Change(ADVENTURE);
	}
	//�޴� â �߰�
	if (m_pGameInstance->GetKeyState(DIK_I) == TAP)
	{
		m_pUIManager->Open_Scene(TEXT("Menu"));
	}
	if (m_pGameInstance->GetKeyState(DIK_Q) == TAP)
	{
		m_pUIManager->Close_Scene();
	}
	if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
	{
		m_pUIManager->Click();
	}

	Synchronize_Root(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

#ifdef _DEBUG
	if (m_isAnimStart)
		m_pModelCom->Play_Animation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));
#endif // _DEBUG

	for (auto& pCollider : m_pColliders)
		pCollider.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pEffects)
		pEffect.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));


	KeyInput(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	Animation_Event();

	Effect_Control_Aura();

	m_RimChecker->Tick(fTimeDelta);
}

void CPlayer::Late_Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	if (m_isObjectRender)
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow�� ���� �߰�
	}
#else
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow�� ���� �߰�
#endif // _DEBUG


	
	
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
	m_RimChecker->Late_Tick(fTimeDelta);
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		//if (!strcmp("[l0]jacketw1", pMesh->Get_Name()))
		//{
		//	if(m_isRimLight)
		//		if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_bool))))
		//			return E_FAIL;
		//}
		//else if (!strcmp("[l0]body_naked1", pMesh->Get_Name()))
		//{
		//	if (m_isRimLight)
		//		if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_bool))))
		//			return E_FAIL;
		//}
		//else
		//{
		//	_bool isfalse = false;
		//	if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &isfalse, sizeof(_bool))))
		//		return E_FAIL;
		//}

		if(m_isRimLight)
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_bool))))
					return E_FAIL;

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

HRESULT CPlayer::Render_LightDepth()
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

void CPlayer::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
	// iHitColliderType �� �浹�� HITŸ�� �ݶ��̴��� ���, �ٵ�, ���������� �����ִ�.

	// ���� ����� ���� �ִϸ��̼� ������ �����´�.
	switch (m_eCurrentStyle)
	{
	case CPlayer::KRS:
	{
		/*CKiryu_KRS_Hit::KRS_Hit_DESC Desc{ &vDir, fDamage, pAttackedObject->Get_CurrentAnimationName()};

		m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::HIT;
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value((void*) &Desc);*/
		break;
	}
	case CPlayer::KRH:
	{

		break;
	}
	case CPlayer::KRC:
	{

		break;
	}
	}
	pAttackedObject->Get_CurrentAnimationName();
}

string CPlayer::Get_CurrentAnimationName()
{
	return m_pModelCom->Get_AnimationName(m_pModelCom->Get_CurrentAnimationIndex());
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

	for (size_t i = 0; i < (_uint)KRH_BEHAVIOR_STATE::KRH_BEHAVIOR_END; i++)
	{
		m_AnimationTree[KRH].emplace(i, CBehaviorAnimation::Create(KRH, i, this));
	}

	for (size_t i = 0; i < (_uint)KRC_BEHAVIOR_STATE::KRC_BEHAVIOR_END; i++)
	{
		m_AnimationTree[KRC].emplace(i, CBehaviorAnimation::Create(KRC, i, this));
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
			//_float4 v;
			//_vector diffQuaternionVector = XMQuaternionMultiply(resultQuaternionVector, XMQuaternionConjugate(XMLoadFloat4(&m_vPrevRotation)));
			//XMStoreFloat4(&v, diffQuaternionVector);
			//m_pTransformCom->Change_Rotation_Quaternion(v);

			//_float4 vb;
			//XMStoreFloat4(&vb, vFF - XMLoadFloat4(&m_vPrevMove));
			//m_pTransformCom->Go_Straight_CustumDir(vb, fTimeDelta);

			_float4 fMoveDir;
			_float fMoveSpeed = XMVectorGetX(XMVector3Length(vFF - XMLoadFloat4(&m_vPrevMove)));
			
			//Y�� �̵��� ���� �������� �����ؾ��Ѵ�.
			_vector vTemp = XMVector3NormalizeEst((vFF - XMLoadFloat4(&m_vPrevMove)));
			vTemp = XMVectorSetZ(vTemp, XMVectorGetY(vTemp));
			XMStoreFloat4(&fMoveDir, XMVector3TransformNormal(XMVectorSetY(vTemp, 0.f), m_pTransformCom->Get_WorldMatrix()));

			if (0.01 > m_fPrevSpeed)
				m_fPrevSpeed = 0.f;

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
		_vector vCamLook = m_pGameInstance->Get_CamLook();
		_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK) + vCamLook);

		m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;

		vLookPos = XMVectorSetY(vLookPos, XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

		if (!XMVector3Equal(vLookPos, XMVectorZero())) {
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
	}

	if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
	{
		_vector vCamLook = m_pGameInstance->Get_CamLook();
		_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK) - vCamLook);

		m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;

		vLookPos = XMVectorSetY(vLookPos, XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

		if (!XMVector3Equal(vLookPos, XMVectorZero())) {
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
	}

	if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
	{
		_vector vCamLook = m_pGameInstance->Get_CamLook();

		_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize((m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamRight()));

		m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
		
		vLookPos = XMVectorSetY(vLookPos, XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		_float a = XMVectorGetX(vLookPos);
		if (isnan(a))
			int h = 99;

		if (m_iCurrentBehavior == (_uint)ADVENTURE_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)ADVENTURE_BEHAVIOR_STATE::RUN)
			m_pTransformCom->LookAt_For_LandObject(vLookPos);

		isMove = true;
	}

	if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
	{
		_vector vCamLook = m_pGameInstance->Get_CamLook();

		_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamRight());

		m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
		
		vLookPos = XMVectorSetY(vLookPos, XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		_float a = XMVectorGetX(vLookPos);
		if (isnan(a))
			int h = 99;

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

	if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::SKILL_FLY_KICK) return;

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
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
	}
	if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
	{
		// ���� ���û��������� �����ؼ� ������ �׼��� �����Ű�ų�
		// �׿� �´� Ŀ�ǵ� �׼��� ����ä��Ѿ� �Ѵ�.

		// ���⿡ ��ųƮ���� �Ϸ�Ǹ� ��ų�� ������������ ���� ���ǽ��� �߰��� ��ƾ��Ѵ�
		if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN)
		{
			m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::SKILL_FLY_KICK;
		}
		// �⺻ �����޺� �������� ���� ��Ŭ���� ������ �ǴϽ� ��� ����
		else if(m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::ATTACK)
		{
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count(true);
		}
		// �ƹ��͵� �ƴ� ���¿��� ��Ŭ���� ���´ٸ� ű�޺��� ����
		else
		{
			m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::SKILL_KICK_COMBO;
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
		}

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
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamLook());
			m_iCurrentBehavior = isShift ? (_uint)KRS_BEHAVIOR_STATE::WALK : (_uint)KRS_BEHAVIOR_STATE::RUN;
			m_InputDirection[B] = true;
			Compute_MoveDirection_FB();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
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
			isMove = true;
		}

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
	if (m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationEnd())
	{
		if ((_uint)KRH_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
		m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::IDLE;
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
		if (m_iCurrentBehavior != (_uint)KRH_BEHAVIOR_STATE::ATTACK)
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

		m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::ATTACK;
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
	}
	if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
	{
		if (m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::ATTACK)
		{
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count(true);
		}
	}

	if (m_iCurrentBehavior < (_uint)KRH_BEHAVIOR_STATE::ATTACK)
	{
		if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamLook());
			m_pGameInstance->Get_CamLook();
			m_iCurrentBehavior = isShift ? (_uint)KRH_BEHAVIOR_STATE::WALK : (_uint)KRH_BEHAVIOR_STATE::RUN;

			m_InputDirection[F] = true;
			Compute_MoveDirection_FB();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}

		if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamLook());
			m_iCurrentBehavior = isShift ? (_uint)KRH_BEHAVIOR_STATE::WALK : (_uint)KRH_BEHAVIOR_STATE::RUN;
			m_InputDirection[B] = true;
			Compute_MoveDirection_FB();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
		if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamRight());
			m_iCurrentBehavior = isShift ? (_uint)KRH_BEHAVIOR_STATE::WALK : (_uint)KRH_BEHAVIOR_STATE::RUN;
			m_InputDirection[L] = true;
			Compute_MoveDirection_RL();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
		if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamRight());
			m_iCurrentBehavior = isShift ? (_uint)KRH_BEHAVIOR_STATE::WALK : (_uint)KRH_BEHAVIOR_STATE::RUN;
			m_InputDirection[R] = true;
			Compute_MoveDirection_RL();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
	}

	if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
	{
		// ������ ���� Ű �Է��� ���� ������ ������ Back�������� �̵��ؾ��ϱ� ������ Ű�Է¿��� üũ�ؼ� ���� �ʱ�ȭ
		if (!isMove)
			Reset_MoveDirection();

		m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::SWAY;
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
	}

	if (!isMove && m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::RUN || m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::WALK)
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stop();
}

void CPlayer::KRC_KeyInput(const _float& fTimeDelta)
{
	if (m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationEnd())
	{
		if ((_uint)KRC_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
		m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::IDLE;
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
		if (m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::ATTACK)
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

		m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::ATTACK;
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
	}
	if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
	{
		// ���� ���û��������� �����ؼ� ������ �׼��� �����Ű�ų�
		// �׿� �´� Ŀ�ǵ� �׼��� ����ä��Ѿ� �Ѵ�.

		if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::ATTACK)
		{
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count(true);
		}
	}

	if (m_iCurrentBehavior < (_uint)KRC_BEHAVIOR_STATE::ATTACK)
	{
		if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamLook());
			m_pGameInstance->Get_CamLook();
			m_iCurrentBehavior = isShift ? (_uint)KRC_BEHAVIOR_STATE::WALK : (_uint)KRC_BEHAVIOR_STATE::RUN;

			m_InputDirection[F] = true;
			Compute_MoveDirection_FB();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}

		if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamLook());
			m_iCurrentBehavior = isShift ? (_uint)KRC_BEHAVIOR_STATE::WALK : (_uint)KRC_BEHAVIOR_STATE::RUN;
			m_InputDirection[B] = true;
			Compute_MoveDirection_FB();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
		if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamRight());
			m_iCurrentBehavior = isShift ? (_uint)KRC_BEHAVIOR_STATE::WALK : (_uint)KRC_BEHAVIOR_STATE::RUN;
			m_InputDirection[L] = true;
			Compute_MoveDirection_RL();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
		if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
		{
			if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::RUN)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamRight());
			m_iCurrentBehavior = isShift ? (_uint)KRC_BEHAVIOR_STATE::WALK : (_uint)KRC_BEHAVIOR_STATE::RUN;
			m_InputDirection[R] = true;
			Compute_MoveDirection_RL();
			m_pTransformCom->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}

		if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
		{
			// ������ ���� Ű �Է��� ���� ������ ������ Back�������� �̵��ؾ��ϱ� ������ Ű�Է¿��� üũ�ؼ� ���� �ʱ�ȭ
			if (!isMove)
				Reset_MoveDirection();

			m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::SWAY;
		}
	}


	if (!isMove && m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::RUN || m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::WALK)
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stop();
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
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
	m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
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

void CPlayer::Effect_Control_Aura()
{
	CSocketEffect* pEffect = { nullptr };

	for (auto& pair : m_pEffects)
	{
		string strKey = pair.first;

		if (string::npos != strKey.find("Aura"))
			pEffect = pair.second;
	}

	if (0 < m_iCurrentHitLevel)
	{
		if(nullptr != pEffect)
			pEffect->On();
	}
	else
	{
		if (nullptr != pEffect)
			pEffect->Off();
	}
}

void CPlayer::AccHitGauge()
{
	if (PLAYER_HITGAUGE_LEVEL_INTERVAL * 3.f < m_fHitGauge)
		m_fHitGauge = PLAYER_HITGAUGE_LEVEL_INTERVAL * 3.f;
	else
		m_fHitGauge += 10.f;

	m_iCurrentHitLevel = (m_fHitGauge / PLAYER_HITGAUGE_LEVEL_INTERVAL);
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
	Safe_Release(m_pUIManager);
	Safe_Release(m_pShaderCom);
}