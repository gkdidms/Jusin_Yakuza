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

#pragma region Çàµ¿ °ü·Ã Çì´õµé
#include "Kiryu_KRC_Hit.h"
#include "Kiryu_KRH_Hit.h"
#include "Kiryu_KRS_Hit.h"
#include "Kiryu_KRS_Down.h"
#include "Kiryu_KRH_Down.h"
#include "Kiryu_KRS_Grab.h"
#include "Kiryu_KRC_Grab.h"
#include "Kiryu_KRS_PickUp.h"
#pragma endregion

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
}

void CPlayer::Set_SeizeOff(_bool isOff)
{
	// ·¯½¬´Â Àâ±â ±â´ÉÀÌ ¾ø´Ù
	if (KRH == m_eCurrentStyle) return;

	//8¹ø Grab °øÅë
	m_AnimationTree[m_eCurrentStyle].at(8)->Event(&isOff);
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
	Ready_CutSceneAnimation();

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	// ±âº» ¸ó½ºÅÍ: 20
	// »æÀïÀÌ: 30
	// ÄíÁ¦: 100
	m_Info.fMaxHP = 150.f;
	m_Info.fHp = m_Info.fMaxHP;

	ZeroMemory(&m_MoveDirection, sizeof(_bool) * MOVE_DIRECTION_END);
	ZeroMemory(&m_InputDirection, sizeof(_bool) * MOVE_DIRECTION_END);

	// »õ·Î »ı¼ºÇÒ ¶§ ¸¶´Ù UI¸Å´ÏÀú¿¡ º»ÀÎÀ» SetÇØÁØ´Ù.
	m_pUIManager->Set_Player(this);

	On_Separation_Hand(0);			// ¾ç¼Õ ºĞ¸® ÄÑµÒ

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

	// ¹èÆ² ½ÃÀÛ ¾Ö´Ï¸ŞÀÌ¼Ç ¾Æ´Ò °æ¿ì Å¸ÀÓµ¨Å¸¸¦ 1·Î °íÁ¤½ÃÄÑÁØ´Ù.
	// TODO: ´Ù¸¥°÷¿¡¼­ ½Ã°£Á¶ÀıÀÌ ÇÊ¿äÇÏ´Ù¸é ¼öÁ¤ÇØ¾ßÇÑ´Ù
	if (m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::BTL_START)
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);

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
	//¸Ş´º Ã¢ Ãß°¡
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
	if (m_pGameInstance->GetKeyState(DIK_T) == TAP)
	{
		m_pUIManager->Open_Scene(TEXT("Tutorial"));
	}
	if (m_pGameInstance->GetKeyState(DIK_Y) == TAP)
	{
		m_pUIManager->Open_Scene(TEXT("Carchase"));
	}


	Synchronize_Root(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

#ifdef _DEBUG
	if (m_pGameInstance->GetKeyState(DIK_Z) == TAP)
	{
		//TODO: ¿©±â¿¡¼­ enum°ªÀ» ÇÊ¿äÇÑ ¾Ö´Ï¸ŞÀÌ¼ÇÀ¸·Î ¹Ù²Ù¸é ÇØ´çÇÏ´Â ÄÆ½ÅÀÌ ½ÇÇàµÈ´ç
		if (m_pTargetObject)
		{
			Set_CutSceneAnim(m_eCutSceneType, 1);
			static_cast<CMonster*>(Get_TargetObject())->Set_Sync(m_CutSceneAnimation[m_eCutSceneType]);
		}
	}

	if (m_isAnimStart)
	{
		if (DEFAULT == m_eAnimComType)
		{
			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iHandAnimIndex, m_SeparationAnimComs[HAND_ANIM], false, (_int)HAND_ANIM);
			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, (_int)FACE_ANIM);
			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iDefaultAnimIndex, m_SeparationAnimComs[DEFAULT_ANIM], false, (_int)DEFAULT_ANIM);
			m_pModelCom->Play_Animation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));
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
		m_pModelCom->Play_Animation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));
	}
	else
	{
		m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iHandAnimIndex, m_SeparationAnimComs[HAND_ANIM], false, (_int)HAND_ANIM);
		m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, (_int)FACE_ANIM);
		Play_CutScene();
	}
#endif // _DEBUG

	for (auto& pCollider : m_pColliders)
		pCollider.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pEffects)
		pEffect.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pTrailEffects)
		pEffect.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	// TODO: Æ©Åä¸®¾ó UI Á¤¸®µÈ ÀÌÈÄ ÄÑ¾ßÇÔ
	//if(!m_pUIManager->IsOpend())
		KeyInput(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	Animation_Event();
	RimLight_Event();
	Trail_Event();
	Effect_Control_Aura();
	Setting_Target_Enemy();
	Setting_Target_Item();
}

void CPlayer::Late_Tick(const _float& fTimeDelta)
{
	m_pCollisionManager->Add_ImpulseResolution(this);

#ifdef _DEBUG
	if (m_isObjectRender)
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
		//m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow¿ë ·»´õ Ãß°¡
	}
#else
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow¿ë ·»´õ Ãß°¡
#endif // _DEBUG

	for (auto& pCollider : m_pColliders)
		pCollider.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pEffects)
		pEffect.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pTrailEffects)
		pEffect.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	// ÇöÀç ÄÑÁ®ÀÖ´Â Attack¿ë Äİ¶óÀÌ´õ »ğÀÔ
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::ATTACK && pPair.second->IsOn())
		{
			if (pPair.second->Get_CollierPartType() == 1)
				int a = 0;

			m_pCollisionManager->Add_AttackCollider(pPair.second, CCollision_Manager::PLAYER);
		}
	}

	// ÇöÀç ÄÑÁ®ÀÖ´Â Hit¿ë Äİ¶óÀÌ´õ »ğÀÔ (¾ÆÁ÷±îÁö´Â Hit¿ë Äİ¶óÀÌ´õ´Â Ç×»ó ÄÑÁ®ÀÖÀ½)
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::HIT && pPair.second->IsOn())
			m_pCollisionManager->Add_HitCollider(pPair.second, CCollision_Manager::PLAYER);
	}
	
	Setting_RimLight();
	Compute_Height();

	if (m_pModelCom->Get_AnimFinished())
	{
		// ¾Ö´Ï¸ŞÀÌ¼Å¤Á¤¤ÀÌ ³¡³µÀ» ¶§ ±âÁ¸ ¾Ö´Ï¸ŞÀÌ¼Ç¿¡¼­ È¸ÀüÇÑ °ª ¸¸Å­ È¸Àü½ÃÄÑÁØ´Ù.

		//_float3 vScaled = m_pTransformCom->Get_Scaled();
		//_vector vScale = XMLoadFloat3(&vScaled);
		//_vector vOriginRot = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		//_vector vRot = XMLoadFloat4(&m_vPrevRotation);
		//_vector vTranslation = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		//_matrix CenterRotationMatrix = XMMatrixTranspose(XMMatrixAffineTransformation(vScale, vOriginRot, vRot, vTranslation));

		//// ÀúÀåÇÏ°í ÀÖ´Â ÄõÅÍ´Ï¾ğÀÇ È¸Àü°ªÀº YÃà ¾çÀÇ ¹æÇâÀ» Forward·Î »ç¿ëÁßÀÎ È¸Àü°ªÀÌ¹Ç·Î,
		//// ZÃà ¾çÀÇ ¹æÇâÀ» forward·Î °¡Áö°Ô º¯È¯ÇØÁÖ¾î¾ß ÇÑ´Ù.
		////_matrix yFlipMatrix = XMMatrixRotationX(XMConvertToRadians(-90.0f));

		////// ÄõÅÍ´Ï¾ğ È¸Àü°ú YÃà È¸ÀüÀ» Àû¿ëÇÑ »õ·Î¿î È¸Àü Çà·Ä »ı¼º
		////_matrix newRotationMatrix = XMMatrixMultiply(yFlipMatrix, CenterRotationMatrix);

		//// ±âÁ¸ÀÇ ¿ùµå Çà·Ä¿¡ »õ·Î¿î È¸Àü Çà·ÄÀ» Àû¿ë
		//_matrix WorldMatrix = XMMatrixMultiply(CenterRotationMatrix, m_pTransformCom->Get_WorldMatrix());

		//XMVECTOR y90DegreeRotation = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(90.0f));
		//XMVECTOR transformedQuat = XMQuaternionMultiply(y90DegreeRotation, XMLoadFloat4(&m_vPrevRotation));

		//XMFLOAT3 euler;
		//XMFLOAT4 q;
		//XMStoreFloat4(&q, transformedQuat);

		//// ¿ÀÀÏ·¯ °¢µµ °è»ê (Yaw, Pitch, Roll)
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
		if(ADVENTURE != m_isRimLight)
		{
			// Àü½ÅÀÏ ¶§ ÀÓÀÇ·Î FullÀ» ÀúÀåÇØÁÖ°í »ç¿ëÇÑ´Ù.
			if (string_view(m_strRimMeshName) == string_view("Full"))
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimUV", &m_fRimPartsUV, sizeof(_float2))))
					return E_FAIL;
			}
			else if (string_view(m_strRimMeshName) == string_view(pMesh->Get_Name()))
			{
				_float2 fUV = m_fRimPartsUV;		// ±âº»ÀûÀ¸·Î ÆÄÃ÷uv¸¦ ³Ö°í
				if (string_view("[l0]jacketw1") == string_view(m_strRimMeshName))
					fUV = m_fRimTopUV;				// »óÃ¼ÀÏ ¶§ Å¾À» ³Ö¾îÁØ´Ù.
				if(string_view("[l0]pants3") == string_view(m_strRimMeshName))
					fUV = m_fRimBotUV;				// ¹ÙÁöÀÏ ¶§ ¹ÙÅÒÀ» ³Ö¾îÁØ´Ù.

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

			// ±â°ÔÀÌÁö°¡ ÄÑÁ®ÀÖ´Â »óÅÂ¶ó¸é »ó¹İ½Å ¸²¶óÀÌÆ®¸¦ ÄÒ´Ù
			if (0 < m_iCurrentHitLevel)
			{
				if (!strcmp(pMesh->Get_Name(), "[l0]jacketw1"))
				{
					if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_float))))
						return E_FAIL;

					if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimUV", &m_fRimPartsUV, sizeof(_float2))))
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
			// ¾îµåº¥Ã³ÀÏ¶§
			if (FAILED(Bind_RimLight()))
				return E_FAIL;
		}

		//¿Ê ¼ÎÀÌ´õ ±¸ºĞ¿ë
		_bool isCloth = pMesh->isCloth();

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
			m_pShaderCom->Begin(1);     //ºí·£µå
		else
			m_pShaderCom->Begin(0);		//µğÆúÆ®

		m_pModelCom->Render(i);

		i++;
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

// ³» °ø°İ Äİ¶óÀÌ´õ¿Í Ãæµ¹ÇßÀ» ¶§
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
			}

			break;
		}
		case CPlayer::KRC:
		{
			if (m_iCurrentBehavior == (_uint)KRC_BEHAVIOR_STATE::GRAB)
			{
				CKiryu_KRC_Grab::KRC_Grab_DESC Desc{ true, Compute_Target_Direction(pLandObject) };
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value(&Desc);
			}

			break;
		}
		}
	}
	
}

void CPlayer::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
	// iHitColliderType ´Â Ãæµ¹ÇÑ HITÅ¸ÀÔ Äİ¶óÀÌ´õ°¡ Çìµå, ¹Ùµğ, ·¹±×ÀÎÁö¸¦ °®°íÀÖ´Ù.

	// µ¥¹ÌÁö °¨¼ÒÀ² ¼³Á¤ (ÆÄ±«ÀÚ °¡µå ½Ã¿¡´Â µôÀ» Æò¼Òº¸´Ù 0.2¹è¸¸Å­¸¸ ¹Ş´Â´Ù (==µô°¨80ÆÛ))
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
		else if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::PICK_UP)		// ¹«¾ğ°¡ µé°í ÀÖ´Â »óÅÂ¸é µû·Î Ã³¸®ÇÑ´Ù.
		{
			string strAnimationName = pAttackedObject->Get_CurrentAnimationName();

			// È÷Æ® °´Ã¼¿¡¼­ ¾Ö´Ï¸ŞÀÌ¼Ç ¼¼ÆÃÇØÁÖ°í, ÇöÀç ±× ¾Ö´Ï¸ŞÀÌ¼ÇÀ» ²¨³»Áà¾ßÇÑ´Ù.
			CKiryu_KRS_Hit::KRS_Hit_DESC HitDesc{ &vDir, fDamage, strAnimationName, iDirection };
			
			m_iDefaultAnimIndex = m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationIndex();			//ÄÄÆ÷³ÍÆ®¿¡¼­ ¾µ ¾Ö´Ï¸ŞÀÌ¼Ç (pickup»óÅÂÀÇ ÀÎµ¦½º)

			CKiryu_KRS_PickUp::PICK_UP_HIT_DESC Desc{ m_AnimationTree[KRS].at((_uint)KRS_BEHAVIOR_STATE::HIT)->Get_AnimationIndex() };
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value((void*)&Desc);

			// »ç¿ëÀ» ´Ù ÇŞÀ¸¸é ´Ù½Ã ÃÊ±âÈ­ÇØÁØ´Ù.
			m_AnimationTree[KRS].at((_uint)KRS_BEHAVIOR_STATE::HIT)->Reset();

		}
		else if(m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::SKILL_FLY_KICK)						// ½´ÆÛ¾Æ¸Ó Àû¿ëÇÒ Çàµ¿ Å¸ÀÔµéÀ» ¿¹¿ÜÃ³¸®ÇØÁÖ¾î¾ßÇÑ´Ù.
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
		else
		{
			m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::HIT;
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value((void*)&Desc);
		}
		
		break;
	}
	}

	//µ¥¹ÌÁö Ã³¸®ÇÏ±â
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
		FINISHBLOW,                 //h23320 ÇÇ´Ï½Ã ºí·ÎÀÇ ±Ø
		GOUGEKI_C,                  // h20021 Àâ±â ÈÄ È÷Æ®¾×¼Ç (ÁÖ¸ÔÀ¸·Î ¾ó±¼¶§¸®°í ¹ÚÄ¡±â)
		HAIHEKI_KICK,               //h23070 º® µîÁö°í ¹«¸­Ä¡´Â ½ºÅ³

		KOBUSHIKUDAKI,              //h10111 ÆÈ²ŞÄ¡·Î °ø°İ¸·´Â ½ºÅ³
		HAIHEKI_PUNCH,              //h23060 º® µîÁö°í ³ª¿À´Â ½ºÅ³
		OI_TRAMPLE_AO,              //h1500 ´Ù¿îµÈ »ó´ë ¾ó±¼ ¹â±â (´©¿öÀÖ´Â »óÅÂ), ·¯½¬
		OI_KICKOVER_UTU_C,          //h1511 ´Ù¿îµÈ »ó´ë ¾ó±¼ Â÷±â (¾şµå·ÁÀÖ´Â »óÅÂ), ·¯½¬

		KIRYU_GSWING,               //h1010 ´Ù¸®Àâ°í µ¹·Á¼­ ½ºÇÃ¸´ °ø°İ
		DORYU_MIN,                  //h11285 ¸è»ìÀâ°í µ¹·Á¼­ ½ºÇÃ¸´ °ø°İ
		LAPEL_OIUCHI_NECK,           //h1540 µé¾î¼­ ¹Ù´Ú¿¡ ³»´øÁü (¾ÕÀâ)
		NAGE_OIUCHI_NECK,           //h1540 µé¾î¼­ ¹Ù´Ú¿¡ ³»´øÁü (µÚÀâ)
		POLE_KNOCK_LAPEL,           //h2040 ±ÙÃ³¿¡ ±âµÕÀÌ ÀÖ´Ù¸é ±âµÕ¿¡ ¹Ú°ÔÇÏ°í ¹âÀ½
		DORAMUKAN_88,               //h3261 Å« ¹«±â (°£ÆÇ)À» µé°í º®¿¡ ¹Ğ°í ³»·ÁÄ§
		MONZETSU,                   //h11250 µé¾î´Ù°¡ ¹«¸­À¸·Î ¶Ë²¿ÂïÀ½ (µÚ¿¡¼­ Àâ±âÇßÀ»¶§ »ç¿ë)

		WALL_KNOCK_NECK_C,          //h2011 º®¿¡ ¸Ó¸®¹Ú°ÔÇÏ°í ¹âÀ½ (¾Æ¸¶ Àâ±â ÀÌÈÄ È÷Æ®¾×¼ÇÇßÀ» ¶§ ±ÙÃ³¿¡ º®ÀÌÀÖ´Ù¸é)
		KABE_AIRON,                 //h23000 º®À¸·Î ¹ĞÄ¡°í ¶§¸²
		OI_KICK,                    //h23010 ¸Ó¸®Ã¤ Àâ°í µé¾î¼­ ¹ß·ÎÂü (¾şµå¸° »óÅÂ)
		OI_UPPER,                   //h23020 ¸Ó¸®Ã¤Àâ°í µé¾î¼­ ÁÖ¸ÔÀ¸·Î Ä§ (´©¿öÀÖ´Â »óÅÂ)
	*/
	/* ºÒÇÑ´ç */
	m_CutSceneAnimation.emplace(FINISHBLOW, "h23320");
	m_CutSceneAnimation.emplace(GOUGEKI_C, "h20021");
	m_CutSceneAnimation.emplace(HAIHEKI_KICK, "h23070");

	/* ·¯½¬ */
	m_CutSceneAnimation.emplace(KOBUSHIKUDAKI, "h10111");
	m_CutSceneAnimation.emplace(HAIHEKI_PUNCH, "h23060");
	m_CutSceneAnimation.emplace(OI_TRAMPLE_AO, "h1500");
	m_CutSceneAnimation.emplace(OI_KICKOVER_UTU_C, "h1511");

	/* ÆÄ±«ÀÚ */
	m_CutSceneAnimation.emplace(KIRYU_GSWING, "h1010");
	m_CutSceneAnimation.emplace(DORYU_MIN, "h11285");
	m_CutSceneAnimation.emplace(LAPEL_OIUCHI_NECK, "h1530");
	m_CutSceneAnimation.emplace(NAGE_OIUCHI_NECK, "h1540");
	m_CutSceneAnimation.emplace(POLE_KNOCK_LAPEL, "h2040");
	m_CutSceneAnimation.emplace(DORAMUKAN_88, "h3261");
	m_CutSceneAnimation.emplace(MONZETSU, "h11250");

	/* °øÅë */
	m_CutSceneAnimation.emplace(WALL_KNOCK_NECK_C, "h2011");
	m_CutSceneAnimation.emplace(KABE_AIRON, "h23000");
	m_CutSceneAnimation.emplace(OI_KICK, "h23010");
	m_CutSceneAnimation.emplace(OI_UPPER, "h23020");
}

// ÇöÀç ¾Ö´Ï¸ŞÀÌ¼ÇÀÇ yÃàÀ» Á¦°ÅÇÏ°í »ç¿ëÇÏ´Â »óÅÂÀÌ´Ù (È¤½Ã ¾Ö´Ï¸ŞÀÌ¼ÇÀÇ yÃà ÀÌµ¿µµ Àû¿ëÀÌ ÇÊ¿äÇÏ´Ù¸é ·ÎÁ÷ ¼öÁ¤ÀÌ ÇÊ¿äÇÔ
void CPlayer::Synchronize_Root(const _float& fTimeDelta)
{
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

	// m_pModelCom->Get_AnimChanged()  ¼±Çüº¸°£ÀÌ ³¡³µ´ÂÁö
	// m_pModelCom->Get_AnimLerp() ¼±Çüº¸°£ÀÌ ÇÊ¿äÇÑ ¾Ö´Ï¸ŞÀÌ¼ÇÀÎÁö
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
			
			//Y°ª ÀÌµ¿À» Á×ÀÎ ¹æÇâÀ¸·Î Àû¿ëÇØ¾ßÇÑ´Ù.
			_vector vTemp = XMVector3Normalize((vFF - XMLoadFloat4(&m_vPrevMove)));
			//Z°¡ YÃ³·³ ¾²ÀÓ
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
		// ¼±Çüº¸°£ÁßÀÏ¶§´Â ¹«Á¶°Ç ÃÊ±âÈ­
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
		if (fTheta < 90)  // ¾ÕÂÊ
			iDirection = 0;
		else // µŞÂÊ
			iDirection = 1;
	}
	else
	{
		if (fTheta < 90) // ¿ŞÂÊ
			iDirection = 2;
		else // ¿À¸¥ÂÊ
			iDirection = 3;
	}

	return iDirection;
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
			// HIT»óÅÂÀÏ ¶§ÀÇ ¾Ö´Ï¸ŞÀÌ¼ÇÀÌ ³¡³µ´Ù¸é
			string strAnimName = m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationName();
			// ´Ù¿î´çÇÑ »óÅÂÀÎÁö¸¦ °Ë»çÇØ¼­
			if (string::npos != strAnimName.find("dwn"))
			{
				// ±âÁ¸ Çàµ¿ ÃÊ±âÈ­ÇØÁÖ°í
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				// ´Ù¿î »óÅÂ·Î º¯°æÇØÁØ´Ù.
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::DOWN;

				// ¾Æ·¡ ¹®±¸°¡ Æ÷ÇÔµÈ ¾Ö´Ï¸ŞÀÌ¼ÇµéÀº ¾ş¾îÁø»óÅÂ·Î ÀÌ¾îÁø´Ù
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
				// ÇöÀç »óÅÂ°¡ ¾ÆÀÌµéÀÌ ¾Æ´Ï¶ó¸é 
				if ((_uint)KRS_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::IDLE;
			}
		}
		else
		{
			// ÇöÀç »óÅÂ°¡ ¾ÆÀÌµéÀÌ ¾Æ´Ï¶ó¸é 
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
			// ±âÁ¸ Çàµ¿À» ÃÊ±âÈ­ÇÏ°í ¾îÅÃÀ¸·Î ¹Ù²ãÁØ´Ù.
			if(m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::ATTACK)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::ATTACK;
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
		}
		if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
		{
			// ÇöÀç ¾îÅÃ»óÅÂÀÎÁö¸¦ ±¸ºĞÇØ¼­ ¸¶¹«¸® ¾×¼ÇÀ» ½ÇÇà½ÃÅ°°Å³ª
			// ±×¿¡ ¸Â´Â Ä¿¸Çµå ¾×¼ÇÀ» ½ÇÇà½Ã¤ÅÄÑ¾ß ÇÑ´Ù.

			// ¿©±â¿¡ ½ºÅ³Æ®¸®°¡ ¿Ï·áµÇ¸é ½ºÅ³À» º¸À¯ÁßÀÎÁö¿¡ ´ëÇÑ Á¶°Ç½ÄÀ» Ãß°¡·Î Àâ¾Æ¾ßÇÑ´Ù
			if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN)
			{
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::SKILL_FLY_KICK;
			}
			// ±âº» ·¯½¬ÄŞº¸ ÁøÇàÁßÀÏ ¶§¿¡ ¿ìÅ¬¸¯ÀÌ µé¾î¿À¸é ÇÇ´Ï½Ã ºí·Î ½ÇÇà
			else if(m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::ATTACK)
			{
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count(true);
			}
			// ¾Æ¹«°Íµµ ¾Æ´Ñ »óÅÂ¿¡¼­ ¿ìÅ¬¸¯ÀÌ µé¾î¿Â´Ù¸é Å±ÄŞº¸¸¦ ½ÇÇà
			else
			{
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::SKILL_KICK_COMBO;
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
			}

		}

		// ¾îÅÃÁßÀÌ ¾Æ´Ò¶§¿¡¸¸ QÀÔ·ÂÀ» ¹Ş´Â´Ù
		if (m_pGameInstance->GetKeyState(DIK_Q) == TAP 
			&&	m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::ATTACK && m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::GRAB 
			&& m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::PICK_UP)
		{
			// ¾ÆÀÌÅÛ Å¸°ÙÆÃ ¾ÈµÇ¾îÀÖÀ» ¶§ GrabÀ¸·Î ºüÁö°í
			if (m_pTargetItem == nullptr)
			{
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::GRAB;
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
			}
			// ¾ÆÀÌÅÛ Å¸°ÙÆÃ µÇ¾îÀÖÀ» ¶§´Â PickUpÀ¸·Î ºüÁø´Ù.
			else
			{
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::PICK_UP;
<<<<<<< HEAD
				// ¿©±â¿¡ »óÅÂ°ª ÇÈ¾÷»óÅÂ Ã³¸®ÇØÁà¾ßÇÔ
=======
>>>>>>> ?€ê²©ê° ?¡ê¸°
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
			// ¿òÁ÷ÀÓ °ü·Ã Å° ÀÔ·ÂÀÌ ¾øÀ» ¶§¿¡´Â ¹«Á¶°Ç Back¹æÇâÀ¸·Î ÀÌµ¿ÇØ¾ßÇÏ±â ¶§¹®¿¡ Å°ÀÔ·Â¿©ºÎ Ã¼Å©ÇØ¼­ ¹æÇâ ÃÊ±âÈ­
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
			// HIT»óÅÂÀÏ ¶§ÀÇ ¾Ö´Ï¸ŞÀÌ¼ÇÀÌ ³¡³µ´Ù¸é
			string strAnimName = m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationName();
			// ´Ù¿î´çÇÑ »óÅÂÀÎÁö¸¦ °Ë»çÇØ¼­
			if (string::npos != strAnimName.find("dwn"))
			{
				// ±âÁ¸ Çàµ¿ ÃÊ±âÈ­ÇØÁÖ°í
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				// ´Ù¿î »óÅÂ·Î º¯°æÇØÁØ´Ù.
				m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::DOWN;

				// ¾Æ·¡ ¹®±¸°¡ Æ÷ÇÔµÈ ¾Ö´Ï¸ŞÀÌ¼ÇµéÀº ¾ş¾îÁø»óÅÂ·Î ÀÌ¾îÁø´Ù
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
				// ÇöÀç »óÅÂ°¡ ¾ÆÀÌµéÀÌ ¾Æ´Ï¶ó¸é 
				if ((_uint)KRH_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::IDLE;
			}
		}
		else
		{
			// ÇöÀç »óÅÂ°¡ ¾ÆÀÌµéÀÌ ¾Æ´Ï¶ó¸é 
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
			// ±âÁ¸ Çàµ¿À» ÃÊ±âÈ­ÇÏ°í ¾îÅÃÀ¸·Î ¹Ù²ãÁØ´Ù.
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
			else
			{
				m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::ATTACK;

				_bool isPunch = true;
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value(&isPunch);
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
			}
		}
	}


	if (m_iCurrentBehavior < (_uint)KRH_BEHAVIOR_STATE::ATTACK && !m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stopping())
	{
		// °È±â»óÅÂÀÏ ¶§¿¡´Â ¾Ö´Ï¸ŞÀÌ¼Ç¿¡¼­ µû·Î Å°ÀÔ·ÂÀ» ¹Ş¾Æ¼­ »ç¿ëÇÑ´Ù
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
		// ½º¿şÀÌ°¡ ÄŞº¸ÀÔ·ÂÀ¸·Î µé¾î°¡±â ¶§¹®¿¡, ÀÌ¹Ì ÀÔ·ÂÇÑ ÀÌÈÄ¿¡ µé¾î¿Â ÀÔ·ÂÀÌ¶ó¸é ±âÁ¸ ¹æÇâÀ» À¯ÁöÇØ¾ßÇÏ¹Ç·Î ÃÊ±âÈ­ÇÏÁö¾ÊÀ½
		if (m_iCurrentBehavior != (_uint)KRH_BEHAVIOR_STATE::SWAY)
		{
			// ¿òÁ÷ÀÓ °ü·Ã Å° ÀÔ·ÂÀÌ ¾øÀ» ¶§¿¡´Â ¹«Á¶°Ç Back¹æÇâÀ¸·Î ÀÌµ¿ÇØ¾ßÇÏ±â ¶§¹®¿¡ Å°ÀÔ·Â¿©ºÎ Ã¼Å©ÇØ¼­ ¹æÇâ ÃÊ±âÈ­
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
		if (!m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stopping() && m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::GRAB)
		{
			if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
			{
				// ±âÁ¸ Çàµ¿À» ÃÊ±âÈ­ÇÏ°í ¾îÅÃÀ¸·Î ¹Ù²ãÁØ´Ù.
				if (m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::ATTACK)
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

				m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::ATTACK;
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
			}
			if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
			{
				// ÇöÀç ¾îÅÃ»óÅÂÀÎÁö¸¦ ±¸ºĞÇØ¼­ ¸¶¹«¸® ¾×¼ÇÀ» ½ÇÇà½ÃÅ°°Å³ª
				// ±×¿¡ ¸Â´Â Ä¿¸Çµå ¾×¼ÇÀ» ½ÇÇà½ÃÄÑ¾ß ÇÑ´Ù.
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

			// ¾îÅÃÁßÀÌ ¾Æ´Ò¶§¿¡¸¸ QÀÔ·ÂÀ» ¹Ş´Â´Ù
			if (m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::ATTACK && m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::GRAB && m_pGameInstance->GetKeyState(DIK_Q) == TAP)
			{
				m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::GRAB;
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
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
				// ¿òÁ÷ÀÓ °ü·Ã Å° ÀÔ·ÂÀÌ ¾øÀ» ¶§¿¡´Â ¹«Á¶°Ç Back¹æÇâÀ¸·Î ÀÌµ¿ÇØ¾ßÇÏ±â ¶§¹®¿¡ Å°ÀÔ·Â¿©ºÎ Ã¼Å©ÇØ¼­ ¹æÇâ ÃÊ±âÈ­
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
	if (m_pModelCom->Set_AnimationIndex(iIndex, fInterval))
	{
		// ½ÇÁ¦·Î ¾Ö´Ï¸ŞÀÌ¼Ç Ã¼ÀÎÁö°¡ ÀÏ¾î³µÀ» ¶§ ÄÑÁ®ÀÖ´ø ¾îÅÃ Äİ¶óÀÌ´õ¸¦ ÀüºÎ ²ö´Ù
		Off_Attack_Colliders();
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
		m_fPrevSpeed = 0.f;
	}
	
	string strAnimName = string(m_pModelCom->Get_AnimationName(iIndex));
	strAnimName = m_pGameInstance->Extract_String(strAnimName, '[', ']');
	m_pData->Set_CurrentAnimation(strAnimName);
}

void CPlayer::Change_ResetAnimaition(_uint iIndex, _float fInterval)
{
	m_pModelCom->Reset_Animation(iIndex);

	if (m_pModelCom->Set_AnimationIndex(iIndex, fInterval))
	{
		// ½ÇÁ¦·Î ¾Ö´Ï¸ŞÀÌ¼Ç Ã¼ÀÎÁö°¡ ÀÏ¾î³µÀ» ¶§ ÄÑÁ®ÀÖ´ø ¾îÅÃ Äİ¶óÀÌ´õ¸¦ ÀüºÎ ²ö´Ù
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
	// ¼³Á¤ÇÑ ½ºÅ¸ÀÏÀÇ Ã¹¹øÂ° ¾×¼ÇÀ» ½ÇÇà½ÃÅ²´Ù (¹èÆ²¸ğµåµéÀº ¹«Á¶°Ç Ã¹¹øÂ°¿¡ ¹èÆ² ½ÃÀÛ ¾×¼ÇÀ» µÑ ¿¹Á¤)
	m_isAuraOn = false;
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

void CPlayer::Set_CutSceneAnim(CUTSCENE_ANIMATION_TYPE eType, _uint iFaceAnimIndex)
{
	// ¾øÀ¸¸é Á¾·á
	auto iter = m_CutSceneAnimation.find(eType);
	if (m_CutSceneAnimation.end() == iter) return;

	m_iFaceAnimIndex = iFaceAnimIndex;

	On_Separation_Face();			// ¾ó±¼ ¾Ö´Ï¸ŞÀÌ¼Ç ÄÑ±â
	Off_Separation_Hand();			// ¼Õ ºĞ¸® ¾Ö´Ï¸ŞÀÌ¼Ç ²ô±â

	string AnimName = (*iter).second;

	_uint i = 0;
	auto AnimList = m_pAnimCom->Get_Animations();
	for (auto& pAnimation : AnimList)
	{
		string ExtractName = m_pGameInstance->Extract_String(pAnimation->Get_AnimName(), '[', ']');

		// ¹®ÀÚ¿­ Æ÷ÇÔÁßÀÌ¶ó¸é ÀÎµ¦½º ÀúÀå ÈÄ ¹İº¹¹® Á¾·á
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

		// ¾Ö´Ï¸ŞÀÌ¼Ç ÀÌ¸§¿¡ .cmt°¡ Æ÷ÇÔµÈ °æ¿ì¸¸ Ä«¸Ş¶ó ¾Ö´Ï¸ŞÀÌ¼ÇÀÌ´Ù.
		if (CameraAnimName.find(".cmt") != std::string::npos && CameraAnimName.find(AnimName) != std::string::npos)
		{
			m_iCutSceneCamAnimIndex = j;
			break;
		}
		j++;
	}

	Reset_CutSceneEvent();
}

void CPlayer::Play_CutScene()
{
	if (CUTSCENE == m_eAnimComType)
	{
		// Ä«¸Ş¶ó ¸ğµ¨ÀÇ ¾Ö´Ï¸ŞÀÌ¼ÇÀÌ Á¾·áµÇ¸é ¶È°°ÀÌ ÇÃ·¹ÀÌ¾îÀÇ ¾Ö´Ï¸ŞÀÌ¼Çµµ Á¾·áµÈ °ÍÀÌ±â ¶§¹®¿¡ ±âÁ¸»óÅÂ·Î µÇµ¹¸°´Ù.
		if (m_pCameraModel->Get_AnimFinished())
		{
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			m_iCurrentBehavior = 1;				// Idle»óÅÂ·Î µÇµ¹·ÁµĞ´Ù
			Reset_CutSceneEvent();

			Off_Separation_Face();				// ÄÆ½Å Á¾·á ÈÄ ¾ó±¼ ¾Ö´Ï¸ŞÀÌ¼Ç Á¾·á
			On_Separation_Hand();				// ÄÆ½Å Á¾·á ÈÄ ¼Õ ¾Ö´Ï¸ŞÀÌ¼Ç ÄÑ±â

			return;
		}

		m_pModelCom->Play_Animation_CutScene(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_pAnimCom, false, m_iCutSceneAnimIndex, false);

		CPlayer* pPlayer = this;
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CUTSCENE));
		// Blender¿¡¼­ ¾òÀº º»ÀÇ º¯È¯ Çà·Ä
		_matrix matBoneMatrix = XMLoadFloat4x4(m_pCameraModel->Get_BoneCombinedTransformationMatrix("Camera"));

		// ÇÃ·¹ÀÌ¾îÀÇ ¿ùµå º¯È¯ Çà·Ä
		//_matrix matPlayerWorld = pPlayer->Get_TransformCom()->Get_WorldMatrix();
		_matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
		_matrix matVectorBoneWorld = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix("vector_c_n"));

		// BlenderÀÇ ÁÂÇ¥°è¸¦ DirectXÀÇ ÁÂÇ¥°è·Î º¯È¯ÇÏ±â À§ÇÑ È¸Àü Çà·Ä
		_matrix rotationMatrixX = XMMatrixRotationX(XMConvertToRadians(90));
		_matrix rotationMatrixY = XMMatrixRotationY(XMConvertToRadians(-180));
		_matrix rotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(90));

		// BlenderÀÇ º» º¯È¯ Çà·Ä°ú ÇÃ·¹ÀÌ¾îÀÇ ¿ùµå º¯È¯ Çà·ÄÀ» °áÇÕÇÏ°í ÁÂÇ¥°è º¯È¯À» Àû¿ë
		_matrix finalMat = rotationMatrixX * rotationMatrixY * rotationMatrixZ * matVectorBoneWorld * matBoneMatrix * matPlayerWorld;

		// ÃÖÁ¾ ºä Çà·ÄÀ» °è»ê
		_matrix viewMatrix = XMMatrixInverse(nullptr, finalMat);

		// ºä Çà·ÄÀ» ÆÄÀÌÇÁ¶óÀÎ¿¡ ¼³Á¤
		m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, viewMatrix);

		auto KeyFrames = m_pCameraModel->Get_CurrentKeyFrameIndices(m_iCutSceneCamAnimIndex);
		_uint iKeyFrameIndex = KeyFrames->front();


		pCamera->Set_FoV(m_pCameraModel->Get_FoV(m_pCameraModel->Get_AnimationName(m_iCutSceneCamAnimIndex), iKeyFrameIndex));

		CModel::ANIMATION_DESC Desc{ m_iCutSceneCamAnimIndex, false };
		m_pCameraModel->Set_AnimationIndex(Desc);

		// Ä«¸Ş¶ó º» ¾Ö´Ï¸ŞÀÌ¼Ç ½ÇÇà
		m_pCameraModel->Play_Animation_CutScene(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), nullptr, false, m_iCutSceneCamAnimIndex, false, "Camera");
	}
}

void CPlayer::Reset_CutSceneEvent()
{
	// ÀÌ ¶§ ½ÇÇàÇÏ´Â ¾Ö´Ï¸ŞÀÌ¼ÇµéÀº ¼±Çüº¸°£À» ÇÏÁö ¾Ê´Â ¾Ö´Ï¸ŞÀÌ¼ÇÀÌ¹Ç·Î ¼±Çüº¸°£ °£°İÀ» 0À¸·Î ²À!! ÃÊ±âÈ­ÇØ¾ßÇÑ´Ù.
	m_pModelCom->Set_ChangeInterval(0.0);
	m_pCameraModel->Set_ChangeInterval(0.0);
	m_pAnimCom->Reset_Animation(m_iCutSceneAnimIndex);
	m_pCameraModel->Reset_Animation(m_iCutSceneCamAnimIndex);

	m_eAnimComType = (m_eAnimComType == DEFAULT ? CUTSCENE : DEFAULT);

	CAMERA eCurrentCam = m_pSystemManager->Get_Camera();

	switch (eCurrentCam)
	{
		// ÇöÀç ÇÃ·¹ÀÌ¾î Ä«¸Ş¶óÀÌ¸ç, ÄÆ½ÅÀ¸·Î µ¹¸®´Â »óÈ²
	case Client::CAMERA_PLAYER:
	{
		CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
		// ÇÃ·¹ÀÌ¾î Ä«¸Ş¶óÀÇ ÇöÀç »óÅÂ¸¦ ÀúÀåÇÑ´Ù.
		pCamera->Store_PrevMatrix();
		break;
	}

	// ÇöÀç ÄÆ½Å Ä«¸Ş¶óÀÌ¸ç, ÇÃ·¹ÀÌ¾î Ä«¸Ş¶ó·Î µ¹¸®´Â »óÈ²
	case Client::CAMERA_CUTSCENE:
		// ÇöÀç ÄÆ½ÅÄ«¸Ş¶óÀÇ ¸¶Áö¸· Çà·Ä°ú Fov¸¦ ¹Ş¾Æ¿Í¼­
		CCutSceneCamera* pCutSceneCamera = dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CUTSCENE));
		_matrix LastMatrix = XMLoadFloat4x4(pCutSceneCamera->Get_WorldMatrix());
		_float fLastFov = pCutSceneCamera->Get_Fov();

		// ÇÃ·¹ÀÌ¾î Ä«¸Ş¶ó¿¡ ÇØ´ç Á¤º¸¸¦ ¸ğµÎ ÀúÀåÇØÁØ´Ù.
		CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
		pCamera->Store_StartMatrix(LastMatrix);
		pCamera->Set_StartFov(fLastFov);		//¼±Çüº¸°£ÇÒ ¶§ ½ÃÀÛ°ª fov ¼³Á¤
		pCamera->Set_FoV(fLastFov);				//ÇöÀç fov¼³Á¤
		pCamera->On_Return();
		break;
	}

	// ±×¸®°í Ã¼ÀÎÁö
	m_pSystemManager->Set_Camera(CAMERA_CUTSCENE == m_pSystemManager->Get_Camera() ? CAMERA_PLAYER : CAMERA_CUTSCENE);
}

void CPlayer::Compute_MoveDirection_FB()
{
	_vector vCamLook = m_pGameInstance->Get_CamLook();
	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	
	// ¹æÇâº¤ÅÍ°£ ³»ÀûÀÇ °á°ú°ªÀÌ ¾ç¼ö¸é 90µµ¹Ì¸¸(°°Àº¹æÇâ), À½¼ö¸é 90µµ ÃÊ°ú(¹İ´ë¹æÇâ), 0ÀÌ¸é Á÷±³ÇÑ´Ù.
	
	// Áö±İ ÀÔ·ÂÇÑ Å°°¡ ¾ÕÀÌ¸é ¾ÕÀ¸·Î ½º¿şÀÌÇÒ°Çµ¥
	if (m_InputDirection[B])
	{
		// ÀÌ ¶§ Ä³¸¯ÅÍ°¡ Ä«¸Ş¶ó¶û °°Àº ¹æÇâÀ» º¸°íÀÖ´Ù¸é Front ½º¿şÀÌ
		if (XMVectorGetX(XMVector3Dot(vCamLook, vPlayerLook)) < 0)
			m_MoveDirection[F] = true;
		// Ä³¸¯ÅÍ°¡ Ä«¸Ş¶ó¶û ¹İ´ë¹æÇâÀ» º¸°í ÀÖ´Ù¸é Back ½º¿şÀÌ
		else
			m_MoveDirection[B] = true;
	}
	// Áö±İ ÀÔ·ÂÇÑ Å°°¡ µÚ¸é µÚ·Î ½º¿şÀÌÇÒ°Çµ¥, 
	if (m_InputDirection[F])
	{
		//Ä³¸¯ÅÍ°¡ Ä«¸Ş¶ó¶û °°Àº ¹æÇâÀ» º¸°íÀÖ´Ù¸é Back¹æÇâ ½º¿şÀÌ
		if (XMVectorGetX(XMVector3Dot(vCamLook, vPlayerLook)) < 0)
			m_MoveDirection[B] = true;
		// Ä³¸¯ÅÍ°¡ Ä«¸Ş¶ó¶û ¹İ´ë¹æÇâÀ» º¸°íÀÖ´Ù¸é Front ½º¿şÀÌ
		else
			m_MoveDirection[F] = true;
	}
}

void CPlayer::Compute_MoveDirection_RL()
{
	_vector vCamRight = m_pGameInstance->Get_CamRight();
	_vector vPlayerRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	// Áö±İ ÀÔ·ÂÇÑ Å°°¡ ¿À¸¥ÂÊ
	if (m_InputDirection[L])
	{
		// ¿À¸¥ÂÊÀ¸·Î ½º¿şÀÌÇÒ°Çµ¥, Ä³¸¯ÅÍÀÇ ¿À¸¥ÂÊÀÌ Ä«¸Ş¶óÀÇ ¿À¸¥ÂÊ°ú °°Àº ¹æÇâÀÌ¶ó¸é ±×³É ¿À¸¥ÂÊ ½º¿şÀÌ¸ğ¼Ç
		if (XMVectorGetX(XMVector3Dot(vCamRight, vPlayerRight)) < 0)
			m_MoveDirection[R] = true;
		// Ä³¸¯ÅÍÀÇ ¿À¸¥ÂÊÀÌ Ä«¸Ş¶óÀÇ ¿À¸¥ÂÊ°ú ¹İ´ë ¹æÇâÀÌ¶ó¸é ¿ŞÂÊ ½º¿şÀÌ¸ğ¼Ç
		else if (XMVectorGetX(XMVector3Dot(vCamRight, vPlayerRight)) > 0)
			m_MoveDirection[L] = true;

	}
	// Áö±İ ÀÔ·ÂÇÑ Å°°¡ µÚ¸é µÚ·Î ½º¿şÀÌÇÒ°Çµ¥, 
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
	CSocketEffect* pHooligan = { nullptr };
	CSocketEffect* pRush = { nullptr };
	CSocketEffect* pDestroyer = { nullptr };

	for (auto& pair : m_pEffects)
	{
		string strKey = m_pGameInstance->WstringToString(pair.second->Get_EffectName());

		regex hooliganPattern("Hooligan");
		regex rushPattern("Rush");
		regex destroyerPattern("Destroyer");

		if (regex_search(strKey, hooliganPattern))
			pHooligan = pair.second;
		if (regex_search(strKey, rushPattern))
			pRush = pair.second;
		if (regex_search(strKey, destroyerPattern))
			pDestroyer = pair.second;
	}

	if (0 < m_iCurrentHitLevel)
	{
		switch (m_eCurrentStyle)
		{
		case Client::CPlayer::KRS:
			// ±âÁ¸¿¡ ÄÑÁ®ÀÖ´ø ¿À¶ó¸¦ ²ö´Ù
			if (nullptr != pRush)
				pRush->Off();
			if (nullptr != pDestroyer)
				pDestroyer->Off();

			// ÇöÀç ½ºÅ¸ÀÏ¿¡ ¸Â´Â ¿À¶ó¸¦ ÄÒ´Ù
			if (nullptr != pHooligan)
				pHooligan->On();


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
		case Client::CPlayer::KRH:
			if (nullptr != pHooligan)
				pHooligan->Off();
			if (nullptr != pDestroyer)
				pDestroyer->Off();

			if (nullptr != pRush)
				pRush->On();

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
		case Client::CPlayer::KRC:
			if (nullptr != pHooligan)
				pHooligan->Off();
			if (nullptr != pRush)
				pRush->Off();

			if (nullptr != pDestroyer)
				pDestroyer->On();

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
		default:
			if (nullptr != pHooligan)
				pHooligan->Off();
			if (nullptr != pRush)
				pRush->Off();
			if (nullptr != pDestroyer)
				pDestroyer->Off();
			return;
		}
	}
	else
	{
		if (nullptr != pHooligan)
			pHooligan->Off();
		if (nullptr != pRush)
			pRush->Off();
		if (nullptr != pDestroyer)
			pDestroyer->Off();
	}
}

void CPlayer::Setting_Target_Enemy()
{
	if (2 == m_iCurrentBehavior) return; 
	auto pMonsters = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Monster"));
	
	if (nullptr != m_pTargetObject)
	{
		_float vDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));
		
		// ±âÁ¸ Å¸°ÙÁßÀÌ´ø Ä£±¸ÀÇ °Å¸®°¡ 3.f ÀÌ»ó ¸Ö¾îÁö¸é ±×¶§ ´Ù½Ã Å¸°ÙÆÃÇÑ´Ù.
		if(3.f < vDistance)
			m_pTargetObject = static_cast<CLandObject*>(m_pCollisionManager->Get_Near_Object(this, pMonsters));
	}
	else
		m_pTargetObject = static_cast<CLandObject*>(m_pCollisionManager->Get_Near_Object(this, pMonsters));
}

void CPlayer::Setting_Target_Item()
{
	if (9 == m_iCurrentBehavior) return;

	auto pItemList = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Item"));

	m_pTargetItem = static_cast<CItem*>(m_pCollisionManager->Get_Near_Object(this, pItemList, 1.5f));
}

void CPlayer::AccHitGauge()
{
	//if (PLAYER_HITGAUGE_LEVEL_INTERVAL * 3.f < m_fHitGauge)
	//	m_fHitGauge = PLAYER_HITGAUGE_LEVEL_INTERVAL * 3.f;
	//else
	//	m_fHitGauge += 5.f;

	//m_iCurrentHitLevel = (m_fHitGauge / PLAYER_HITGAUGE_LEVEL_INTERVAL);
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

	Safe_Release(m_pTargetObject);

	for (size_t i = 0; i < BATTLE_STYLE_END; i++)
	{
		for (auto& pair : m_AnimationTree[i])
			Safe_Release(pair.second);

		m_AnimationTree[i].clear();
	}
	

#ifdef _DEBUG
	Safe_Release(m_pDebugManager);
#endif // _DEBUG

	Safe_Release(m_pUIManager);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pAnimCom);
	Safe_Release(m_pCameraModel);

	for (size_t i = 0; i < BATTLE_STYLE_END; i++)
	{
		for (auto& pair : m_AnimationTree[i])
			Safe_Release(pair.second);

		m_AnimationTree[i].clear();
	}
}