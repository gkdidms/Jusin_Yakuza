#include "SceneModel_Test.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Transform.h"

#include "Mesh.h"
#include "Camera.h"
#include "Player.h"

CSceneModel_Test::CSceneModel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CSceneModel_Test::CSceneModel_Test(const CSceneModel_Test& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CSceneModel_Test::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CSceneModel_Test::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	CModel::ANIMATION_DESC Desc{19, true};

	m_pModelCom->Set_AnimationIndex(Desc);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(30, 30, 30, 1));

	return S_OK;
}

void CSceneModel_Test::Priority_Tick(const _float& fTimeDelta)
{
	if (m_pGameInstance->GetKeyState(DIK_PGUP))
		m_pTransformCom->Go_Straight(fTimeDelta);
	if (m_pGameInstance->GetKeyState(DIK_PGDN))
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta);

	if (m_pGameInstance->GetKeyState(DIK_O))
		fTestY += 1.f;
	if (m_pGameInstance->GetKeyState(DIK_P))
		fTestY -= 1.f;

	if (m_pGameInstance->GetKeyState(DIK_N))
		fTestZ += 1.f;
	if (m_pGameInstance->GetKeyState(DIK_M))
		fTestZ -= 1.f;

	if (m_pGameInstance->GetKeyState(DIK_K))
		fTestX += 1.f;
	if (m_pGameInstance->GetKeyState(DIK_L))
		fTestX -= 1.f;
}

void CSceneModel_Test::Tick(const _float& fTimeDelta)
{
	if (m_pGameInstance->GetKeyState(DIK_SPACE) == TAP)
	{
		test = !test;
	}

	if(test)
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 0.f);
	else
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 1.f);

	m_pModelCom->Play_Animation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));

	if (CAMERA_CINEMACHINE == m_pSystemManager->Get_Camera())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_TEST, TEXT("Layer_SceneModel_Test"), 0));
		//CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
		// Blender에서 얻은 본의 변환 행렬
		_matrix matBoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix("Camera"));

		// 플레이어의 월드 변환 행렬
		//_matrix matPlayerWorld = pPlayer->Get_TransformCom()->Get_WorldMatrix();
		_matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
		_matrix matVectorBoneWorld = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix("pattern_c_n"));

		// Blender의 좌표계를 DirectX의 좌표계로 변환하기 위한 회전 행렬
		_matrix rotationMatrixX = XMMatrixRotationX(XMConvertToRadians(fTestX));
		_matrix rotationMatrixY = XMMatrixRotationY(XMConvertToRadians(fTestY));
		_matrix rotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(fTestZ));

		// Blender의 본 변환 행렬과 플레이어의 월드 변환 행렬을 결합하고 좌표계 변환을 적용
		_matrix finalMat = rotationMatrixX* rotationMatrixY * rotationMatrixZ * matVectorBoneWorld * matBoneMatrix * matPlayerWorld;

		// 최종 뷰 행렬을 계산
		_matrix viewMatrix = XMMatrixInverse(nullptr, finalMat);

		// 뷰 행렬을 파이프라인에 설정
		m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, viewMatrix);
	}
	
	
	CModel::ANIMATION_DESC Desc{ 40, true };
	m_pModelCom->Play_Animation(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")), Desc);
}

void CSceneModel_Test::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow용 렌더 추가
}

HRESULT CSceneModel_Test::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		_bool isRS = true;
		_bool isRD = true;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
			isRS = false;
		m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;

		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		bool	bNormalExist = m_pModelCom->Check_Exist_Material(i, aiTextureType_NORMALS);
		// Normal texture가 있을 경우
		if (true == bNormalExist)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

			if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistNormalTex", &bNormalExist, sizeof(bool))))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistNormalTex", &bNormalExist, sizeof(bool))))
				return E_FAIL;
		}

		bool	bRMExist = m_pModelCom->Check_Exist_Material(i, aiTextureType_METALNESS);
		if (true == bRMExist)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
				return E_FAIL;
		}

		bool	bRSExist = m_pModelCom->Check_Exist_Material(i, aiTextureType_SPECULAR);
		if (true == bRSExist)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRSTex", &bRSExist, sizeof(bool))))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRSTex", &bRSExist, sizeof(bool))))
				return E_FAIL;
		}

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CSceneModel_Test::Render_LightDepth()
{
	const _float4x4* ViewMatrixArray = m_pGameInstance->Get_Shadow_Transform_View_Float4x4();
	const _float4x4* ProjMatrixArray = m_pGameInstance->Get_Shadow_Transform_Proj_Float4x4();

	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_ViewMatrixArray", ViewMatrixArray, 3)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_ProjMatrixArray", ProjMatrixArray, 3)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Begin(5);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CSceneModel_Test::Add_Components(void* pArg)
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Model_Kiryu_CamAction"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSceneModel_Test::Bind_ShaderResources()
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

CSceneModel_Test* CSceneModel_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSceneModel_Test* pInstance = new CSceneModel_Test(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSceneModel_Test");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSceneModel_Test::Clone(void* pArg)
{
	CSceneModel_Test* pInstance = new CSceneModel_Test(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSceneModel_Test");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSceneModel_Test::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSystemManager);
}
