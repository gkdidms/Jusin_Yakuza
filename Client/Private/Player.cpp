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

#include "Monster.h"

#pragma region 행동 관련 헤더들
#include "Kiryu_KRC_Hit.h"
#include "Kiryu_KRH_Hit.h"
#include "Kiryu_KRS_Hit.h"
#include "Kiryu_KRS_Down.h"
#include "Kiryu_KRH_Down.h"
#include "Kiryu_KRS_Grab.h"
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
	//8번 Grab 공통
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

	// 기본 몬스터: 20
	// 삥쟁이: 30
	// 쿠제: 100
	m_Info.iMaxHP = 150.f;
	m_Info.iHp = m_Info.iMaxHP;

	ZeroMemory(&m_MoveDirection, sizeof(_bool) * MOVE_DIRECTION_END);
	ZeroMemory(&m_InputDirection, sizeof(_bool) * MOVE_DIRECTION_END);

	// 새로 생성할 때 마다 UI매니저에 본인을 Set해준다.
	m_pUIManager->Set_Player(this);

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

	// 배틀 시작 애니메이션 아닐 경우 타임델타를 1로 고정시켜준다.
	// TODO: 다른곳에서 시간조절이 필요하다면 수정해야한다
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
	//메뉴 창 추가
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
		//m_pUIManager->Open_Scene(TEXT("Talk"));
		//m_pUIManager->Start_Talk(TEXT("키류"), TEXT("뿡밥바아아야아앙바아빠ㅃ우 \\뿡뿡뿡 \\빠아앙"));
		
	}


	Synchronize_Root(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

#ifdef _DEBUG
	if (m_pGameInstance->GetKeyState(DIK_Z) == TAP)
	{
		//TODO: 여기에서 enum값을 필요한 애니메이션으로 바꾸면 해당하는 컷신이 실행된당
		if (m_pTargetObject)
		{
			Set_CutSceneAnim(m_eCutSceneType, 1);
			static_cast<CMonster*>(Get_TargetObject())->Set_Sync(m_CutSceneAnimation[m_eCutSceneType]);
		}
		
	}

	// 뼈 분리 테스트
	if (m_pGameInstance->GetKeyState(DIK_N) == TAP)
		m_iFaceAnimIndex--;
	if (m_pGameInstance->GetKeyState(DIK_M) == TAP)
		m_iFaceAnimIndex++;

	if (m_pGameInstance->GetKeyState(DIK_C) == TAP)
	{
		m_pModelCom->Set_Separation_ParentBone("ude3_l_n", -1);
		m_pModelCom->Set_Separation_ParentBone("ude3_r_n", -1);

		m_pModelCom->Set_Separation_ParentBone("face_c_n", -1);
	}
	if (m_pGameInstance->GetKeyState(DIK_X) == TAP)
	{
		m_pModelCom->Set_Separation_ParentBone("ude3_l_n", (_int)HAND_COM);
		m_pModelCom->Set_Separation_ParentBone("ude3_r_n", (_int)HAND_COM);

		m_pModelCom->Set_Separation_ParentBone("face_c_n", (_int)FACE_COM);
		m_pModelCom->Set_Separation_SingleBone("face_c_n", -1);
	}

	if (m_isAnimStart)
	{
		if (DEFAULT == m_eAnimComType)
		{
			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iHandAnimIndex, m_SeparationAnimComs[HAND_COM], false, (_int)HAND_COM);

			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iFaceAnimIndex, m_SeparationAnimComs[FACE_COM], false, (_int)FACE_COM);
			m_pModelCom->Play_Animation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));
		}
		else
		{
			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iHandAnimIndex, m_SeparationAnimComs[HAND_COM], false, (_int)HAND_COM);

			m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iFaceAnimIndex, m_SeparationAnimComs[FACE_COM], false, (_int)FACE_COM);
			Play_CutScene();
		}
	}
#else
	if (DEFAULT_ANIMAITION == m_eAnimComType)
		m_pModelCom->Play_Animation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));
	else
	{
		Play_CutScene();
	}
#endif // _DEBUG

	for (auto& pCollider : m_pColliders)
		pCollider.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pEffects)
		pEffect.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pTrailEffects)
		pEffect.second->Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));


	KeyInput(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	Animation_Event();
	RimLight_Event();
	Trail_Event();
	Effect_Control_Aura();
	Setting_Target_Enemy();
}

void CPlayer::Late_Tick(const _float& fTimeDelta)
{
	m_pCollisionManager->Add_ImpulseResolution(this);

#ifdef _DEBUG
	if (m_isObjectRender)
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow용 렌더 추가
	}
#else
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow용 렌더 추가
#endif // _DEBUG

	for (auto& pCollider : m_pColliders)
		pCollider.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pEffects)
		pEffect.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	for (auto& pEffect : m_pTrailEffects)
		pEffect.second->Late_Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	// 현재 켜져있는 Attack용 콜라이더 삽입
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::ATTACK && pPair.second->IsOn())
		{
			if (pPair.second->Get_CollierPartType() == 1)
				int a = 0;

			m_pCollisionManager->Add_AttackCollider(pPair.second, CCollision_Manager::PLAYER);
		}
	}

	// 현재 켜져있는 Hit용 콜라이더 삽입 (아직까지는 Hit용 콜라이더는 항상 켜져있음)
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::HIT && pPair.second->IsOn())
			m_pCollisionManager->Add_HitCollider(pPair.second, CCollision_Manager::PLAYER);
	}
	
	Setting_RimLight();
	Compute_Height();

	if (m_pModelCom->Get_AnimFinished())
	{
		// 애니메이셔ㅑㄴ이 끝났을 때 기존 애니메이션에서 회전한 값 만큼 회전시켜준다.

		//_float3 vScaled = m_pTransformCom->Get_Scaled();
		//_vector vScale = XMLoadFloat3(&vScaled);
		//_vector vOriginRot = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		//_vector vRot = XMLoadFloat4(&m_vPrevRotation);
		//_vector vTranslation = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		//_matrix CenterRotationMatrix = XMMatrixTranspose(XMMatrixAffineTransformation(vScale, vOriginRot, vRot, vTranslation));

		//// 저장하고 있는 쿼터니언의 회전값은 Y축 양의 방향을 Forward로 사용중인 회전값이므로,
		//// Z축 양의 방향을 forward로 가지게 변환해주어야 한다.
		////_matrix yFlipMatrix = XMMatrixRotationX(XMConvertToRadians(-90.0f));

		////// 쿼터니언 회전과 Y축 회전을 적용한 새로운 회전 행렬 생성
		////_matrix newRotationMatrix = XMMatrixMultiply(yFlipMatrix, CenterRotationMatrix);

		//// 기존의 월드 행렬에 새로운 회전 행렬을 적용
		//_matrix WorldMatrix = XMMatrixMultiply(CenterRotationMatrix, m_pTransformCom->Get_WorldMatrix());

		//XMVECTOR y90DegreeRotation = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(90.0f));
		//XMVECTOR transformedQuat = XMQuaternionMultiply(y90DegreeRotation, XMLoadFloat4(&m_vPrevRotation));

		//XMFLOAT3 euler;
		//XMFLOAT4 q;
		//XMStoreFloat4(&q, transformedQuat);

		//// 오일러 각도 계산 (Yaw, Pitch, Roll)
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
			// 전신일 때 임의로 Full을 저장해주고 사용한다.
			if (string_view(m_strRimMeshName) == string_view("Full"))
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimUV", &m_fRimPartsUV, sizeof(_float2))))
					return E_FAIL;
			}
			else if (string_view(m_strRimMeshName) == string_view(pMesh->Get_Name()))
			{
				_float2 fUV = m_fRimPartsUV;		// 기본적으로 파츠uv를 넣고
				if ("[l0]jacketw1" == m_strRimMeshName)
					fUV = m_fRimTopUV;				// 상체일 때 탑을 넣어준다.
				if("[l0]pants3" == m_strRimMeshName)
					fUV = m_fRimBotUV;				// 바지일 때 바텀을 넣어준다.

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

			// 기게이지가 켜져있는 상태라면 상반신 림라이트를 켠다
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
			// 어드벤처일때
			if (FAILED(Bind_RimLight()))
				return E_FAIL;
		}

		//옷 셰이더 구분용
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
		//if (!strcmp(pMesh->Get_Name(), "[l0]face_kiryu"))
		//{
		//	isRS = false;
		//	isRD = false;
		//}

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

// 내 공격 콜라이더와 충돌했을 때
void CPlayer::Attack_Event(CLandObject* pHitObject)
{
	switch (m_eCurrentStyle)
	{
	case CPlayer::KRS:
	{

		if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::GRAB)
		{
			CKiryu_KRS_Grab::KRS_Grab_DESC Desc{ true, Compute_Target_Direction(pHitObject) };
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Setting_Value(&Desc);
		}

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
}

void CPlayer::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
	// iHitColliderType 는 충돌한 HIT타입 콜라이더가 헤드, 바디, 레그인지를 갖고있다.

	// 데미지 감소율 설정 (파괴자 가드 시에는 딜을 평소보다 0.2배만큼만 받는다 (==딜감80퍼))
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
		else if(m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::SKILL_FLY_KICK)						// 슈퍼아머 적용할 행동 타입들을 예외처리해주어야한다.
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

	//데미지 처리하기
	if (!m_isObjectDead)
	{
		m_Info.iHp -= (fDamage * fDamageDownScale);
		if (m_Info.iHp <= 0.f)
			m_isObjectDead = true;
	}
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

void CPlayer::Ready_CutSceneAnimation()
{
	/*
		FINISHBLOW,                 //h23320 피니시 블로의 극
		GOUGEKI_C,                  // h20021 잡기 후 히트액션 (주먹으로 얼굴때리고 박치기)
		HAIHEKI_KICK,               //h23070 벽 등지고 무릎치는 스킬

		KOBUSHIKUDAKI,              //h10111 팔꿈치로 공격막는 스킬
		HAIHEKI_PUNCH,              //h23060 벽 등지고 나오는 스킬
		OI_TRAMPLE_AO,              //h1500 다운된 상대 얼굴 밟기 (누워있는 상태), 러쉬
		OI_KICKOVER_UTU_C,          //h1511 다운된 상대 얼굴 차기 (엎드려있는 상태), 러쉬

		KIRYU_GSWING,               //h1010 다리잡고 돌려서 스플릿 공격
		DORYU_MIN,                  //h11285 멱살잡고 돌려서 스플릿 공격
		NAGE_OIUCHI_NECK,           //h1540 들어서 바닥에 내던짐
		POLE_KNOCK_LAPEL,           //h2040 근처에 기둥이 있다면 기둥에 박게하고 밟음
		DORAMUKAN_88,               //h3261 큰 무기 (간판)을 들고 벽에 밀고 내려침
		MONZETSU,                   //h11250 들어다가 무릎으로 똥꼬찍음 (뒤에서 잡기했을때 사용)

		WALL_KNOCK_NECK_C,          //h2011 벽에 머리박게하고 밟음 (아마 잡기 이후 히트액션했을 때 근처에 벽이있다면)
		KABE_AIRON,                 //h23000 벽으로 밀치고 때림
		OI_KICK,                    //h23010 머리채 잡고 들어서 발로참 (엎드린 상태)
		OI_UPPER,                   //h23020 머리채잡고 들어서 주먹으로 침 (누워있는 상태)
	*/
	/* 불한당 */
	m_CutSceneAnimation.emplace(FINISHBLOW, "h23320");
	m_CutSceneAnimation.emplace(GOUGEKI_C, "h20021");
	m_CutSceneAnimation.emplace(HAIHEKI_KICK, "h23070");

	/* 러쉬 */
	m_CutSceneAnimation.emplace(KOBUSHIKUDAKI, "h10111");
	m_CutSceneAnimation.emplace(HAIHEKI_PUNCH, "h23060");
	m_CutSceneAnimation.emplace(OI_TRAMPLE_AO, "h1500");
	m_CutSceneAnimation.emplace(OI_KICKOVER_UTU_C, "h1511");

	/* 파괴자 */
	m_CutSceneAnimation.emplace(KIRYU_GSWING, "h1010");
	m_CutSceneAnimation.emplace(DORYU_MIN, "h11285");
	m_CutSceneAnimation.emplace(NAGE_OIUCHI_NECK, "h1540");
	m_CutSceneAnimation.emplace(POLE_KNOCK_LAPEL, "h2040");
	m_CutSceneAnimation.emplace(DORAMUKAN_88, "h3261");
	m_CutSceneAnimation.emplace(MONZETSU, "h11250");

	/* 공통 */
	m_CutSceneAnimation.emplace(WALL_KNOCK_NECK_C, "h2011");
	m_CutSceneAnimation.emplace(KABE_AIRON, "h23000");
	m_CutSceneAnimation.emplace(OI_KICK, "h23010");
	m_CutSceneAnimation.emplace(OI_UPPER, "h23020");
}

// 현재 애니메이션의 y축을 제거하고 사용하는 상태이다 (혹시 애니메이션의 y축 이동도 적용이 필요하다면 로직 수정이 필요함
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

	// m_pModelCom->Get_AnimChanged()  선형보간이 끝났는지
	// m_pModelCom->Get_AnimLerp() 선형보간이 필요한 애니메이션인지
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
			
			//Y값 이동을 죽인 방향으로 적용해야한다.
			_vector vTemp = XMVector3Normalize((vFF - XMLoadFloat4(&m_vPrevMove)));
			//Z가 Y처럼 쓰임
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
		// 선형보간중일때는 무조건 초기화
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
		if (fTheta < 90)  // 앞쪽
			iDirection = 0;
		else // 뒷쪽
			iDirection = 1;
	}
	else
	{
		if (fTheta < 90) // 왼쪽
			iDirection = 2;
		else // 오른쪽
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
		if ((_uint)KRS_BEHAVIOR_STATE::HIT == m_iCurrentBehavior)
		{
			// HIT상태일 때의 애니메이션이 끝났다면
			string strAnimName = m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationName();
			// 다운당한 상태인지를 검사해서
			if (string::npos != strAnimName.find("dwn"))
			{
				// 기존 행동 초기화해주고
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				// 다운 상태로 변경해준다.
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::DOWN;

				// 아래 문구가 포함된 애니메이션들은 엎어진상태로 이어진다
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
				// 현재 상태가 아이들이 아니라면 
				if ((_uint)KRS_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::IDLE;
			}
		}
		else
		{
			// 현재 상태가 아이들이 아니라면 
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

	if (!m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stopping())
	{
		if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
		{
			// 기존 행동을 초기화하고 어택으로 바꿔준다.
			if(m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::ATTACK)
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::ATTACK;
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
		}
		if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
		{
			// 현재 어택상태인지를 구분해서 마무리 액션을 실행시키거나
			// 그에 맞는 커맨드 액션을 실행시ㅕ켜야 한다.

			// 여기에 스킬트리가 완료되면 스킬을 보유중인지에 대한 조건식을 추가로 잡아야한다
			if (m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::RUN)
			{
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::SKILL_FLY_KICK;
			}
			// 기본 러쉬콤보 진행중일 때에 우클릭이 들어오면 피니시 블로 실행
			else if(m_iCurrentBehavior == (_uint)KRS_BEHAVIOR_STATE::ATTACK)
			{
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count(true);
			}
			// 아무것도 아닌 상태에서 우클릭이 들어온다면 킥콤보를 실행
			else
			{
				m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::SKILL_KICK_COMBO;
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
			}

		}

		// 어택중이 아닐때에만 Q입력을 받는다
		if (m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::ATTACK && m_iCurrentBehavior != (_uint)KRS_BEHAVIOR_STATE::GRAB && m_pGameInstance->GetKeyState(DIK_Q) == TAP)
		{
			m_iCurrentBehavior = (_uint)KRS_BEHAVIOR_STATE::GRAB;
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
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
			// 움직임 관련 키 입력이 없을 때에는 무조건 Back방향으로 이동해야하기 때문에 키입력여부 체크해서 방향 초기화
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
			// HIT상태일 때의 애니메이션이 끝났다면
			string strAnimName = m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationName();
			// 다운당한 상태인지를 검사해서
			if (string::npos != strAnimName.find("dwn"))
			{
				// 기존 행동 초기화해주고
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				// 다운 상태로 변경해준다.
				m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::DOWN;

				// 아래 문구가 포함된 애니메이션들은 엎어진상태로 이어진다
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
				// 현재 상태가 아이들이 아니라면 
				if ((_uint)KRH_BEHAVIOR_STATE::IDLE != m_iCurrentBehavior)
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();
				m_iCurrentBehavior = (_uint)KRH_BEHAVIOR_STATE::IDLE;
			}
		}
		else
		{
			// 현재 상태가 아이들이 아니라면 
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
			// 기존 행동을 초기화하고 어택으로 바꿔준다.
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
		// 걷기상태일 때에는 애니메이션에서 따로 키입력을 받아서 사용한다
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
		// 스웨이가 콤보입력으로 들어가기 때문에, 이미 입력한 이후에 들어온 입력이라면 기존 방향을 유지해야하므로 초기화하지않음
		if (m_iCurrentBehavior != (_uint)KRH_BEHAVIOR_STATE::SWAY)
		{
			// 움직임 관련 키 입력이 없을 때에는 무조건 Back방향으로 이동해야하기 때문에 키입력여부 체크해서 방향 초기화
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
		if (!m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stopping())
		{
			if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
			{
				// 기존 행동을 초기화하고 어택으로 바꿔준다.
				if (m_iCurrentBehavior != (_uint)KRC_BEHAVIOR_STATE::ATTACK)
					m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

				m_iCurrentBehavior = (_uint)KRC_BEHAVIOR_STATE::ATTACK;
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
				m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Combo_Count();
			}
			if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
			{
				// 현재 어택상태인지를 구분해서 마무리 액션을 실행시키거나
				// 그에 맞는 커맨드 액션을 실행시켜야 한다.
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
				// 움직임 관련 키 입력이 없을 때에는 무조건 Back방향으로 이동해야하기 때문에 키입력여부 체크해서 방향 초기화
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
		// 실제로 애니메이션 체인지가 일어났을 때 켜져있던 어택 콜라이더를 전부 끈다
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
		// 실제로 애니메이션 체인지가 일어났을 때 켜져있던 어택 콜라이더를 전부 끈다
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
	// 설정한 스타일의 첫번째 액션을 실행시킨다 (배틀모드들은 무조건 첫번째에 배틀 시작 액션을 둘 예정)
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
	// 없으면 종료
	auto iter = m_CutSceneAnimation.find(eType);
	if (m_CutSceneAnimation.end() == iter) return;

	m_iFaceAnimIndex = iFaceAnimIndex;

	On_Separation_Face();

	string AnimName = (*iter).second;

	_uint i = 0;
	auto AnimList = m_pAnimCom->Get_Animations();
	for (auto& pAnimation : AnimList)
	{
		string ExtractName = m_pGameInstance->Extract_String(pAnimation->Get_AnimName(), '[', ']');

		// 문자열 포함중이라면 인덱스 저장 후 반복문 종료
		if (ExtractName.find(AnimName) != std::string::npos)
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

		// 애니메이션 이름에 .cmt가 포함된 경우만 카메라 애니메이션이다.
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
		// 카메라 모델의 애니메이션이 종료되면 똑같이 플레이어의 애니메이션도 종료된 것이기 때문에 기존상태로 되돌린다.
		if (m_pCameraModel->Get_AnimFinished())
		{
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Reset();

			m_iCurrentBehavior = 1;				// Idle상태로 되돌려둔다
			Reset_CutSceneEvent();

			Off_Separation_Face();

			return;
		}

		m_pModelCom->Play_Animation_CutScene(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_pAnimCom, false, m_iCutSceneAnimIndex, false);

		CPlayer* pPlayer = this;
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CUTSCENE));
		// Blender에서 얻은 본의 변환 행렬
		_matrix matBoneMatrix = XMLoadFloat4x4(m_pCameraModel->Get_BoneCombinedTransformationMatrix("Camera"));

		// 플레이어의 월드 변환 행렬
		//_matrix matPlayerWorld = pPlayer->Get_TransformCom()->Get_WorldMatrix();
		_matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
		_matrix matVectorBoneWorld = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix("vector_c_n"));

		// Blender의 좌표계를 DirectX의 좌표계로 변환하기 위한 회전 행렬
		_matrix rotationMatrixX = XMMatrixRotationX(XMConvertToRadians(90));
		_matrix rotationMatrixY = XMMatrixRotationY(XMConvertToRadians(-180));
		_matrix rotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(90));

		// Blender의 본 변환 행렬과 플레이어의 월드 변환 행렬을 결합하고 좌표계 변환을 적용
		_matrix finalMat = rotationMatrixX * rotationMatrixY * rotationMatrixZ * matVectorBoneWorld * matBoneMatrix * matPlayerWorld;

		// 최종 뷰 행렬을 계산
		_matrix viewMatrix = XMMatrixInverse(nullptr, finalMat);

		// 뷰 행렬을 파이프라인에 설정
		m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, viewMatrix);

		auto KeyFrames = m_pCameraModel->Get_CurrentKeyFrameIndices(m_iCutSceneCamAnimIndex);
		_uint iKeyFrameIndex = KeyFrames->front();


		pCamera->Set_FoV(m_pCameraModel->Get_FoV(m_pCameraModel->Get_AnimationName(m_iCutSceneCamAnimIndex), iKeyFrameIndex));

		CModel::ANIMATION_DESC Desc{ m_iCutSceneCamAnimIndex, false };
		m_pCameraModel->Set_AnimationIndex(Desc);

		// 카메라 본 애니메이션 실행
		m_pCameraModel->Play_Animation_CutScene(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), nullptr, false, m_iCutSceneCamAnimIndex, false, "Camera");
	}
}

void CPlayer::Reset_CutSceneEvent()
{
	// 이 때 실행하는 애니메이션들은 선형보간을 하지 않는 애니메이션이므로 선형보간 간격을 0으로 꼭!! 초기화해야한다.
	m_pModelCom->Set_ChangeInterval(0.0);
	m_pCameraModel->Set_ChangeInterval(0.0);
	m_pAnimCom->Reset_Animation(m_iCutSceneAnimIndex);
	m_pCameraModel->Reset_Animation(m_iCutSceneCamAnimIndex);

	m_eAnimComType = (m_eAnimComType == DEFAULT ? CUTSCENE : DEFAULT);
	m_pSystemManager->Set_Camera(CAMERA_CUTSCENE == m_pSystemManager->Get_Camera() ? CAMERA_PLAYER : CAMERA_CUTSCENE);
}

// iHandType: 0 양손, 1 왼손, 2 오른손
void CPlayer::On_Separation_Hand(_uint iHandType)
{
	switch (iHandType)
	{
	case 0:		//양손
		m_pModelCom->Set_Separation_ParentBone("ude3_r_n", (_int)HAND_COM);
		m_pModelCom->Set_Separation_ParentBone("ude3_l_n", (_int)HAND_COM);
		break;
	case 1:		//왼손
		m_pModelCom->Set_Separation_ParentBone("ude3_l_n", (_int)HAND_COM);
		break;
	case 2:		//오른손
		m_pModelCom->Set_Separation_ParentBone("ude3_r_n", (_int)HAND_COM);
		break;
	}
}

void CPlayer::Off_Separation_Hand(_uint iHandType)
{
	switch (iHandType)
	{
	case 0:		//양손
		m_pModelCom->Set_Separation_ParentBone("ude3_r_n", -1);
		m_pModelCom->Set_Separation_ParentBone("ude3_l_n", -1);
		break;
	case 1:		//왼손
		m_pModelCom->Set_Separation_ParentBone("ude3_l_n", -1);
		break;
	case 2:		//오른손
		m_pModelCom->Set_Separation_ParentBone("ude3_r_n", -1);
		break;
	}
}

void CPlayer::On_Separation_Face()
{
	m_pModelCom->Set_Separation_ParentBone("face_c_n", (_int)FACE_COM);
	m_pModelCom->Set_Separation_SingleBone("face_c_n", -1);
}

void CPlayer::Off_Separation_Face()
{
	m_pModelCom->Set_Separation_ParentBone("face_c_n", -1);
}

void CPlayer::Compute_MoveDirection_FB()
{
	_vector vCamLook = m_pGameInstance->Get_CamLook();
	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	
	// 방향벡터간 내적의 결과값이 양수면 90도미만(같은방향), 음수면 90도 초과(반대방향), 0이면 직교한다.
	
	// 지금 입력한 키가 앞이면 앞으로 스웨이할건데
	if (m_InputDirection[B])
	{
		// 이 때 캐릭터가 카메라랑 같은 방향을 보고있다면 Front 스웨이
		if (XMVectorGetX(XMVector3Dot(vCamLook, vPlayerLook)) < 0)
			m_MoveDirection[F] = true;
		// 캐릭터가 카메라랑 반대방향을 보고 있다면 Back 스웨이
		else
			m_MoveDirection[B] = true;
	}
	// 지금 입력한 키가 뒤면 뒤로 스웨이할건데, 
	if (m_InputDirection[F])
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

void CPlayer::Effect_Control_Aura()
{
	CSocketEffect* pHooligan = { nullptr };
	CSocketEffect* pRush = { nullptr };
	CSocketEffect* pDestroyer = { nullptr };

	for (auto& pair : m_pEffects)
	{
		string strKey = m_pGameInstance->WstringToString(pair.second->Get_EffectName());

		if (string::npos != strKey.find("Hooligan"))
			pHooligan = pair.second;
		if (string::npos != strKey.find("Rush"))
			pRush = pair.second;
		if (string::npos != strKey.find("Destroyer"))
			pDestroyer = pair.second;
	}

	if (0 < m_iCurrentHitLevel)
	{
		switch (m_eCurrentStyle)
		{
		case Client::CPlayer::KRS:
			// 기존에 켜져있던 오라를 끈다
			if (nullptr != pRush)
				pRush->Off();
			if (nullptr != pDestroyer)
				pDestroyer->Off();

			// 현재 스타일에 맞는 오라를 켠다
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
		
		// 기존 타겟중이던 친구의 거리가 3.f 이상 멀어지면 그때 다시 타겟팅한다.
		if(3.f < vDistance)
			m_pTargetObject = m_pCollisionManager->Get_Near_LandObject(this, pMonsters);
	}
	else
		m_pTargetObject = m_pCollisionManager->Get_Near_LandObject(this, pMonsters);
	
}

void CPlayer::AccHitGauge()
{
	if (PLAYER_HITGAUGE_LEVEL_INTERVAL * 3.f < m_fHitGauge)
		m_fHitGauge = PLAYER_HITGAUGE_LEVEL_INTERVAL * 3.f;
	else
		m_fHitGauge += 5.f;

	m_iCurrentHitLevel = (m_fHitGauge / PLAYER_HITGAUGE_LEVEL_INTERVAL);
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