#include "Karaoke_Kiryu.h"

#include "GameInstance.h"
#include "SystemManager.h"

#ifdef _DEBUG
#include "DebugManager.h"
#endif // _DEBUG

#include "Mesh.h"
#include "Animation.h"

#include "UIManager.h"
#include "Camera.h"
#include "PlayerCamera.h"
#include "CutSceneCamera.h"
#include "FileTotalMgr.h"

#include "Karaoke_Mic.h"
#include "Karaoke_Glass.h"

CKaraoke_Kiryu::CKaraoke_Kiryu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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

CKaraoke_Kiryu::CKaraoke_Kiryu(const CKaraoke_Kiryu& rhs)
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


HRESULT CKaraoke_Kiryu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKaraoke_Kiryu::Initialize(void* pArg)
{
	m_wstrModelName = TEXT("Kiryu_Karaoke");

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	Ready_SingingInterval();
	Ready_PlayTimeEvent();

	m_iHandAnimIndex = HAND_BOU;
	On_Separation_Hand(0);			// ��� �� �ָ���

	m_iFaceAnimIndex = CLOSE;
	Separation_Bone("_jaw_c_n", 3, false);

	/*
	*	[	[0] p_kru_uta_ainote_a_lp[p_kru_uta_ainote_a_lp]		//�ڼ�
		[	[1] p_kru_uta_sing_good_1[p_kru_uta_sing_good_1]		//��â
		[	[2] p_kru_uta_sing_kamae[p_kru_uta_sing_kamae]			//���̵�
		[	[3] p_kru_uta_sing_nml[p_kru_uta_sing_nml]				//Singing
		[	[4] p_oki_uta_sing_nml_lp[p_oki_uta_sing_nml_lp]		//Dancing

	*/
	CModel::ANIMATION_DESC Desc{ static_cast<_uint>(IDLE), true };
	m_pModelCom->Set_AnimationIndex(Desc, 4.f);

	if (FAILED(Add_Objects()))
		return E_FAIL;

	return S_OK;
}

void CKaraoke_Kiryu::Priority_Tick(const _float& fTimeDelta)
{
}

void CKaraoke_Kiryu::Tick(const _float& fTimeDelta)
{
	//if (m_pGameInstance->GetKeyState(DIK_L) == TAP)
	//{
	//	if (m_isSinging)
	//		SingOff();
	//	else
	//		m_isSinging = true;
	//}

	//if (m_pGameInstance->GetKeyState(DIK_K) == TAP)
	//{
	//	Set_CutSceneAnim();
	//}

	PlayTime_Event();

	if (m_isSinging)
		Play_SingingAnim(fTimeDelta);

 	m_pModelCom->Play_Animation(fTimeDelta, false);
	m_pModelCom->Play_Animation_Separation(fTimeDelta, m_iHandAnimIndex, m_SeparationAnimComs[HAND_ANIM], false, (_int)HAND_ANIM);
	//m_pModelCom->Play_Animation_Separation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, (_int)FACE_ANIM);
	m_pModelCom->Play_Animation_Separation(fTimeDelta, m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, 3, 2.f);

	Play_CutScene(fTimeDelta);

	m_pRightHand[m_eRHType]->Tick(fTimeDelta);
}

void CKaraoke_Kiryu::Late_Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	if (m_isObjectRender)
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
		//m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow�� ���� �߰�
	}
#else
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow�� ���� �߰�
#endif // _DEBUG


	if (CUTSCENE != m_eAnimComType)
		Compute_Height();
	else
		m_eRHType = GLASS;			// �ƽ��� �ƴҶ� �ܵ������

	if(!(CUTSCENE == m_eAnimComType && Checked_Animation_Ratio(0.75f)))
 		m_pRightHand[m_eRHType]->Late_Tick(fTimeDelta);
}

HRESULT CKaraoke_Kiryu::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		m_pModelCom->Bind_BoneMatrices(i);
		m_pModelCom->Bind_Compute(m_pComputeShaderCom, i);

		if (FAILED(m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(pMesh->Get_MaterialIndex()))))
			return E_FAIL;

		_float fFar = *m_pGameInstance->Get_CamFar();
		m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

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

HRESULT CKaraoke_Kiryu::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_BoneCompute"),
		TEXT("Com_ComputeShader"), reinterpret_cast<CComponent**>(&m_pComputeShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Kiryu_Karaoke"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Kiryu_Karaoke_CamAction"),
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

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_Kiryu_Karaoke_CutScene"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	//Prototype_Component_Anim_Kiryu_Karaoke_Face
	CAnim* pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_Kiryu_Karaoke_Face"),
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




	//Prototype_Component_Anim_Kiryu
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Material_Kiryu"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CKaraoke_Kiryu::Bind_ResourceData()
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

void CKaraoke_Kiryu::Compute_Height()
{
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	float fHeight = m_pNavigationCom->Compute_Height(vCurrentPos);

	vCurrentPos = XMVectorSetY(vCurrentPos, fHeight);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
}

HRESULT CKaraoke_Kiryu::Add_Objects()
{
	CKaraoke_Mic::SOCKETOBJECT_DESC Desc{};
	Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_r_n");
	Desc.fRotatePecSec = XMConvertToRadians(90.f);
	Desc.fSpeedPecSec = 1.f;
	m_pRightHand.push_back(dynamic_cast<CSocketObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Karaoke_Mic"), &Desc)));

	m_pRightHand.push_back(dynamic_cast<CSocketObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Karaoke_Glass"), &Desc)));

	return S_OK;
}

CKaraoke_Kiryu* CKaraoke_Kiryu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKaraoke_Kiryu* pInstance = new CKaraoke_Kiryu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CKaraoke_Kiryu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKaraoke_Kiryu::Clone(void* pArg)
{
	CKaraoke_Kiryu* pInstance = new CKaraoke_Kiryu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CKaraoke_Kiryu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKaraoke_Kiryu::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pDebugManager);
#endif // _DEBUG
	
	Safe_Release(m_pUIManager);
	//Safe_Release(m_pNavigationCom);
	Safe_Release(m_pAnimCom);
	Safe_Release(m_pCameraModel);

	for (auto& pObj : m_pRightHand)
	{
		Safe_Release(pObj);
	}
	m_pRightHand.clear();

	__super::Free();
}

string CKaraoke_Kiryu::Get_CurrentAnimationName()
{
	return ExtractString(m_pModelCom->Get_AnimationName(m_pModelCom->Get_CurrentAnimationIndex()));
}

_bool CKaraoke_Kiryu::Checked_Animation_Ratio(_float fRatio)
{
	if (fRatio < *m_pCameraModel->Get_AnimationCurrentPosition() / *m_pCameraModel->Get_AnimationDuration())
		return true;

	return false;
}

void CKaraoke_Kiryu::Change_SingingAnimaition(SINGING_ANIMATION eType)
{
	m_eSingingAnimType = eType;
	CModel::ANIMATION_DESC Desc{ static_cast<_uint>(m_eSingingAnimType), true };
	m_pModelCom->Set_AnimationIndex(Desc, 10.f);

	switch (m_eSingingAnimType)
	{
	case Client::CKaraoke_Kiryu::CLAP:
		Off_Separation_Hand();
		break;
	case Client::CKaraoke_Kiryu::PASSIONATE_SINGING:
		On_Separation_Hand(0);
		break;
	case Client::CKaraoke_Kiryu::IDLE:
		On_Separation_Hand(0);
		break;
	case Client::CKaraoke_Kiryu::SINGING:
		On_Separation_Hand(0);
		break;
	case Client::CKaraoke_Kiryu::DANCING:
		Off_Separation_Hand();
		break;
	}

}

void CKaraoke_Kiryu::Set_CutSceneAnim()
{
	if (CUTSCENE == m_eAnimComType) return;

	m_eAnimComType = CUTSCENE;

	Separation_Bone("_jaw_c_n");
	Off_Separation_Hand();			// �� �и� �ִϸ��̼� ����

	_uint j = 0;
	auto CameraAnimList = m_pCameraModel->Get_Animations();
	for (auto& pAnimation : CameraAnimList)
	{
		string CameraAnimName = pAnimation->Get_AnimName();

		// �ִϸ��̼� �̸��� .cmt�� ���Ե� ��츸 ī�޶� �ִϸ��̼��̴�.
		if (CameraAnimName.find(".cmt") != std::string::npos && CameraAnimName.find("m53020") != std::string::npos)
		{
			m_iCutSceneCamAnimIndex = j;
			break;
		}
		j++;
	}

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(CFileTotalMgr::GetInstance()->Get_PlayerMatrix()));
}

void CKaraoke_Kiryu::Play_CutScene(const _float& fTimeDelta)
{
	if (CUTSCENE == m_eAnimComType)
	{
		// ī�޶� ���� �ִϸ��̼��� ����Ǹ� �Ȱ��� �÷��̾��� �ִϸ��̼ǵ� ����� ���̱� ������ �������·� �ǵ�����.
		// �ƽ� ���� �� ������ �������
		if (m_pCameraModel->Get_AnimFinished())
		{
			Reset_CutSceneEvent();

			return;
		}

		// ������ ���� �ִϸ��̼��� �����°� ������Ʈ�̰�, m_pCameraModel�� ī�޶� �ִϸ��̼��� �����ϴ� ���̶� ���������ʴ´�
		m_pModelCom->Play_Animation_CutScene(fTimeDelta, m_pAnimCom, false, m_iCutSceneAnimIndex, false);

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
		m_pCameraModel->Play_Animation_CutScene(fTimeDelta, nullptr, false, m_iCutSceneCamAnimIndex, false);
	}
}

void CKaraoke_Kiryu::Reset_CutSceneEvent()
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

void CKaraoke_Kiryu::PlayTime_Event()
{
	_float fCurTime = m_pGameInstance->GetSoundPosition(TEXT("Bakamita.mp3"), SOUND_BGM);

	for (auto& pEvent : m_PlayTimeEvent)
	{
		if (pEvent.fTime <= fCurTime && !pEvent.isChanged)
		{
			pEvent.isChanged = true;

			if(pEvent.iEventType < 0)
				m_isSinging = true;
			else
			{
				Change_SingingAnimaition(static_cast<SINGING_ANIMATION>(pEvent.iEventType));
			}
		}
	}
	
}

void CKaraoke_Kiryu::Ready_SingingInterval()
{
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.2f);
	m_fMouthChangeInterval.push_back(0.2f);
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.3f);
	m_fMouthChangeInterval.push_back(0.3f);
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.4f);
	m_fMouthChangeInterval.push_back(0.4f);
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.1f);
	m_fMouthChangeInterval.push_back(0.2f);
	m_fMouthChangeInterval.push_back(0.2f);
}

void CKaraoke_Kiryu::Ready_PlayTimeEvent()
{
	PLAY_TIME_EVENT Desc{ 14.450f, SINGING, false };
	m_PlayTimeEvent.push_back(Desc);		// ����ũ�� ���

	Desc.fTime = 15.305f;
	Desc.iEventType = -1;
	m_PlayTimeEvent.push_back(Desc);		// �� �ִϸ��̼��� �ִ´�.

	Desc.fTime = 41.375f;
	Desc.iEventType = PASSIONATE_SINGING;
	m_PlayTimeEvent.push_back(Desc);		// ��â��

	Desc.fTime = 43.289f;
	Desc.iEventType = SINGING;
	m_PlayTimeEvent.push_back(Desc);		

	Desc.fTime = 47.561f;
	Desc.iEventType = PASSIONATE_SINGING;
	m_PlayTimeEvent.push_back(Desc);		// ��â��

	Desc.fTime = 48.977f;
	Desc.iEventType = SINGING;
	m_PlayTimeEvent.push_back(Desc);		// ��â��
}

void CKaraoke_Kiryu::Play_SingingAnim(const _float& fTimeDelta)
{
	_float fCurrent = m_fMouthChangeInterval.front();

	m_fMouthTimer += fTimeDelta;
	if (fCurrent <= m_fMouthTimer)
	{
		m_fMouthTimer = 0.f;

		Change_MouthAnim();
		// �� �տ��Ÿ� �����, �Ǿ� ���� �� �ڷ� �ٽ� Ǫ�ù����ش�.
		m_fMouthChangeInterval.erase(m_fMouthChangeInterval.begin());
		m_fMouthChangeInterval.push_back(fCurrent);
	}
}

void CKaraoke_Kiryu::Change_MouthAnim()
{
	m_iFaceAnimIndex = m_iFaceAnimIndex == CLOSE ? OPEN : CLOSE;
	m_SeparationAnimComs[FACE_ANIM]->Reset_Animation(m_iFaceAnimIndex);
}
