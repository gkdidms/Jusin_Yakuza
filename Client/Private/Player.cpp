#include "Player.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Collision_Manager.h"

#ifdef _DEBUG
#include "DebugManager.h"
#endif // _DEBUG

#include "CharacterData.h"
#include "SocketCollider.h"
#include "SocketEffect.h"
#include "Effect.h"

#include "BehaviorAnimation.h"
#include "Mesh.h"
#include "Animation.h"

#include "UIManager.h"
#include "Camera.h"
#include "PlayerCamera.h"
#include "CutSceneCamera.h"

#include "Monster.h"
#include "Item.h"
#include "MapCollider.h"
#include "EffectManager.h"
#include "QteManager.h"

#include "MonsterGroup.h"

#pragma region �ൿ ���� �����
#include "Kiryu_KRS_Attack.h"
#include "Kiryu_KRH_Attack.h"
#include "Kiryu_KRC_Attack.h"
#include "Kiryu_KRC_Hit.h"
#include "Kiryu_KRH_Hit.h"
#include "Kiryu_KRS_Hit.h"
#include "Kiryu_KRS_Down.h"
#include "Kiryu_KRH_Down.h"
#include "Kiryu_KRS_Grab.h"
#include "Kiryu_KRC_Grab.h"
#include "Kiryu_KRS_PickUp.h"
#include "Kiryu_KRC_PickUp.h"
#pragma endregion

CPlayer::PLAYER_INFO CPlayer::PlayerInfo{ };

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext },
#ifdef _DEBUG
	m_pDebugManager{ CDebugManager::GetInstance() },
#endif // _DEBUG
	m_pUIManager{ CUIManager::GetInstance() }
{
#ifdef _DEBUG
	Safe_AddRef(m_pDebugManager);
#endif // _DEBUG
	Safe_AddRef(m_pUIManager);

	m_isMonster = false;
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CLandObject{ rhs },
#ifdef _DEBUG
	m_pDebugManager{ rhs.m_pDebugManager },
#endif // _DEBUG
	m_pUIManager{ rhs.m_pUIManager }
{
#ifdef _DEBUG
	Safe_AddRef(m_pDebugManager);
#endif // _DEBUG
	Safe_AddRef(m_pUIManager);

	m_isMonster = false;
}

void CPlayer::Set_SeizeOff(_bool isOff)
{
	// ������ ��� ����� ����
	if (KRH == m_eCurrentStyle) return;

	//8�� Grab ����
	m_AnimationTree[m_eCurrentStyle].at(8)->Event(&isOff);
}

void CPlayer::Set_ItemOff()
{
	m_pTargetItem = nullptr;
}

void CPlayer::Battle_Start()
{
	m_eCurrentStyle = KRS;
}

void CPlayer::Play_Kuze_QTE(CMonster* pMonster)
{
	m_pQTEMgr->Set_Kuze(pMonster);
	m_pQTEMgr->Set_Animation(m_pAnimCom, "h23250");

	Set_CutSceneAnim(KUZE_RUSH);
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	m_wstrModelName = TEXT("Kiryu");

	m_isMonster = false;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Ready_AnimationTree();
	Ready_CutSceneAnimation();

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	Ready_AuraEffect();

	Ready_RootFalseAnimation();

	// �⺻ ����: 20
	// ������: 30
	// ����: 300
	m_Info.fMaxHP = 150.f;
	m_Info.fHp = m_Info.fMaxHP;

	PlayerInfo.iMoney = 100;
	PlayerInfo.fHitGauge = 100.f;
	ZeroMemory(&m_MoveDirection, sizeof(_bool) * MOVE_DIRECTION_END);
	ZeroMemory(&m_InputDirection, sizeof(_bool) * MOVE_DIRECTION_END);

	// ���� ������ �� ���� UI�Ŵ����� ������ Set���ش�.
	//m_pUIManager->Set_Player(this);

	m_pQTEMgr = dynamic_cast<CQteManager*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_QTEManager"), nullptr));

	On_Separation_Hand(0);			// ��� �и� �ѵ�

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

	Update_RootFalseAnim();			//Play_Animation ������ �����ؾ��Ѵ�.

#ifdef _DEBUG

	if (m_pGameInstance->GetKeyState(DIK_Z) == TAP)
	{
		//TODO: ���⿡�� enum���� �ʿ��� �ִϸ��̼����� �ٲٸ� �ش��ϴ� �ƽ��� ����ȴ�
		if (m_pTargetObject)
		{
			Set_CutSceneAnim(m_eCutSceneType);
		}
	}

	if (m_isAnimStart)
	{
		if (DEFAULT == m_eAnimComType)
		{
			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iHandAnimIndex, m_SeparationAnimComs[HAND_ANIM], false, (_int)HAND_ANIM);
			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, (_int)FACE_ANIM);
			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iDefaultAnimIndex, m_SeparationAnimComs[DEFAULT_ANIM], false, (_int)DEFAULT_ANIM);
			m_pModelCom->Play_Animation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_isRootAnim);
			Play_CutScene();
		}
		else
		{
			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iHandAnimIndex, m_SeparationAnimComs[HAND_ANIM], false, (_int)HAND_ANIM);
			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, (_int)FACE_ANIM);
			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iDefaultAnimIndex, m_SeparationAnimComs[DEFAULT_ANIM], false, (_int)DEFAULT_ANIM);
			Play_CutScene();
		}
	}
#else
	if (DEFAULT == m_eAnimComType)
	{
		m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iHandAnimIndex, m_SeparationAnimComs[HAND_ANIM], false, (_int)HAND_ANIM);
		m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, (_int)FACE_ANIM);
		m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iDefaultAnimIndex, m_SeparationAnimComs[DEFAULT_ANIM], false, (_int)DEFAULT_ANIM);
		m_pModelCom->Play_Animation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_isRootAnim);
		Play_CutScene();
	}
	else
	{
		m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iHandAnimIndex, m_SeparationAnimComs[HAND_ANIM], false, (_int)HAND_ANIM);
		m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, (_int)FACE_ANIM);
		m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iDefaultAnimIndex, m_SeparationAnimComs[DEFAULT_ANIM], false, (_int)DEFAULT_ANIM);
		Play_CutScene();
	}
#endif // _DEBUG

	if(m_pModelCom->Get_CurrentAnimationIndex() != 579)
		Synchronize_Root(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	//��ȭ���� ��� �÷��̾�� �����̰ų� �������� �ʴ´�.
	if (m_isStop || m_pUIManager->isOpen(TEXT("Inven")))
		return;

	// ��Ʋ ���� �ִϸ��̼� �ƴ� ��� Ÿ�ӵ�Ÿ�� 1�� ���������ش�.
	if (!m_isHitFreeze && m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::BTL_START)
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);

	if (m_isHitFreeze)
		HitFreeze_Timer(fTimeDelta);
	if (m_isHitRadial)
		HitRadial_Timer(fTimeDelta);
	if (m_isHitZoom)
		HitZoomIn_Timer(fTimeDelta);

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

		ShowCursor(true);
	}
	if (m_pGameInstance->GetKeyState(DIK_Q) == TAP)
	{
		if (m_pGameInstance->Get_CurrentLevel() != LEVEL_TUTORIAL)
		{
			m_pGameInstance->StopSound(SOUND_UI);
			m_pGameInstance->PlaySound_W(TEXT("4a72 [10].wav"), SOUND_UI, 0.7f);

			m_pUIManager->Close_Scene();

			if (m_pUIManager->isInvenClose())
			{
				SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f); // ���콺 ��ǥ �������ֱ�
				ShowCursor(false);
			}
		}
	}
	if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
	{
		m_pUIManager->Click();
	}
	if (m_pGameInstance->GetKeyState(DIK_T) == TAP)
	{
		//m_pUIManager->Open_Scene(TEXT("Tutorial"));
	}
	if (m_pGameInstance->GetKeyState(DIK_Y) == TAP)
	{
		m_pUIManager->Open_Scene(TEXT("Carchase"));
	}
	if (m_pGameInstance->GetKeyState(DIK_0) == TAP)
	{
		m_pUIManager->Open_Scene(TEXT("FightScore"));
	}
	if (m_pGameInstance->GetKeyState(DIK_9) == TAP)
	{
		m_pUIManager->Open_Scene(TEXT("Title"));
	}
	if (m_pGameInstance->GetKeyState(DIK_7) == TAP)
	{
		m_pUIManager->Open_Scene(TEXT("QTE"));
	}
	if (m_pGameInstance->GetKeyState(DIK_5) == TAP)
	{
		m_pUIManager->Fade_Out();
	}
	if (m_pGameInstance->GetKeyState(DIK_6) == TAP)
	{
		m_pUIManager->Fade_In();
	}
	

	for (auto& pCollider : m_pColliders)
		pCollider.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pEffects)
		pEffect.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pTrailEffects)
		pEffect.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	// ��Ʈ�׼��� ������ �������� �����Ѵ�.
	if (2 < CPlayer::PlayerInfo.iCurrentHitLevel)
	{
		m_CanHitAction = true;
	}

	
	if (m_eAnimComType == DEFAULT)
	{
		// TODO: Ʃ�丮�� UI ������ ���� �Ѿ���
		//if(!m_pUIManager->IsOpend())
		KeyInput(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));
	}


	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	Animation_Event();
	RimLight_Event();
	Trail_Event();
	Radial_Event();
	Face_Event();
	Sound_Event();
	Effect_Control_Aura();
	Setting_Target_Enemy();
	Setting_Target_Item();
	//Setting_Target_Wall();

	if(nullptr != m_pQTEMgr)
		m_pQTEMgr->Tick(fTimeDelta);
}

void CPlayer::Late_Tick(const _float& fTimeDelta)
{
	m_pCollisionManager->Add_ImpulseResolution(this);

#ifdef _DEBUG
	if (m_isObjectRender)
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
		//m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow�� ���� �߰�
	}
#else
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	//m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow�� ���� �߰�
#endif // _DEBUG

	for (auto& pCollider : m_pColliders)
		pCollider.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pEffects)
		pEffect.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pTrailEffects)
		pEffect.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	// ���� �����ִ� Attack�� �ݶ��̴� ����
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::ATTACK && pPair.second->IsOn())
		{
			if (pPair.second->Get_CollierPartType() == 1)
				int a = 0;

			m_pCollisionManager->Add_AttackCollider(pPair.second, CCollision_Manager::PLAYER);
		}
	}

	// ���� �����ִ� Hit�� �ݶ��̴� ���� (���������� Hit�� �ݶ��̴��� �׻� ��������)
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::HIT && pPair.second->IsOn())
			m_pCollisionManager->Add_HitCollider(pPair.second, CCollision_Manager::PLAYER);
	}
	
	Setting_RimLight();
	Compute_Height();

	if (m_pModelCom->Get_AnimFinished())
	{
		// �ִϸ��̼Ť����� ������ �� ���� �ִϸ��̼ǿ��� ȸ���� �� ��ŭ ȸ�������ش�.

		//_float3 vScaled = m_pTransformCom->Get_Scaled();
		//_vector vScale = XMLoadFloat3(&vScaled);
		//_vector vOriginRot = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		//_vector vRot = XMLoadFloat4(&m_vPrevRotation);
		//_vector vTranslation = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		//_matrix CenterRotationMatrix = XMMatrixTranspose(XMMatrixAffineTransformation(vScale, vOriginRot, vRot, vTranslation));

		//// �����ϰ� �ִ� ���ʹϾ��� ȸ������ Y�� ���� ������ Forward�� ������� ȸ�����̹Ƿ�,
		//// Z�� ���� ������ forward�� ������ ��ȯ���־�� �Ѵ�.
		////_matrix yFlipMatrix = XMMatrixRotationX(XMConvertToRadians(-90.0f));

		////// ���ʹϾ� ȸ���� Y�� ȸ���� ������ ���ο� ȸ�� ��� ����
		////_matrix newRotationMatrix = XMMatrixMultiply(yFlipMatrix, CenterRotationMatrix);

		//// ������ ���� ��Ŀ� ���ο� ȸ�� ����� ����
		//_matrix WorldMatrix = XMMatrixMultiply(CenterRotationMatrix, m_pTransformCom->Get_WorldMatrix());

		//XMVECTOR y90DegreeRotation = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(90.0f));
		//XMVECTOR transformedQuat = XMQuaternionMultiply(y90DegreeRotation, XMLoadFloat4(&m_vPrevRotation));

		//XMFLOAT3 euler;
		//XMFLOAT4 q;
		//XMStoreFloat4(&q, transformedQuat);

		//// ���Ϸ� ���� ��� (Yaw, Pitch, Roll)
		//euler.y = atan2(2.0f * (q.x * q.w + q.y * q.z), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)); // Yaw
		//euler.x = asin(2.0f * (q.x * q.z - q.w * q.y)); // Pitch
		//euler.z = atan2(2.0f * (q.x * q.y + q.z * q.w), 1.0f - 2.0f * (q.x * q.x + q.z * q.z)); // Roll

		//_matrix RotMatrixX = XMMatrixRotationY(euler.x);
		//_matrix RotMatrixZ = XMMatrixRotationY(euler.z);

		//_matrix WorldMatrix = XMMatrixMultiply(RotMatrixZ, m_pTransformCom->Get_WorldMatrix());
		////_matrix WorldMatrix = XMMatrixMultiply(XMMatrixRotationRollPitchYaw(euler.x, euler.y, euler.z), m_pTransformCom->Get_WorldMatrix());



		//m_pTransformCom->Set_WorldMatrix(WorldMatrix);
		//_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + WorldMatrix.r[CTransform::STATE_LOOK];

		//m_pTransformCom->LookAt_For_LandObject(vLookPos);
	}
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		if (!m_pGameInstance->isShadow())
		{
			m_pModelCom->Bind_BoneMatrices(i);
			m_pModelCom->Bind_Compute(m_pComputeShaderCom, i);
		}

		if(ADVENTURE != m_isRimLight)
		{
			// ������ �� ���Ƿ� Full�� �������ְ� ����Ѵ�.
			if (string_view(m_strRimMeshName) == string_view("Full"))
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimUV", &m_fRimPartsUV, sizeof(_float2))))
					return E_FAIL;
			}
			else if (string_view(m_strRimMeshName) == string_view(pMesh->Get_Name()))
			{
				_float2 fUV = m_fRimPartsUV;		// �⺻������ ����uv�� �ְ�
				if (string_view("[l0]jacketw1") == string_view(m_strRimMeshName))
				{
					fUV = m_fRimTopUV;				// ��ü�� �� ž�� �־��ش�.

					if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimArmUV", &m_fRimArmUV, sizeof(_float2))))
						return E_FAIL;

					_bool isTop = true;
					if (FAILED(m_pShaderCom->Bind_RawValue("g_isTop", &isTop, sizeof(_bool))))
						return E_FAIL;

				}
				if(string_view("[l0]pants3") == string_view(m_strRimMeshName))
					fUV = m_fRimBotUV;				// ������ �� ������ �־��ش�.

				if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimUV", &fUV, sizeof(_float2))))
					return E_FAIL;
			}
			else
			{
				_float isfalse = 0.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &isfalse, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimUV", &m_fRimPartsUV, sizeof(_float2))))
					return E_FAIL;
			}

			// ��������� �����ִ� ���¶�� ��ݽ� ������Ʈ�� �Ҵ�
			if (0 < CPlayer::PlayerInfo.iCurrentHitLevel)
			{
				if (!strcmp(pMesh->Get_Name(), "[l0]jacketw1"))
				{
					if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_float))))
						return E_FAIL;

					if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimUV", &m_fRimTopUV, sizeof(_float2))))
						return E_FAIL;

					if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimArmUV", &m_fRimArmUV, sizeof(_float2))))
						return E_FAIL;

					_bool isTop = true;
					if (FAILED(m_pShaderCom->Bind_RawValue("g_isTop", &isTop, sizeof(_bool))))
						return E_FAIL;
					
				}
			}

			if ((_uint)KRS_BEHAVIOR_STATE::SWAY == m_iCurrentBehavior)
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimUV", &m_fRimPartsUV, sizeof(_float2))))
					return E_FAIL;
			}
		}
		else
		{	
			// ��庥ó�϶�
			if (FAILED(Bind_RimLight()))
				return E_FAIL;
		}

		if (FAILED(m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(pMesh->Get_MaterialIndex()))))
			return E_FAIL;

		m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float));

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		_bool isRS = true;
		_bool isRD = true;
		_bool isRM = true;
		_bool isRT = true;
		_bool isMulti = true;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS)))
			isMulti = false;
		m_pShaderCom->Bind_RawValue("g_isMulti", &isMulti, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
			isRS = false;
		m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;
		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS)))
			isRM = false;
		m_pShaderCom->Bind_RawValue("g_isRM", &isRM, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RTTexture", i, aiTextureType_EMISSIVE)))
			isRT = false;
		m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

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

void CPlayer::Sound_Event()
{
	if (m_eAnimComType != CUTSCENE)
	{
		auto& pCurEvents = m_pData->Get_Current_SoundEvents();
		for (auto& pEvent : pCurEvents)
		{
			_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition());
			_double Duration = *(m_pModelCom->Get_AnimationDuration());

			if (!pEvent->isPlayed && CurPos >= pEvent->fAinmPosition && CurPos < Duration)
			{
				pEvent->isPlayed = true;
				wstring wstrSoundFile = m_pGameInstance->StringToWstring(pEvent->strSoundFileName);
				m_pGameInstance->StopAndPlaySound(wstrSoundFile, static_cast<CHANNELID>(pEvent->iChannel), pEvent->fSoundVolume);
			}
		}
	}
	else
	{
		auto& pCurEvents = m_pData->Get_Current_SoundEvents();
		for (auto& pEvent : pCurEvents)
		{
			_double CurPos = *(m_pCameraModel->Get_AnimationCurrentPosition());
			_double Duration = *(m_pCameraModel->Get_AnimationDuration());

			if (!pEvent->isPlayed && CurPos >= pEvent->fAinmPosition && CurPos < Duration)
			{
				pEvent->isPlayed = true;
				wstring wstrSoundFile = m_pGameInstance->StringToWstring(pEvent->strSoundFileName);
				m_pGameInstance->StopAndPlaySound(wstrSoundFile, static_cast<CHANNELID>(pEvent->iChannel), pEvent->fSoundVolume);
			}
		}
	}
}

// �� ���� �ݶ��̴��� �浹���� ��
void CPlayer::Attack_Event(CGameObject* pHitObject, _bool isItem)
{
	if (!isItem)
	{
		CLandObject* pLandObject = static_cast<CLandObject*>(pHitObject);

		switch (m_eCurrentStyle)
		{
		case CPlayer::KRS:
		{
			if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::GRAB)
			{
				CKiryu_KRS_Grab::KRS_Grab_DESC Desc{ true, Compute_Target_Direction(pLandObject) };
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value(&Desc);

				HitRadial_On();
			}

			if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::ATTACK)
			{
				if (nullptr == m_pTargetObject) return;

				_bool isTargetDead = m_pTargetObject->isObjectDead();

				if (isTargetDead)
					PlayerInfo.iMoney += 1000;

				if (static_cast<CKiryu_KRS_Attack*>(m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior))->IsFinishBlow())
				{
					HitFreeze_On();

					CEffect::EFFECT_DESC EffectDesc{};	

					EffectDesc.pWorldMatrix= pHitObject->Get_TransformCom()->Get_WorldFloat4x4();
					CEffectManager::GetInstance()->Money(EffectDesc);	
					CUIManager::GetInstance()->Add_MoneyEffect(pHitObject->Get_TransformCom()->Get_WorldMatrix(), 1000, 1);
				}
			}

			break;
		}
		case CPlayer::KRH:
		{
			if (m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::ATTACK)
			{
				// �ǴϽ� ��ο��, ������ �׾��ٸ� ��� �����.
				if (static_cast<CKiryu_KRH_Attack*>(m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior))->IsFinishBlow())
				{
					HitFreeze_On();

					CEffect::EFFECT_DESC EffectDesc{};

					EffectDesc.pWorldMatrix = pHitObject->Get_TransformCom()->Get_WorldFloat4x4();
					CEffectManager::GetInstance()->Money(EffectDesc);
					CUIManager::GetInstance()->Add_MoneyEffect(pHitObject->Get_TransformCom()->Get_WorldMatrix(), 4563, 2);
				}
			}

			break;
		}
		case CPlayer::KRC:
		{
			if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::GRAB)
			{
				CKiryu_KRC_Grab::KRC_Grab_DESC Desc{ true, Compute_Target_Direction(pLandObject) };
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value(&Desc);

				HitRadial_On();
			}

			if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::ATTACK)
			{
				if (nullptr == m_pTargetObject) return;

				if (m_pTargetObject->isObjectDead())
					HitRadial_On();
				else if (static_cast<CKiryu_KRC_Attack*>(m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior))->IsFinishBlow())
				{
					HitFreeze_On();
					CEffect::EFFECT_DESC EffectDesc{};

					EffectDesc.pWorldMatrix = pHitObject->Get_TransformCom()->Get_WorldFloat4x4();
					CEffectManager::GetInstance()->Money(EffectDesc);
					CUIManager::GetInstance()->Add_MoneyEffect(pHitObject->Get_TransformCom()->Get_WorldMatrix(), 6993, 3);
				}
			}

			break;
		}
		}
	}
	
}

void CPlayer::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
	// iHitColliderType �� �浹�� HITŸ�� �ݶ��̴��� ���, �ٵ�, ���������� �����ִ�.

	// ������ ������ ���� (�ı��� ���� �ÿ��� ���� ��Һ��� 0.2�踸ŭ�� �޴´� (==����80��))
	_float fDamageDownScale = 1.f;

	switch (m_eCurrentStyle)
	{
	case CPlayer::KRS:
	{
		_int iDirection = Compute_Target_Direction(pAttackedObject);

		if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::DOWN)
		{
			string strAnimationName = pAttackedObject->Get_CurrentAnimationName();

			CKiryu_KRS_Down::KRS_DOWN_DESC Desc{ -1, iDirection, strAnimationName };
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value((void*)&Desc);
		}
		else if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::PICK_UP)		// ���� ��� �ִ� ���¸� ���� ó���Ѵ�.
		{
			string strAnimationName = pAttackedObject->Get_CurrentAnimationName();

			// ��Ʈ ��ü���� �ִϸ��̼� �������ְ�, ���� �� �ִϸ��̼��� ��������Ѵ�.
			CKiryu_KRS_Hit::KRS_Hit_DESC HitDesc{ &vDir, fDamage, strAnimationName, iDirection };
			
			m_iDefaultAnimIndex = m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationIndex();			//������Ʈ���� �� �ִϸ��̼� (pickup������ �ε���)

			CKiryu_KRS_PickUp::KRS_PICK_UP_HIT_DESC Desc{ m_AnimationTree[KRS].at((_uint)KRS_BEHAVIOR_STATE::HIT)->Get_AnimationIndex() };
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value((void*)&Desc);

			// ����� �� ������ �ٽ� �ʱ�ȭ���ش�.
			m_AnimationTree[KRS].at((_uint)KRS_BEHAVIOR_STATE::HIT)->Reset();

		}
		else if(m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::SKILL_FLY_KICK)						// ���۾Ƹ� ������ �ൿ Ÿ�Ե��� ����ó�����־���Ѵ�.
		{
			CKiryu_KRS_Hit::KRS_Hit_DESC Desc{ &vDir, fDamage, pAttackedObject->Get_CurrentAnimationName(), iDirection };

			m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::HIT;
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value((void*)&Desc);
		}


		break;
	}
	case CPlayer::KRH:
	{
		_int iDirection = Compute_Target_Direction(pAttackedObject);

		if (m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::DOWN)
		{
			string strAnimationName = pAttackedObject->Get_CurrentAnimationName();

			CKiryu_KRH_Down::KRH_DOWN_DESC Desc{ -1, iDirection, strAnimationName };
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value((void*)&Desc);
		}
		else
		{
			CKiryu_KRH_Hit::KRH_Hit_DESC Desc{ &vDir, fDamage, pAttackedObject->Get_CurrentAnimationName(), iDirection };

			m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::HIT;
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value((void*)&Desc);
		}

		break;
	}
	case CPlayer::KRC:
	{
		
		_int iDirection = Compute_Target_Direction(pAttackedObject);

		CKiryu_KRC_Hit::KRC_Hit_DESC Desc{ &vDir, fDamage, pAttackedObject->Get_CurrentAnimationName(), iDirection };

		if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::GUARD)
		{
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value((void*)&Desc);
			fDamageDownScale = 0.2f;
		}
		else if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::PICK_UP)		// ���� ��� �ִ� ���¸� ���� ó���Ѵ�.
		{
			string strAnimationName = pAttackedObject->Get_CurrentAnimationName();

			// ��Ʈ ��ü���� �ִϸ��̼� �������ְ�, ���� �� �ִϸ��̼��� ��������Ѵ�.
			CKiryu_KRC_Hit::KRC_Hit_DESC HitDesc{ &vDir, fDamage, strAnimationName, iDirection };

			m_iDefaultAnimIndex = m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationIndex();			//������Ʈ���� �� �ִϸ��̼� (pickup������ �ε���)

			CKiryu_KRC_PickUp::KRC_PICK_UP_HIT_DESC Desc{ m_AnimationTree[KRC].at((_uint)KRC_BEHAVIOR_STATE::HIT)->Get_AnimationIndex() };
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value((void*)&Desc);

			// ����� �� ������ �ٽ� �ʱ�ȭ���ش�.
			m_AnimationTree[KRC].at((_uint)KRC_BEHAVIOR_STATE::HIT)->Reset();

		}
		else
		{
			m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::HIT;
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value((void*)&Desc);
		}
		
		break;
	}
	}

	//������ ó���ϱ�
	if (!m_isObjectDead)
	{
		m_Info.fHp -= (fDamage * fDamageDownScale);
		if (m_Info.fHp <= 0.f)
			m_isObjectDead = true;
	}
}

string CPlayer::Get_CurrentAnimationName()
{
	return ExtractString(m_pModelCom->Get_AnimationName(m_pModelCom->Get_CurrentAnimationIndex()));
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

void CPlayer::Ready_CutSceneAnimation()
{
	/*
		FINISHBLOW,                 //h23320 �ǴϽ� ����� ��
		GOUGEKI_C,                  // h20021 ��� �� ��Ʈ�׼� (�ָ����� �󱼶����� ��ġ��)
		HAIHEKI_KICK,               //h23070 �� ������ ����ġ�� ��ų

		KOBUSHIKUDAKI,              //h10111 �Ȳ�ġ�� ���ݸ��� ��ų
		HAIHEKI_PUNCH,              //h23060 �� ������ ������ ��ų
		OI_TRAMPLE_AO,              //h1500 �ٿ�� ��� �� ��� (�����ִ� ����), ����
		OI_KICKOVER_UTU_C,          //h1511 �ٿ�� ��� �� ���� (������ִ� ����), ����

		KIRYU_GSWING,               //h1010 �ٸ���� ������ ���ø� ����
		DORYU_MIN,                  //h11285 ������ ������ ���ø� ����
		LAPEL_OIUCHI_NECK,           //h1540 �� �ٴڿ� ������ (����)
		NAGE_OIUCHI_NECK,           //h1540 �� �ٴڿ� ������ (����)
		POLE_KNOCK_LAPEL,           //h2040 ��ó�� ����� �ִٸ� ��տ� �ڰ��ϰ� ����
		DORAMUKAN_88,               //h3261 ū ���� (����)�� ��� ���� �а� ����ħ
		MONZETSU,                   //h11250 ���ٰ� �������� �˲����� (�ڿ��� ��������� ���)

		WALL_KNOCK_NECK_C,          //h2011 ���� �Ӹ��ڰ��ϰ� ���� (�Ƹ� ��� ���� ��Ʈ�׼����� �� ��ó�� �����ִٸ�)
		KABE_AIRON,                 //h23000 ������ ��ġ�� ����
		OI_KICK,                    //h23010 �Ӹ�ä ��� �� �߷��� (���帰 ����)
		OI_UPPER,                   //h23020 �Ӹ�ä��� �� �ָ����� ħ (�����ִ� ����)

		YONEDA_H,                   //a60300 ��״� ����
		YONEDA_DOWN_ATTACK,         //a60330 ��״� ����
		YONEDA_DOSU,                //a60350 ��״� ȭ���
	*/
	/* ���Ѵ� */
	m_CutSceneAnimation.emplace(FINISHBLOW, "h23320");
	m_CutSceneAnimation.emplace(GOUGEKI_C, "h20021");
	m_CutSceneAnimation.emplace(HAIHEKI_KICK, "h23070");

	/* ���� */
	m_CutSceneAnimation.emplace(KOBUSHIKUDAKI, "h10111");
	m_CutSceneAnimation.emplace(HAIHEKI_PUNCH, "h23060");
	m_CutSceneAnimation.emplace(OI_TRAMPLE_AO, "h1500");
	m_CutSceneAnimation.emplace(OI_KICKOVER_UTU_C, "h1511");

	/* �ı��� */
	m_CutSceneAnimation.emplace(KIRYU_GSWING, "h1010");
	m_CutSceneAnimation.emplace(DORYU_MIN, "h11285");
	m_CutSceneAnimation.emplace(LAPEL_OIUCHI_NECK, "h1530");
	m_CutSceneAnimation.emplace(NAGE_OIUCHI_NECK, "h1540");
	m_CutSceneAnimation.emplace(POLE_KNOCK_LAPEL, "h2040");
	m_CutSceneAnimation.emplace(DORAMUKAN_88, "h3261");
	m_CutSceneAnimation.emplace(MONZETSU, "h11250");

	/* ���� */
	m_CutSceneAnimation.emplace(WALL_KNOCK_NECK_C, "h2011");
	m_CutSceneAnimation.emplace(KABE_AIRON, "h23000");
	m_CutSceneAnimation.emplace(OI_KICK, "h23010");
	m_CutSceneAnimation.emplace(OI_UPPER, "h23020");

	/* ��״� �ƽ� (QTE) */
	m_CutSceneAnimation.emplace(YONEDA_H, "a60300");
	m_CutSceneAnimation.emplace(YONEDA_DOWN_ATTACK, "a60330");
	m_CutSceneAnimation.emplace(YONEDA_DOSU, "a60350");

	/* ���� QTE */
	m_CutSceneAnimation.emplace(KUZE_RUSH, "h23250");
}

void CPlayer::Ready_AuraEffect()
{
	for (auto& pair : m_pEffects)
	{
		string strKey = m_pGameInstance->WstringToString(pair.second->Get_EffectName());

		string strHooligan = "Hooligan";
		string strRush = "Rush";
		string strDestroyer = "Destroyer";

		if (string::npos != strKey.find(strHooligan))
		{
			m_HooliganAura.push_back(pair.second);
		}
		if (string::npos != strKey.find(strRush))
		{
			m_RushAura.push_back(pair.second);
		}
		if (string::npos != strKey.find(strDestroyer))
		{
			m_DestroyerAura.push_back(pair.second);
		}
	}

}

void CPlayer::Ready_RootFalseAnimation()
{
	//��Ʈ�ִϸ��̼��� ���ܽ�ų �ִϸ��̼��� �ε����� �ۼ��Ѵ�.
	m_RootFalseAnims.push_back(682);		// [682]	[p_stand_nml]
	m_RootFalseAnims.push_back(681);		// [681]	[p_stand_idle_lookaround]

	//�ı���
	m_RootFalseAnims.push_back(318);		// [318]	p_krc_stand_btl_lp[p_krc_stand_btl_lp]
	m_RootFalseAnims.push_back(367);		// [367]	[p_krc_wpc_stand]

	//����
	m_RootFalseAnims.push_back(476);		// [476]	p_krh_style_st[p_krh_style_st]
	m_RootFalseAnims.push_back(474);		// [474]	p_krh_stand_btl_lp[p_krh_stand_btl_lp]
	m_RootFalseAnims.push_back(473);		// [473]	p_krh_stand_btl_en[p_krh_stand_btl_en]

	//���Ѵ�
	m_RootFalseAnims.push_back(567);		// [567]	p_krs_style_st[p_krs_style_st]
	m_RootFalseAnims.push_back(565);		// [565]	p_krs_stand_btl_lp[p_krs_stand_btl_lp]
	m_RootFalseAnims.push_back(564);		// [564]	p_krs_stand_btl_en[p_krs_stand_btl_en]
	m_RootFalseAnims.push_back(751);		// [751]	[p_wpc_stand]

	m_RootFalseAnims.push_back(578);		// [578] [p_kru_sync_lapel_lp]
	m_RootFalseAnims.push_back(579);		// [579] [p_kru_sync_lapel_nage]
}

// ���� �ִϸ��̼��� y���� �����ϰ� ����ϴ� �����̴� (Ȥ�� �ִϸ��̼��� y�� �̵��� ������ �ʿ��ϴٸ� ���� ������ �ʿ���
void CPlayer::Synchronize_Root(const _float& fTimeDelta)
{
	if (m_eAnimComType == CUTSCENE) return;

	_vector vCenterMove = XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove());
	_vector vDeleteZ = XMVectorSetZ(vCenterMove, 0);

	if (m_iCurrentBehavior != 2)
	{
		vCenterMove = XMVectorSetX(vDeleteZ, 0);
	}
	else
	{
		vCenterMove = vDeleteZ;
	}

	//_vector vFF = XMVector3TransformNormal(XMVectorSetZ(XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove()), 0), m_pTransformCom->Get_WorldMatrix());
	//_vector vFF = XMVector3TransformNormal(vCenterMove, m_pTransformCom->Get_WorldMatrix());
	_vector vFF = vCenterMove;

	//_matrix Mat = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix_AtIndex(0));

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
			_float4 fMoveDir;
			_float fMoveSpeed = XMVectorGetX(XMVector3Length(vFF - XMLoadFloat4(&m_vPrevMove)));

			//Y�� �̵��� ���� �������� �����ؾ��Ѵ�.
			_vector vTemp = XMVector3Normalize((vFF - XMLoadFloat4(&m_vPrevMove)));
			//Z�� Yó�� ����
			vTemp = XMVectorSetZ(vTemp, XMVectorGetY(vTemp));
			//vTemp = XMVectorSetX(vTemp, XMVectorGetX(vTemp) * -1.f);

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
		// �����������϶��� ������ �ʱ�ȭ
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
	}

	m_vPrevRotation = *(m_pModelCom->Get_AnimationCenterRotation());
	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix());
}

_int CPlayer::Compute_Target_Direction(CLandObject* pAttackedObject)
{
	// F, B, L, R
	_int iDirection = -1;

	_vector vAttackedObjectLook = XMVector3Normalize(pAttackedObject->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));
	_vector vMyLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_float fTheta = 0.0f;
	_float fDot = XMVectorGetX(XMVector3Dot(vMyLook, vAttackedObjectLook));
	fTheta = XMConvertToDegrees(acosf(fDot));

	if (fDot >= 0.0f)
	{
		if (fTheta < 90)  // ����
			iDirection = 0;
		else // ����
			iDirection = 1;
	}
	else
	{
		if (fTheta < 90) // ����
			iDirection = 2;
		else // ������
			iDirection = 3;
	}

	return iDirection;
}

_int CPlayer::Compute_Target_Direction_Pos(_fvector vTargetPos)
{
	// F, B, L, R
	_int iDirection = -1;

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir = XMVector3Normalize(vMyPos - vTargetPos);
	vDir = XMVectorSetY(vDir, 0.f);

	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vMyRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	// Look ���Ϳ��� ���� ���
	_float fDotLook = XMVector3Dot(vMyLook, vDir).m128_f32[0];
	// Right ���Ϳ��� ���� ���
	_float fDotRight = XMVector3Dot(vMyRight, vDir).m128_f32[0];
	// ��/�� ����
	_float cos45 = cosf(XMConvertToRadians(45.f));
	iDirection = -1; // �ʱ�ȭ

	if (fDotLook > cos45) {
		// ���� (�ڻ��� 45������ ũ�� �������� ����)
		iDirection = 1;
	}
	else if (fDotLook < -cos45) {
		// ����
		iDirection = 0;
	}

	// ��/�� ���� (��/�ڰ� �ƴ� ��쿡�� �˻�)
	if (iDirection == -1) {
		if (fDotRight > cos45) {
			// ������
			iDirection = 2;
		}
		else if (fDotRight < -cos45) {
			// ����
			iDirection = 3;
		}
	}

	// ���� ��� ���ǿ� �ش���� �ʴ� ��츦 ó�� (���⼭ �߰� �˻縦 �� ���� ����)
	if (iDirection == -1) {
		if (fDotLook > 0) {
			iDirection = 1; // �������� ����
		}
		else {
			iDirection = 0; // �������� ����
		}
	}

	return iDirection;
}

void CPlayer::Radial_Event()
{
	auto& pCurEvents = m_pData->Get_Current_RadialEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pCameraModel->Get_AnimationCurrentPosition(nullptr, m_iCutSceneCamAnimIndex));
		_double Duration = *(m_pCameraModel->Get_AnimationDuration(nullptr, m_iCutSceneCamAnimIndex));

		if (CurPos >= pEvent.fAinmPosition && CurPos < Duration)
		{
			if (pEvent.iType == 0)
				m_pGameInstance->Set_RadialBlur(true);
			else
				m_pGameInstance->Set_RadialBlur(false);
		}
	}
}

void CPlayer::Face_Event()
{
	auto& pCurEvents = m_pData->Get_Current_FaceEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pCameraModel->Get_AnimationCurrentPosition(nullptr, m_iCutSceneCamAnimIndex));
		_double Duration = *(m_pCameraModel->Get_AnimationDuration(nullptr, m_iCutSceneCamAnimIndex));

		if (CurPos >= pEvent.fAinmPosition && CurPos < Duration)
		{
			if (pEvent.iType == 0)
			{
				m_iFaceAnimIndex = pEvent.iFaceAnimIndex;
				On_Separation_Face();
			}
			else if (pEvent.iType == 2)
			{
				m_iFaceAnimIndex = pEvent.iFaceAnimIndex;
			}
			else if (pEvent.iType == 1)
			{
				Off_Separation_Face();
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
		if ((_uint)KRS_BEHAVIOR_STATE::HIT == m_iCurrentBehavior)
		{
			// HIT������ ���� �ִϸ��̼��� �����ٸ�
			string strAnimName = m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationName();
			// �ٿ���� ���������� �˻��ؼ�
			if (string::npos != strAnimName.find("dwn"))
			{
				// ���� �ൿ �ʱ�ȭ���ְ�
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				// �ٿ� ���·� �������ش�.
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::DOWN;

				// �Ʒ� ������ ���Ե� �ִϸ��̼ǵ��� ���������·� �̾�����
				if (string::npos != strAnimName.find("body_l") || (string::npos == strAnimName.find("body") && string::npos != strAnimName.find("_b"))
					|| string::npos != strAnimName.find("y_b") 
					|| string::npos != strAnimName.find("_guard_") || string::npos != strAnimName.find("_dnf_"))
				{
					CKiryu_KRS_Down::KRS_DOWN_DESC Desc{ 0, -1, string() };
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value(&Desc);
				}
				else if (string::npos != strAnimName.find("body_r") || string::npos != strAnimName.find("_f")
					|| string::npos != strAnimName.find("_direct_") || string::npos != strAnimName.find("dnb"))
				{
					CKiryu_KRS_Down::KRS_DOWN_DESC Desc{ 1, -1, string() };
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value(&Desc);
				}

			}
			else
			{
				// ���� ���°� ���̵��� �ƴ϶�� 
				if ((_uint)KRS_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::IDLE;
			}
		}
		else
		{
			// ���� ���°� ���̵��� �ƴ϶�� 
			if ((_uint)KRS_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
			m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::IDLE;
		}
	}

	_bool isShift = { false };
	_bool isMove = { false };

	if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::SKILL_FLY_KICK) return;

	if (m_pGameInstance->GetKeyState(DIK_SPACE) == HOLD)
	{
		isShift = true;
	}
	else if (m_pGameInstance->GetKeyState(DIK_SPACE) == AWAY)
	{
		if(m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::WALK)
			m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::IDLE;
	}

	if (!m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stopping() && m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::GRAB
		&& m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::PICK_UP)
	{
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

			_bool isHitActionPlay = { false };
			if (m_CanHitAction)
			{
				// �躮�� ���� �����¿����� ����
				if (m_pTargetWall != nullptr && m_pUIManager->Get_Skill_Holligan()[CUIManager::WALL_ATTACK])
				{
					isHitActionPlay = true;
					HitAction_WallBack();
				}
				else if (m_pTargetObject == nullptr ? false : m_pTargetObject->isDown())
				{
					isHitActionPlay = true;
					HitAction_Down();
				}
			}

			if (!isHitActionPlay) 
			{
				if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN)
				{
					m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::SKILL_FLY_KICK;
				}
				// �⺻ �����޺� �������� ���� ��Ŭ���� ������ �ǴϽ� ��� ����
				else if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::ATTACK)
				{
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count(true);
				}
				// �ƹ��͵� �ƴ� ���¿��� ��Ŭ���� ���´ٸ� ű�޺��� ����
				// ű�޺��� ��� ���¿����� ����
				else if(m_pUIManager->Get_Skill_Holligan()[CUIManager::KICK_COMBO])
				{
					m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::SKILL_KICK_COMBO;
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
				}
			}
		}

		// �������� �ƴҶ����� Q�Է��� �޴´�
		if (m_pGameInstance->GetKeyState(DIK_Q) == TAP 
			&&	m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::ATTACK && m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::GRAB 
			&& m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::PICK_UP)
		{
			// ������ Ÿ���� �ȵǾ����� �� Grab���� ������
			if (m_pTargetItem == nullptr)
			{
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::GRAB;
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
			}
			// ������ Ÿ���� �Ǿ����� ���� PickUp���� ������.
			else
			{
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::PICK_UP;
				dynamic_cast<CItem*>(m_pTargetItem)->Set_ParentMatrix(m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_l_n"));
				dynamic_cast<CItem*>(m_pTargetItem)->Set_Grab(true);
			}

		}
	}

	if (m_iCurrentBehavior < (_uint)KRS_BEHAVIOR_STATE::ATTACK && !m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stopping())
	{
		if (m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::WALK && m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::GRAB)
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
		}

		if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
		{
			// ������ ���� Ű �Է��� ���� ������ ������ Back�������� �̵��ؾ��ϱ� ������ Ű�Է¿��� üũ�ؼ� ���� �ʱ�ȭ
			if (!isMove)
				Reset_MoveDirection();

			m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::SWAY;
		}
	}

	if(isMove)
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

	if (!isMove && m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN || m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::WALK)
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stop();
}

void CPlayer::KRH_KeyInput(const _float& fTimeDelta)
{
	if (m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationEnd())
	{
		if ((_uint)KRH_BEHAVIOR_STATE::HIT == m_iCurrentBehavior)
		{
			// HIT������ ���� �ִϸ��̼��� �����ٸ�
			string strAnimName = m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationName();
			// �ٿ���� ���������� �˻��ؼ�
			if (string::npos != strAnimName.find("dwn"))
			{
				// ���� �ൿ �ʱ�ȭ���ְ�
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				// �ٿ� ���·� �������ش�.
				m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::DOWN;

				// �Ʒ� ������ ���Ե� �ִϸ��̼ǵ��� ���������·� �̾�����
				if (string::npos != strAnimName.find("body_l") || (string::npos == strAnimName.find("body") && string::npos != strAnimName.find("_b"))
					|| string::npos != strAnimName.find("y_b")
					|| string::npos != strAnimName.find("_guard_") || string::npos != strAnimName.find("_dnf_"))
				{
					CKiryu_KRS_Down::KRS_DOWN_DESC Desc{ 0, -1, string()};
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value(&Desc);
				}
				else if (string::npos != strAnimName.find("body_r") || string::npos != strAnimName.find("_f")
					|| string::npos != strAnimName.find("_direct_") || string::npos != strAnimName.find("dnb"))
				{
					CKiryu_KRS_Down::KRS_DOWN_DESC Desc{ 1, -1, string() };
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value(&Desc);
				}

			}
			else if (m_iCurrentBehavior != (_uint)KRH_BEHAVIOR_STATE::DOWN)
			{
				// ���� ���°� ���̵��� �ƴ϶�� 
				if ((_uint)KRH_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::IDLE;
			}
		}
		else
		{
			// ���� ���°� ���̵��� �ƴ϶�� 
			if ((_uint)KRH_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
			m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::IDLE;
		}
	}

	_bool isShift = { false };
	_bool isMove = { false };

	if (m_pGameInstance->GetKeyState(DIK_SPACE) == HOLD)
	{
		isShift = true;
	}
	else if (m_pGameInstance->GetKeyState(DIK_SPACE) == AWAY)
	{
		if (m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::WALK)
			m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::IDLE;
	}

	if (!m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stopping() && m_iCurrentBehavior != (_uint)KRH_BEHAVIOR_STATE::DOWN)
	{
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
			_bool isHitActionPlay = { false };

			if (m_CanHitAction)
			{
				if (m_pTargetWall != nullptr)
				{
					isHitActionPlay = true;
					HitAction_WallBack();
				}
				else if (m_pTargetObject == nullptr ? false : m_pTargetObject->isDown())
				{
					isHitActionPlay = true;
					HitAction_Down();
				}
				else if (m_pTargetObject == nullptr ? false : m_pTargetObject->isAttack())
				{
					isHitActionPlay = true;
					HitAction_CounterElbow();
				}
			}

			if (!isHitActionPlay)
			{
				if (m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::ATTACK)
				{
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count(true);
				}
				else
				{
					m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::ATTACK;

					_bool isPunch = true;
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value(&isPunch);
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
				}
			}
		}

	}


	if (m_iCurrentBehavior < (_uint)KRH_BEHAVIOR_STATE::ATTACK && !m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stopping())
	{
		// �ȱ������ ������ �ִϸ��̼ǿ��� ���� Ű�Է��� �޾Ƽ� ����Ѵ�
		if (m_iCurrentBehavior != (_uint)KRH_BEHAVIOR_STATE::WALK)
		{
			if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
			{
				_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamLook());
				m_pGameInstance->Get_CamLook();
				m_iCurrentBehavior = isShift ? (_uint)KRH_BEHAVIOR_STATE::WALK : (_uint)KRH_BEHAVIOR_STATE::RUN;

				m_InputDirection[F] = true;
				Compute_MoveDirection_FB();
				m_pTransformCom->LookAt_For_LandObject(vLookPos);
				isMove = true;

				if (m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::WALK || m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::RUN)
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
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
		
	}

	if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
	{
		// �����̰� �޺��Է����� ���� ������, �̹� �Է��� ���Ŀ� ���� �Է��̶�� ���� ������ �����ؾ��ϹǷ� �ʱ�ȭ��������
		if (m_iCurrentBehavior != (_uint)KRH_BEHAVIOR_STATE::SWAY)
		{
			// ������ ���� Ű �Է��� ���� ������ ������ Back�������� �̵��ؾ��ϱ� ������ Ű�Է¿��� üũ�ؼ� ���� �ʱ�ȭ
			if (!isMove)
				Reset_MoveDirection();
		}

		m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::SWAY;
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
	}

	if (isMove)
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

	if (!isMove && m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::RUN || m_iCurrentBehavior == (_uint)KRH_BEHAVIOR_STATE::WALK)
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stop();
}

void CPlayer::KRC_KeyInput(const _float& fTimeDelta)
{
	if (m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationEnd())
	{
		if ((_uint)KRC_BEHAVIOR_STATE::GUARD == m_iCurrentBehavior)
		{
			if (m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stopping())
			{
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::IDLE;
			}
		}
		else
		{
			if ((_uint)KRC_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
			m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::IDLE;
		}
		
	}

	_bool isShift = { false };
	_bool isMove = { false };

	if (m_pGameInstance->GetKeyState(DIK_SPACE) == HOLD)
	{
		isShift = true;
	}
	else if (m_pGameInstance->GetKeyState(DIK_SPACE) == AWAY)
	{
		if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::WALK)
			m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::IDLE;
	}

	if (m_pGameInstance->GetKeyState(DIK_LSHIFT) == HOLD)
	{
		m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::GUARD;
	}
	else if (m_pGameInstance->GetKeyState(DIK_LSHIFT) == AWAY)
	{
		if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::GUARD)
		{
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stop();
			//m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::IDLE;
		}
	}

	if (m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::GUARD)
	{
		if (!m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stopping() && m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::GRAB
			&& m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::PICK_UP)
		{
			if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
			{
				// ���� �ൿ�� �ʱ�ȭ�ϰ� �������� �ٲ��ش�.
				if (m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::ATTACK)
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

				// ��Ŭ �޺� ����, Ÿ���õǴ� �������� �ִٸ� �ڵ����� ���´�.
				if (nullptr == m_pTargetItem)
				{
					m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::ATTACK;
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
				}
				else
				{
					m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::PICK_UP;
					CKiryu_KRC_PickUp::KRC_PICK_UP_DESC Desc{};
					_uint iCount = static_cast<CKiryu_KRC_Attack*>(m_AnimationTree[KRC].at((_uint)KRC_BEHAVIOR_STATE::ATTACK))->Get_ComboCount();

					Desc.iComboCount = iCount > 3 ? 3 : iCount;
					Desc.isLeft = Compute_Target_Direction_Pos(m_pTargetItem->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)) == 3 ? true : false;

					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Event(&Desc);

					// ������ ���� �ڵ�
					dynamic_cast<CItem*>(m_pTargetItem)->Set_ParentMatrix(m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_l_n"));
					dynamic_cast<CItem*>(m_pTargetItem)->Set_Grab(true);
				}

			}
			if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
			{
				// ���� ���û��������� �����ؼ� ������ �׼��� �����Ű�ų�
				// �׿� �´� Ŀ�ǵ� �׼��� ������Ѿ� �Ѵ�.

				_bool isHitActionPlay = { false };
				if (m_CanHitAction)
				{
					if (m_pTargetObject == nullptr ? false : m_pTargetObject->isDown())
					{
						isHitActionPlay = true;
						HitAction_Down();
					}
				}

				if (!isHitActionPlay)
				{
					if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::ATTACK)
					{
						m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count(true);
					}
					else
					{
						m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::ATTACK;

						_bool isBut = true;
						m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value(&isBut);
						m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
					}
				}
			}

			// �������� �ƴҶ����� Q�Է��� �޴´�
			if (m_pGameInstance->GetKeyState(DIK_Q) == TAP
				&& m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::ATTACK && m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::GRAB
				&& m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::PICK_UP)
			{
				// ������ Ÿ���� �ȵǾ����� �� Grab���� ������
				if (m_pTargetItem == nullptr)
				{
					m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::GRAB;
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				}
				// ������ Ÿ���� �Ǿ����� ���� PickUp���� ������.
				else
				{
					// Q�� ���� �������� ������ ���´�
					m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::PICK_UP;
					dynamic_cast<CItem*>(m_pTargetItem)->Set_ParentMatrix(m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_l_n"));
					dynamic_cast<CItem*>(m_pTargetItem)->Set_Grab(true);
				}

			}
		}

		if (m_iCurrentBehavior < (_uint)KRC_BEHAVIOR_STATE::ATTACK && !m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stopping())
		{
			if (m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::WALK)
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
			}

			if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
			{
				// ������ ���� Ű �Է��� ���� ������ ������ Back�������� �̵��ؾ��ϱ� ������ Ű�Է¿��� üũ�ؼ� ���� �ʱ�ȭ
				if (!isMove)
					Reset_MoveDirection();

				m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::SWAY;
			}
		}
	}

	if (isMove)
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
	
	if (!isMove && m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::RUN || m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::WALK)
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stop();
}

HRESULT CPlayer::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Kiryu"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Kiryu_CamAction"),
		TEXT("Com_Model_Cam"), reinterpret_cast<CComponent**>(&m_pCameraModel))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.5, 0.8, 0.5);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_CutSceneAnim_ForPlayer"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	//Prototype_Component_Anim_KiryuFace
	CAnim* pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_KiryuFace"),
		TEXT("Com_Anim_Face"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);

	//Prototype_Component_Anim_Hand
	pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_Hand"),
		TEXT("Com_Anim_Hand"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);

	//Prototype_Component_Anim_Kiryu
	pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_Kiryu"),
		TEXT("Com_Anim_Default"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Material_Kiryu"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_BoneCompute"),
		TEXT("Com_ComputeShader"), reinterpret_cast<CComponent**>(&m_pComputeShaderCom))))
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

#ifdef _DEBUG
	_float2 vTexcoord = m_pDebugManager->Get_Texcoord();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexcoordX", &vTexcoord.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexcoordY", &vTexcoord.y, sizeof(_float))))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

void CPlayer::Compute_Height()
{
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	float fHeight = m_pNavigationCom->Compute_Height(vCurrentPos);

	vCurrentPos = XMVectorSetY(vCurrentPos, fHeight);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
}


void CPlayer::Change_Animation(_uint iIndex, _float fInterval)
{
	if (m_isCutSceneStartMotion) return;
	if (m_eAnimComType == CUTSCENE) return;

	if (m_pModelCom->Set_AnimationIndex(iIndex, fInterval))
	{
		// ������ �ִϸ��̼� ü������ �Ͼ�� �� �����ִ� ���� �ݶ��̴��� ���� ����
		Off_Attack_Colliders();
		Off_Trails();
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
		m_fPrevSpeed = 0.f;
	}
	
	string strAnimName = string(m_pModelCom->Get_AnimationName(iIndex));
	strAnimName = m_pGameInstance->Extract_String(strAnimName, '[', ']');
	m_pData->Set_CurrentAnimation(strAnimName);
}

void CPlayer::Change_ResetAnimaition(_uint iIndex, _float fInterval)
{
	if (m_isCutSceneStartMotion) return;

	m_pModelCom->Reset_Animation(iIndex);

	if (m_pModelCom->Set_AnimationIndex(iIndex, fInterval))
	{
		// ������ �ִϸ��̼� ü������ �Ͼ�� �� �����ִ� ���� �ݶ��̴��� ���� ����
		Off_Attack_Colliders();
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
	m_isAuraOn = false;
	m_eCurrentStyle = eStyle;
	m_iCurrentBehavior = 0;
	m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
	m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();

	m_pUIManager->Change_Style(eStyle);

	Off_Aura(eStyle);
}

void CPlayer::Reset_MoveDirection()
{
	ZeroMemory(m_MoveDirection, sizeof(_bool) * MOVE_DIRECTION_END);
	ZeroMemory(m_InputDirection, sizeof(_bool) * MOVE_DIRECTION_END);
}

void CPlayer::Set_CutSceneStartMotion(CUTSCENE_ANIMATION_TYPE eType)
{
	/*
	*	[648]	[p_sh1010_kiryu_gswing]
		[650]	[p_sh11330_noukudaki_climax]
		[651]	[p_sh1511_oi_kickover_utu_c]
		[652]	[p_sh1520_oiuchi_head_punch_ao]
		[653]	[p_sh1530_nage_oiuchi_lapel]
		[654]	[p_sh1540_nage_oiuchi_neck]
		[655]	[p_sh1550_oi_combo]
		[656]	[p_sh1620_mount]
		[657]	[p_sh20021_gougeki_c]
		[659]	[p_sh2040_pole_knock_lapel]
		[660]	[p_sh2140_brainbuster_bench]
		[661]	[p_sh23000_kabe_airon]
		[662]	[p_sh23020_oi_upper]
		[663]	[p_sh3261_doramukan_88]
		[664]	[p_sh5470_gao_kneekick]
	*/

	m_isCutSceneStartMotion = true;
	switch (eType)
	{
	case Client::CPlayer::GOUGEKI_C:
	{
		//[657]	[p_sh20021_gougeki_c]
		m_pModelCom->Set_AnimationIndex(657, 4.f);
		m_pGameInstance->PlaySound_W(TEXT("HitAction_Start.wav"), SOUND_UI, 0.7f);
		break;
	}
	case Client::CPlayer::OI_TRAMPLE_AO:
	{
		//[655]	[p_sh1550_oi_combo]
		m_pModelCom->Set_AnimationIndex(655, 4.f);
		m_pGameInstance->PlaySound_W(TEXT("HitAction_Start.wav"), SOUND_UI, 0.7f);
		break;

	}
	case Client::CPlayer::OI_KICKOVER_UTU_C:
	{
		//[651]	[p_sh1511_oi_kickover_utu_c]
		m_pModelCom->Set_AnimationIndex(651, 4.f);
		m_pGameInstance->PlaySound_W(TEXT("HitAction_Start.wav"), SOUND_UI, 0.7f);
		break;

	}
	case Client::CPlayer::KIRYU_GSWING:
	{
		//[648]	[p_sh1010_kiryu_gswing]
		m_pModelCom->Set_AnimationIndex(648, 4.f);
		m_pGameInstance->PlaySound_W(TEXT("HitAction_Start.wav"), SOUND_UI, 0.7f);
		break;

	}
	case Client::CPlayer::LAPEL_OIUCHI_NECK:
	{
		//[653]	[p_sh1530_nage_oiuchi_lapel]
		m_pModelCom->Set_AnimationIndex(653, 4.f);
		m_pGameInstance->PlaySound_W(TEXT("HitAction_Start.wav"), SOUND_UI, 0.7f);
		break;

	}
	case Client::CPlayer::NAGE_OIUCHI_NECK:
	{
		//[654]	[p_sh1540_nage_oiuchi_neck]
		m_pModelCom->Set_AnimationIndex(651, 4.f);
		m_pGameInstance->PlaySound_W(TEXT("HitAction_Start.wav"), SOUND_UI, 0.7f);
		break;

	}
	case Client::CPlayer::POLE_KNOCK_LAPEL:
	{
		//	[659]	[p_sh2040_pole_knock_lapel]
		m_pModelCom->Set_AnimationIndex(659, 4.f);
		m_pGameInstance->PlaySound_W(TEXT("HitAction_Start.wav"), SOUND_UI, 0.7f);
		break;
	}
	case Client::CPlayer::DORAMUKAN_88:
	{
		//	[663]	[p_sh3261_doramukan_88]
		m_pModelCom->Set_AnimationIndex(663, 4.f);
		m_pGameInstance->PlaySound_W(TEXT("HitAction_Start.wav"), SOUND_UI, 0.7f);
		break;
	}
	case Client::CPlayer::OI_KICK:
	{
		//[662]	[p_sh23020_oi_upper]
		m_pModelCom->Set_AnimationIndex(662, 4.f);
		m_pGameInstance->PlaySound_W(TEXT("HitAction_Start.wav"), SOUND_UI, 0.7f);
		break;
	}
	case Client::CPlayer::OI_UPPER:
	{
		//[662]	[p_sh23020_oi_upper]
		m_pModelCom->Set_AnimationIndex(662, 4.f);
		m_pGameInstance->PlaySound_W(TEXT("HitAction_Start.wav"), SOUND_UI, 0.7f);
		break;
	}
	// ���� �ִϸ��̼��� ���� �ƽ��̶�� false�� �����ش�.
	default:
	{
		m_isCutSceneStartMotion = false;
		Reset_CutSceneEvent();
		break;
	}
	}

}

void CPlayer::Set_CutSceneAnim(CUTSCENE_ANIMATION_TYPE eType)
{
	if (CUTSCENE == m_eAnimComType) return;
	Set_CutSceneStartMotion(eType);

	// ������ ����
	auto iter = m_CutSceneAnimation.find(eType);
	if (m_CutSceneAnimation.end() == iter) return;

	m_eCutSceneType = eType;

	//On_Separation_Face();			// �� �ִϸ��̼� �ѱ�
	Off_Separation_Hand();			// �� �и� �ִϸ��̼� ����

	m_pData->Set_CurrentCutSceneAnimation(iter->second);

	string AnimName = (*iter).second;

	_uint i = 0;
	auto AnimList = m_pAnimCom->Get_Animations();
	for (auto& pAnimation : AnimList)
	{
		string ExtractName = m_pGameInstance->Extract_String(pAnimation->Get_AnimName(), '[', ']');

		// ���ڿ� �������̶�� �ε��� ���� �� �ݺ��� ����
		if (ExtractName.find(AnimName) != string::npos)
		{
			m_iCutSceneAnimIndex = i;
			break;
		}

		i++;;
	}

	_uint j = 0;
	auto CameraAnimList = m_pCameraModel->Get_Animations();
	for (auto& pAnimation : CameraAnimList)
	{
		string CameraAnimName = pAnimation->Get_AnimName();

		// �ִϸ��̼� �̸��� .cmt�� ���Ե� ��츸 ī�޶� �ִϸ��̼��̴�.
		if (CameraAnimName.find(".cmt") != std::string::npos && CameraAnimName.find(AnimName) != std::string::npos)
		{
			m_iCutSceneCamAnimIndex = j;
			break;
		}
		j++;
	}

	if(nullptr != m_pTargetObject && !m_isCutSceneStartMotion)
		m_pTargetObject->Set_Sync(m_CutSceneAnimation[m_eCutSceneType]);

	m_pQTEMgr->Set_Animation(m_pAnimCom, m_CutSceneAnimation.at(eType));
}

void CPlayer::Play_CutScene()
{
	if (m_isCutSceneStartMotion)
	{
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), m_pSystemManager->Get_Camera()));
		_double fRatio = *m_pModelCom->Get_AnimationCurrentPosition() / *m_pModelCom->Get_AnimationDuration();
		pCamera->Set_CustomRatio(static_cast<_float>(fRatio));

		m_pGameInstance->Set_RadialBlur(true);

		pCamera->Set_TargetFoV(XMConvertToRadians(30.f));
		pCamera->Start_Zoom();
		
		if (m_pModelCom->Get_AnimFinished())
		{
			m_pGameInstance->Set_RadialBlur(false);
			m_isCutSceneStartMotion = false;
			Reset_CutSceneEvent();
			m_pTargetObject->Set_Sync(m_CutSceneAnimation[m_eCutSceneType]);
		}
	}

	if (CUTSCENE == m_eAnimComType)
	{
		// ī�޶� ���� �ִϸ��̼��� ����Ǹ� �Ȱ��� �÷��̾��� �ִϸ��̼ǵ� ����� ���̱� ������ �������·� �ǵ�����.
		if (m_pCameraModel->Get_AnimFinished())
		{
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			m_iCurrentBehavior = 1;				// Idle���·� �ǵ����д�
			Reset_CutSceneEvent();

			Off_Separation_Face();				// �ƽ� ���� �� �� �ִϸ��̼� ����
			On_Separation_Hand();				// �ƽ� ���� �� �� �ִϸ��̼� �ѱ�

			//// �ƽ� �ִϸ��̼��� ����� ���Ŀ� ��ġ�ϵ��� �ϴ� �ڵ� (�����ȵ�)
			//_matrix boneMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix("center_c_n")) * m_pTransformCom->Get_WorldMatrix();
			//_vector vPos = boneMatrix.r[CTransform::STATE_POSITION];
			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);			
			return;
		}

		// ������ ���� �ִϸ��̼��� �����°� ������Ʈ�̰�, m_pCameraModel�� ī�޶� �ִϸ��̼��� �����ϴ� ���̶� ���������ʴ´�
		m_pModelCom->Play_Animation_CutScene(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_pAnimCom, false, m_iCutSceneAnimIndex, false);

		CPlayer* pPlayer = this;
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CUTSCENE));
		// Blender���� ���� ���� ��ȯ ���
		_matrix matBoneMatrix = XMLoadFloat4x4(m_pCameraModel->Get_BoneCombinedTransformationMatrix("Camera"));

		// �÷��̾��� ���� ��ȯ ���
		//_matrix matPlayerWorld = pPlayer->Get_TransformCom()->Get_WorldMatrix();
		_matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();

		_matrix matVectorBoneWorld = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix("vector_c_n"));

		// Blender�� ��ǥ�踦 DirectX�� ��ǥ��� ��ȯ�ϱ� ���� ȸ�� ���
		_matrix rotationMatrixX = XMMatrixRotationX(XMConvertToRadians(90));
		_matrix rotationMatrixY = XMMatrixRotationY(XMConvertToRadians(-180));
		_matrix rotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(90));

		// Blender�� �� ��ȯ ��İ� �÷��̾��� ���� ��ȯ ����� �����ϰ� ��ǥ�� ��ȯ�� ����
		_matrix finalMat = rotationMatrixX * rotationMatrixY * rotationMatrixZ * matVectorBoneWorld * matBoneMatrix * matPlayerWorld;

		// ���� �� ����� ���
		_matrix viewMatrix = XMMatrixInverse(nullptr, finalMat);

		bool containsNaN = XMMatrixIsNaN(viewMatrix);
		if (containsNaN)
			return;

		// �� ����� ���������ο� ����
		m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, viewMatrix);

		auto KeyFrames = m_pCameraModel->Get_CurrentKeyFrameIndices(m_iCutSceneCamAnimIndex);
		_uint iKeyFrameIndex = KeyFrames->front();

		_float fFov = m_pCameraModel->Get_FoV(m_pCameraModel->Get_AnimationName(m_iCutSceneCamAnimIndex), iKeyFrameIndex);
		pCamera->Set_FoV(m_pCameraModel->Get_FoV(m_pCameraModel->Get_AnimationName(m_iCutSceneCamAnimIndex), iKeyFrameIndex));

		CModel::ANIMATION_DESC Desc{ m_iCutSceneCamAnimIndex, false };
		m_pCameraModel->Set_AnimationIndex(Desc);

		// ī�޶� �� �ִϸ��̼� ����
		m_pCameraModel->Play_Animation_CutScene(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), nullptr, false, m_iCutSceneCamAnimIndex, false);
	}
}

void CPlayer::Reset_CutSceneEvent()
{
	CAMERA eCurrentCam = m_pSystemManager->Get_Camera();

	switch (eCurrentCam)
	{
		// ���� �÷��̾� ī�޶��̸�, �ƽ����� ������ ��Ȳ
	case Client::CAMERA_PLAYER:
	{
		CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
		// �÷��̾� ī�޶��� ���� ���¸� �����Ѵ�.
		pCamera->Store_PrevMatrix();

		// �ƽ����� ���� ��, �ƽ� ī�޶� �ʱ�ȭ���ش�.
		CCutSceneCamera* pCutSceneCamera = dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CUTSCENE));
		pCutSceneCamera->Reset_ReturnVariables();

		break;
	}

	// ���� �ƽ� ī�޶��̸�, �÷��̾� ī�޶�� ������ ��Ȳ
	case Client::CAMERA_CUTSCENE:
		// ���� �ƽ�ī�޶��� ������ ��İ� Fov�� �޾ƿͼ�
		CCutSceneCamera* pCutSceneCamera = dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CUTSCENE));
		//_matrix LastMatrix = XMLoadFloat4x4(pCutSceneCamera->Get_WorldMatrix());
		_matrix LastMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
		_float fLastFov = pCutSceneCamera->Get_Fov();
		pCutSceneCamera->On_Return();		

		// Ȥ�ø𸣴� ��� ������ ��ũ�� �������ش�.
		if(nullptr != m_pTargetObject)
			m_pTargetObject->Off_Sync();

		break;
	}

	// �� �� �����ϴ� �ִϸ��̼ǵ��� ���������� ���� �ʴ� �ִϸ��̼��̹Ƿ� �������� ������ 0���� ��!! �ʱ�ȭ�ؾ��Ѵ�.
	m_pModelCom->Set_ChangeInterval(0.0);
	m_pCameraModel->Set_ChangeInterval(0.0);
	m_pAnimCom->Reset_Animation(m_iCutSceneAnimIndex);
	m_pCameraModel->Reset_Animation(m_iCutSceneCamAnimIndex);

	m_eAnimComType = (m_eAnimComType == DEFAULT ? CUTSCENE : DEFAULT);

	// �׸��� ü����
	m_pSystemManager->Set_Camera(CAMERA_CUTSCENE == m_pSystemManager->Get_Camera() ? CAMERA_PLAYER : CAMERA_CUTSCENE);
}

void CPlayer::HitAction_Down()
{
	if (KRH == m_eCurrentStyle)
	{
		// ����� �Ѿ�������, ������ �Ѿ������� �Ǵ��ؼ� ������Ѿ���
		/*
		* DIR_F : ������ ��������
		* DIR_B : �ڷ� ����������
		* DIR_END : ������ �������� ����
		*/
		Set_CutSceneAnim(m_pTargetObject->Get_DownDir() == DIR_F ? OI_TRAMPLE_AO : OI_KICKOVER_UTU_C);
	}
	else
	{
		Set_CutSceneAnim(m_pTargetObject->Get_DownDir() == DIR_F ? OI_UPPER : OI_KICK);
	}
}

void CPlayer::HitAction_WallBack()
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vWallPos = XMLoadFloat3(&m_pTargetWall->Get_Center());

	// F, B, L, R
	_int iDir = Compute_Target_Direction_Pos(vWallPos);
	if (B == iDir)
	{
		// TODO: �躮�� ��) ��ġ Set�������.
		//CBounding_AABB::BOUNDING_AABB_DESC* pDesc = reinterpret_cast<CBounding_AABB::BOUNDING_AABB_DESC*>(m_pTargetWall->Get_Desc());
		//_vector vExtents = XMLoadFloat3(&pDesc->vExtents);

		//_vector vPos = vWallPos;
		//vPos.m128_f32[0] += vExtents.m128_f32[0];
		//	

		//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		//vPos.m128_f32[0] += 1.f;
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		Set_CutSceneAnim(m_eCurrentStyle == KRS ? HAIHEKI_KICK : HAIHEKI_PUNCH);
	}
}

void CPlayer::HitAction_CounterElbow()
{
	Set_CutSceneAnim(KOBUSHIKUDAKI);
}

void CPlayer::Update_RootFalseAnim()
{
	for (auto& index : m_RootFalseAnims)
	{
		if (m_pModelCom->Get_CurrentAnimationIndex() == index)
		{
			if(m_pModelCom->Get_AnimChanged())			//���������� ������ ������ ��ȯ���ش�.
				m_isRootAnim = false;
			break;
		}
		else
			m_isRootAnim = true;
	}
		
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
	if (0 < CPlayer::PlayerInfo.iCurrentHitLevel)
	{
		On_Aura(m_eCurrentStyle);
	}
	else
	{
		Off_Aura(ADVENTURE);
	}
}


void CPlayer::Setting_Target_Enemy()
{
	if (2 == m_iCurrentBehavior) return;
	if (CUTSCENE == m_eAnimComType) return;				// �ƽ��������̸� Ÿ���� �ǵ��� �ʴ´�.

	if (m_pGameInstance->Get_CurrentLevel() == LEVEL_TUTORIAL)
	{
		auto pMonsterGroups = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_MonsterGroup"));

		for (auto& pGroup : pMonsterGroups)
		{
			vector<CMonster*> pMonsters = dynamic_cast<CMonsterGroup*>(pGroup)->Get_Monsters();

			for (auto& pMonster : pMonsters)
			{
				if (nullptr != m_pTargetObject)
				{
					_float vDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));

					// ���� Ÿ�����̴� ģ���� �Ÿ��� 3.f �̻� �־����� �׶� �ٽ� Ÿ�����Ѵ�.
					if (3.f < vDistance)
					{
						_float vMonsterLength = 999999.f;
						_float vYonedaLength = 999999.f;
						if (nullptr != pMonster)
							vMonsterLength = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pMonster->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));

						m_pTargetObject = vMonsterLength < vYonedaLength ? static_cast<CMonster*>(pMonster) : static_cast<CMonster*>(pMonster);
					}
				}
				else
				{
					_float vMonsterLength = 999999.f;
					if (nullptr != pMonster)
						vMonsterLength = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pMonster->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));

					m_pTargetObject = static_cast<CMonster*>(pMonster);
				}
			}

		}
	}
	else
	{
		auto pMonsters = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Monster"));
		auto pYonedas = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Yoneda"));

		if (nullptr != m_pTargetObject)
		{
			_float vDistance = 99999.f;
			if (m_pTargetObject->isObjectDead())
			{
				m_pTargetObject = nullptr;
			}
			else
			{
				vDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));
			}

			// ���� Ÿ�����̴� ģ���� �Ÿ��� 3.f �̻� �־����� �׶� �ٽ� Ÿ�����Ѵ�.
			if (3.f < vDistance)
			{
				auto pMosnter = m_pCollisionManager->Get_Near_Object(this, pMonsters);
				auto pYoneda = m_pCollisionManager->Get_Near_Object(this, pYonedas);

				_float vMonsterLength = 999999.f;
				_float vYonedaLength = 999999.f;
				if (nullptr != pMosnter)
					vMonsterLength = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pMosnter->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));
				if (nullptr != pYoneda)
					vYonedaLength = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pYoneda->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));

				m_pTargetObject = vMonsterLength < vYonedaLength ? static_cast<CMonster*>(pMosnter) : static_cast<CMonster*>(pYoneda);
			}
		}
		else
		{
			auto pMosnter = m_pCollisionManager->Get_Near_Object(this, pMonsters);
			auto pYoneda = m_pCollisionManager->Get_Near_Object(this, pYonedas);

			_float vMonsterLength = 999999.f;
			_float vYonedaLength = 999999.f;
			if (nullptr != pMosnter)
				vMonsterLength = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pMosnter->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));
			if (nullptr != pYoneda)
				vYonedaLength = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pYoneda->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));

			m_pTargetObject = vMonsterLength < vYonedaLength ? static_cast<CMonster*>(pMosnter) : static_cast<CMonster*>(pYoneda);

		}
	}
}

void CPlayer::Setting_Target_Item()
{
	if (9 == m_iCurrentBehavior) return;

	auto pItemList = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Item"));

	m_pTargetItem = static_cast<CItem*>(m_pCollisionManager->Get_Near_Object(this, pItemList, 1.5f));
}

void CPlayer::Setting_Target_Wall()
{
	if (static_cast<_uint>(KRS_BEHAVIOR_STATE::RUN) < m_iCurrentBehavior) return;

	CMapCollider* pMapCollider = static_cast<CMapCollider*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_MapCollider"), 0));

	auto pWallList = pMapCollider->Get_Colliders();

	m_pTargetWall = (m_pCollisionManager->Get_Near_Collider(this, pWallList, 3.f));
}

void CPlayer::On_Aura(BATTLE_STYLE eStyle)
{
	switch (eStyle)
	{
	case CPlayer::KRS:
	{
		for (auto pEffect : m_HooliganAura)
			pEffect->On();

		if (!m_isAuraOn)
		{
			CEffect::EFFECT_DESC EffectDesc{};

			_matrix BoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix("kubi_c_n"));
			_matrix ComputeMatrix = BoneMatrix * m_pTransformCom->Get_WorldMatrix();
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, ComputeMatrix);

			EffectDesc.pWorldMatrix = &Matrix;
			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_Start_Hooligan"), TEXT("Layer_Particle"), &EffectDesc);

			m_isAuraOn = true;
		}
		break;
	}
	case CPlayer::KRH:
	{
		for (auto pEffect : m_RushAura)
			pEffect->On();

		if (!m_isAuraOn)
		{
			CEffect::EFFECT_DESC EffectDesc{};

			_matrix BoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix("kubi_c_n"));
			_matrix ComputeMatrix = BoneMatrix * m_pTransformCom->Get_WorldMatrix();
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, ComputeMatrix);

			EffectDesc.pWorldMatrix = &Matrix;
			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_Start_Rush"), TEXT("Layer_Particle"), &EffectDesc);

			m_isAuraOn = true;
		}

		break;
	}
	case CPlayer::KRC:
	{
		for (auto pEffect : m_DestroyerAura)
			pEffect->On();

		if (!m_isAuraOn)
		{
			CEffect::EFFECT_DESC EffectDesc{};

			_matrix BoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix("kubi_c_n"));
			_matrix ComputeMatrix = BoneMatrix * m_pTransformCom->Get_WorldMatrix();
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, ComputeMatrix);

			EffectDesc.pWorldMatrix = &Matrix;
			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_Start_Destroyer"), TEXT("Layer_Particle"), &EffectDesc);

			m_isAuraOn = true;
		}

		break;
	}
	}
}

void CPlayer::Off_Aura(BATTLE_STYLE eStyle)
{
	// eStyle ���� ���� ��Ÿ���̴�

	switch (eStyle)
	{
	case CPlayer::ADVENTURE:
	{
		for (auto pEffect : m_HooliganAura)
			pEffect->Off();

		for (auto pEffect : m_RushAura)
			pEffect->Off();

		for (auto pEffect : m_DestroyerAura)
			pEffect->Off();
		break;
	}
	case CPlayer::KRS:
	{
		for (auto pEffect : m_RushAura)
			pEffect->Off();

		for (auto pEffect : m_DestroyerAura)
			pEffect->Off();
		break;
	}
	case CPlayer::KRH:
	{
		for (auto pEffect : m_HooliganAura)
			pEffect->Off();

		for (auto pEffect : m_DestroyerAura)
			pEffect->Off();
		break;
	}
	case CPlayer::KRC:
	{
		for (auto pEffect : m_HooliganAura)
			pEffect->Off();

		for (auto pEffect : m_RushAura)
			pEffect->Off();

		break;
	}
	}
}

void CPlayer::AccHitGauge()
{
	if (PLAYER_HITGAUGE_LEVEL_INTERVAL * 3.f < CPlayer::PlayerInfo.fHitGauge)
		CPlayer::PlayerInfo.fHitGauge = PLAYER_HITGAUGE_LEVEL_INTERVAL * 3.f;
	else
		CPlayer::PlayerInfo.fHitGauge += 5.f;

	CPlayer::PlayerInfo.iCurrentHitLevel = (CPlayer::PlayerInfo.fHitGauge / PLAYER_HITGAUGE_LEVEL_INTERVAL);
}

void CPlayer::Setting_RimLight()
{
	switch (m_eCurrentStyle)
	{
	case Client::CPlayer::ADVENTURE:
	{
		m_isRimLight = 0.f;
		break;
	}
	case Client::CPlayer::KRS:
	{
		m_isRimLight = 0.1f;
		break;
	}
	case Client::CPlayer::KRH:
	{
		m_isRimLight = 0.2f;
		break;
	}
	case Client::CPlayer::KRC:
	{
		m_isRimLight = 0.3f;
		break;
	}
	default:
		break;
	}
}

HRESULT CPlayer::Bind_RimLight()
{
	_float isfale = 0.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &isfale, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CPlayer::HitFreeze_On()
{
	m_isHitFreeze = true;
	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.1f);
	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 0.1f);
	
	CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
	pCamera->Set_StartFov(XMConvertToRadians(45.0f));				//���� fov����
	pCamera->Set_FoV(XMConvertToRadians(45.0f));				//���� fov����
	pCamera->On_Return();

	m_pGameInstance->Set_RadialBlur(true);
}

void CPlayer::HitFreeze_Off()
{
	m_isHitFreeze = false;
	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 1.f);

	m_pGameInstance->Set_RadialBlur(false);
	//CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
	//pCamera->Set_FoV(XMConvertToRadians(60.0f));				//���� fov����
}

void CPlayer::HitFreeze_Timer(const _float& fTimeDelta)
{
	m_fHitFreezeTimer += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

	if (m_fHitFreezeTime <= m_fHitFreezeTimer)
	{
		m_fHitFreezeTimer = 0.f;
		HitFreeze_Off();
	}
}

void CPlayer::HitRadial_On()
{
	m_isHitRadial = true;
	m_pGameInstance->Set_RadialBlur(true);
}

void CPlayer::HitRadial_Off()
{
	m_isHitRadial = false;
	m_pGameInstance->Set_RadialBlur(false);
}

void CPlayer::HitRadial_Timer(const _float& fTimeDelta)
{
	m_fHitRadialTimer += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

	if (m_fHitRadialTime <= m_fHitRadialTimer)
	{
		m_fHitRadialTimer = 0.f;
		HitRadial_Off();
	}
}

void CPlayer::HitZoomIn_On()
{
	m_isHitZoom = true;

	CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
	pCamera->Set_StartFov(XMConvertToRadians(45.0f));				//���� fov����
	pCamera->Set_FoV(XMConvertToRadians(45.0f));				//���� fov����
	pCamera->On_Return();
}

void CPlayer::HitZoomIn_Off()
{
	m_isHitZoom = false;
}

void CPlayer::HitZoomIn_Timer(const _float& fTimeDelta)
{
	m_fHitZoomTimer += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

	if (m_fHitZoomTime <= m_fHitZoomTimer)
	{
		m_fHitZoomTimer = 0.f;
		HitZoomIn_Off();
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
	//Safe_Release(m_pTargetObject);			// AddRef ���������Ƿ� �������� �ʾƵ���
	Safe_Release(m_pQTEMgr);

	for (size_t i = 0; i < BATTLE_STYLE_END; i++)
	{
		for (auto& pair : m_AnimationTree[i])
			Safe_Release(pair.second);

		m_AnimationTree[i].clear();
	}

#ifdef _DEBUG
	Safe_Release(m_pDebugManager);
#endif // _DEBUG

	//Safe_Release(m_pTargetItem);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pAnimCom);
	Safe_Release(m_pCameraModel);

	for (size_t i = 0; i < BATTLE_STYLE_END; i++)
	{
		for (auto& pair : m_AnimationTree[i])
			Safe_Release(pair.second);

		m_AnimationTree[i].clear();
	}

	// AddRef�� �����ʾƼ� ������ x 
	//for (auto& pEffect : m_HooliganAura)
	//{
	//	Safe_Release(pEffect);
	//}
	m_HooliganAura.clear();
	//for (auto& pEffect : m_RushAura)
	//{
	//	Safe_Release(pEffect);
	//}
	m_RushAura.clear();
	//for (auto& pEffect : m_DestroyerAura)
	//{
	//	Safe_Release(pEffect);
	//}
	m_DestroyerAura.clear();

	__super::Free();
}