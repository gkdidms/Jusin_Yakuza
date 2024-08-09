#include "..\Public\Map.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Transform.h"

#include "Mesh.h"

CMap::CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CMap::CMap(const CMap& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CMap::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CMap::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;
		m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
		m_iLayerNum = gameobjDesc->iLayer;
		m_wstrModelName = gameobjDesc->wstrModelName;
		m_iShaderPassNum = gameobjDesc->iShaderPass;
		m_iObjectType = gameobjDesc->iObjType;
		m_bCull = gameobjDesc->bCull;
		m_bLocalCull = gameobjDesc->bLocalCull;

		for (int i = 0; i < gameobjDesc->iDecalNum; i++)
		{
			DECAL_DESC  tDecal;
			CTexture* pTexture;
			XMMATRIX    vStartPos;

			CDecal::DECALOBJ_DESC		decalObjDesc{};
			decalObjDesc.iMaterialNum = gameobjDesc->pDecal[i].iMaterialNum;
			decalObjDesc.pTexture = m_pModelCom->Copy_DecalTexture(decalObjDesc.iMaterialNum);
			decalObjDesc.vStartPos = XMLoadFloat4x4(&gameobjDesc->pDecal[i].vTransform);

			CDecal* pDecal = dynamic_cast<CDecal*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Decal"), &decalObjDesc));

			m_vDecals.push_back(pDecal);
		}

		for (int i = 0; i < gameobjDesc->iColliderNum; i++)
		{
			OBJCOLLIDER_DESC		objDesc = gameobjDesc->pColliderDesc[i];

			CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

			ColliderDesc.eType = (CCollider::TYPE)objDesc.iColliderType;
			ColliderDesc.vExtents = objDesc.vExtents;
			ColliderDesc.vCenter = objDesc.vCenter;
			ColliderDesc.vRotation = objDesc.vQuaternion;

			CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(m_iCurrentLevel, TEXT("Prototype_Component_Collider"), &ColliderDesc));

			m_vColliders.push_back(pCollider);

		}

		m_pPlayerMatrix = gameobjDesc->vPlayerMatrix;
	}

	m_Casecade = { 0.f, 10.f, 24.f, 40.f };



	return S_OK;
}

void CMap::Priority_Tick(const _float& fTimeDelta)
{
}

void CMap::Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_vDecals)
		iter->Tick(fTimeDelta);

	if (2 == m_iShaderPassNum)
	{
		m_fWaterDeltaTime += fTimeDelta * 0.07;

		if (m_fWaterDeltaTime > 1)
			m_fWaterDeltaTime = 0;
	}


#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		iter->Tick(m_pTransformCom->Get_WorldMatrix());
#endif
}

void CMap::Late_Tick(const _float& fTimeDelta)
{
	// Renderer에 추가되는 mesh index 비워주고 시작
	m_vRenderDefaulMeshIndex.clear();
	m_vRenderGlassMeshIndex.clear();
	m_vDecalMeshIndex.clear();
	m_vDecalLightMeshIndex.clear();
	m_vSignMeshIndex.clear();
	m_vLampMeshIndex.clear();
	m_vDecalBlendMeshIndex.clear();
	m_vBloomIndex.clear();
	m_vMaskSignIndex.clear();
	m_vDynamicSignIndex.clear();
	m_vStrongBloomIndex.clear();
	m_vCompulsoryDecalBlendMeshIndex.clear();

	_vector vPlayerPosition;
	memcpy(&vPlayerPosition, m_pPlayerMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));

	float fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vPlayerPosition));


	if (fDistance < 100)
	{
		// Renderer 추가 및 벡터에 추가
	}
	Add_Renderer(fTimeDelta);

//	m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this);

}

HRESULT CMap::Render()
{
#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		m_pGameInstance->Add_DebugComponent(iter);
#endif


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

#pragma region mesh컬링전Render
	//_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	//vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();
	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
	//		return E_FAIL;

	//	_bool isRS = true;
	//	_bool isRD = true;
	//	if (!strcmp(Meshes[i]->Get_Name(), "box4783"))
	//	{
	//		isRS = false;
	//		isRD = false;
	//	}

	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
	//		isRS = false;
	//	m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));

	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
	//		isRD = false;

	//	m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

	//	bool	bNormalExist = m_pModelCom->Check_Exist_Material(i, aiTextureType_NORMALS);
	//	// Normal texture가 있을 경우
	//	if (true == bNormalExist)
	//	{
	//		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

	//		if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistNormalTex", &bNormalExist, sizeof(bool))))
	//			return E_FAIL;
	//	}
	//	else
	//	{
	//		if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistNormalTex", &bNormalExist, sizeof(bool))))
	//			return E_FAIL;
	//	}

	//	bool	bRMExist = m_pModelCom->Check_Exist_Material(i, aiTextureType_METALNESS);
	//	if (true == bRMExist)
	//	{
	//		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS)))
	//			return E_FAIL;

	//		if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
	//			return E_FAIL;
	//	}
	//	else
	//	{
	//		if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
	//			return E_FAIL;
	//	}

	//	bool	bRSExist = m_pModelCom->Check_Exist_Material(i, aiTextureType_SPECULAR);
	//	if (true == bRSExist)
	//	{
	//		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
	//			return E_FAIL;

	//		if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRSTex", &bRSExist, sizeof(bool))))
	//			return E_FAIL;
	//	}
	//	else
	//	{
	//		if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRSTex", &bRSExist, sizeof(bool))))
	//			return E_FAIL;
	//	}

	//	if (1 == m_iShaderPassNum)
	//	{
	//		// 유리문 처리
	//		if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_NonBlendDiffuse"), m_pShaderCom, "g_RefractionTexture")))
	//			return E_FAIL;
	//	}
	//	else if (2 == m_iShaderPassNum)
	//	{
	//		//m_pTexture->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0);

	//		// 웅덩이
	//		if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fWaterDeltaTime, sizeof(float))))
	//			return E_FAIL;

	//		if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_NonBlendDiffuse"), m_pShaderCom, "g_RefractionTexture")))
	//			return E_FAIL;

	//		if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_NonBlendDepth"), m_pShaderCom, "g_DepthTexture")))
	//			return E_FAIL;

	//		if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
	//			return E_FAIL;

	//		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
	//			return E_FAIL;
	//		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
	//			return E_FAIL;
	//		if (FAILED(m_pShaderCom->Bind_Matrix("g_ReflectViewMatrix", m_pGameInstance->Get_ReflectViewMatrix())))
	//			return E_FAIL;

	//		if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BackBlurReverse"), m_pShaderCom, "g_BlurReverseTexture")))
	//			return E_FAIL;

	//	}

	//	const char* defaultMesh = "DEFAULTMESH";
	//	const char* glassMesh = "GLASSMESH";
	//	const char* decalMesh = "DECALMESH";
	//	const char* signMesh = "SIGNMESH";

	//	if (0 == strcmp(Meshes[i]->Get_Name(), defaultMesh))
	//	{
	//		m_pShaderCom->Begin(SHADER_DEFAULT_MAP);
	//	}
	//	else if (0 == strcmp(Meshes[i]->Get_Name(), glassMesh))
	//	{
	//		m_pShaderCom->Begin(SHADER_GLASS);
	//	}
	//	else if (0 == strcmp(Meshes[i]->Get_Name(), decalMesh))
	//	{
	//		m_pShaderCom->Begin(SHADER_DECAL);
	//	}
	//	else if (0 == strcmp(Meshes[i]->Get_Name(), signMesh))
	//	{
	//		m_pShaderCom->Begin(SHADER_SIGN);
	//	}



	//	m_pModelCom->Render(i);
	//}
#pragma endregion

	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();

	int		iRenderState = m_pGameInstance->Get_RenderState();

#pragma region Render_DefaultMeshGroup
	if (iRenderState == CRenderer::RENDER_NONBLENDER)
	{
		// VtxMesh - Defulat_Pass - 0번
		for (size_t k = 0; k < m_vRenderDefaulMeshIndex.size(); k++)
		{
			int		i = m_vRenderDefaulMeshIndex[k];

			m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(Meshes[i]->Get_MaterialIndex()));

			_bool fFar = m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;
			m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

			_bool isRS = true;
			_bool isRD = true;
			_bool isRM = true;
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

			m_pShaderCom->Begin(SHADER_DEFAULT_MAP);

			m_pModelCom->Render(i);
		}

		// VtxMesh - Defulat_Pass - 0번
		for (size_t k = 0; k < m_vSignMeshIndex.size(); k++)
		{
			int		i = m_vSignMeshIndex[k];

			m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(Meshes[i]->Get_MaterialIndex()));

			_bool fFar = m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;
			m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

			_bool isRS = true;
			_bool isRD = true;
			_bool isRM = true;
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

			m_pShaderCom->Begin(SHADER_SIGN);

			m_pModelCom->Render(i);
		}


		for (size_t k = 0; k < m_vLampMeshIndex.size(); k++)
		{
			int		i = m_vLampMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			bool	bRMExist = m_pModelCom->Check_Exist_Material(i, aiTextureType_METALNESS);

			// Normal texture가 있을 경우
			if (true == bRMExist)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS);

				if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
					return E_FAIL;
			}

			m_pShaderCom->Begin(SHADER_DEFAULT_MAP);

			m_pModelCom->Render(i);
		}

		for (size_t k = 0; k < m_vBloomIndex.size(); k++)
		{
			int		i = m_vBloomIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_DEFAULT_MAP);

			m_pModelCom->Render(i);
		}

		for (size_t k = 0; k < m_vMaskSignIndex.size(); k++)
		{
			int		i = m_vMaskSignIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_SIGN_MASK);

			m_pModelCom->Render(i);
		}


		for (size_t k = 0; k < m_vStrongBloomIndex.size(); k++)
		{
			int		i = m_vStrongBloomIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_DEFAULT_MAP);

			m_pModelCom->Render(i);
		}
	}
#pragma endregion

#pragma region Render_GlassMeshGroup
	if (iRenderState == CRenderer::RENDER_GLASS)
	{
		for (size_t k = 0; k < m_vRenderGlassMeshIndex.size(); k++)
		{
			int		i = m_vRenderGlassMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			_bool isRS = true;
			_bool isRD = true;
			if (!strcmp(Meshes[i]->Get_Name(), "box4783"))
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

			if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_NonBlendDiffuse"), m_pShaderCom, "g_RefractionTexture")))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_GLASS);

			m_pModelCom->Render(i);
		}

	}

#pragma endregion

#pragma region Render_EffectMeshGroup
	if (iRenderState == CRenderer::RENDER_EFFECT)
	{
		for (size_t k = 0; k < m_vDecalLightMeshIndex.size(); k++)
		{
			int		i = m_vDecalLightMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_DECAL_LIGHT);

			m_pModelCom->Render(i);
		}

	}
#pragma endregion

#pragma region Render_DecalMesh

	if (iRenderState == CRenderer::RENDER_DECAL)
	{
		// 일반 마스크
		// VtxMesh - Defulat_Pass - 2번
		for (size_t k = 0; k < m_vDecalBlendMeshIndex.size(); k++)
		{
			int		i = m_vDecalBlendMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_DECAL_BLEND);

			m_pModelCom->Render(i);
		}

		// 일반 마스크
		// VtxMesh - Defulat_Pass - 2번
		for (size_t k = 0; k < m_vDecalMeshIndex.size(); k++)
		{
			int		i = m_vDecalMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;


			m_pShaderCom->Begin(SHADER_DECAL_MASK);

			m_pModelCom->Render(i);
		}

		for (size_t k = 0; k < m_vCompulsoryDecalBlendMeshIndex.size(); k++)
		{
			int		i = m_vCompulsoryDecalBlendMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_COMPULSORY_DECALBLEND);

			m_pModelCom->Render(i);
		}
	}
	
#pragma endregion

#pragma region Render_Bloom
	if (iRenderState == CRenderer::RENDER_NONLIGHT)
	{
		// 일반 마스크
		// VtxMesh - Defulat_Pass - 2번
		for (size_t k = 0; k < m_vBloomIndex.size(); k++)
		{
			int		i = m_vBloomIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			//White 부분만 되게끔
			m_pShaderCom->Begin(SHADER_BLOOM_WHITE);

			m_pModelCom->Render(i);
		}

		for (size_t k = 0; k < m_vSignMeshIndex.size(); k++)
		{
			int		i = m_vSignMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			//White 부분만 되게끔
			m_pShaderCom->Begin(SHADER_BLOOM_WHITE);

			m_pModelCom->Render(i);
		}


		for (size_t k = 0; k < m_vLampMeshIndex.size(); k++)
		{
			int		i = m_vLampMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			bool	bRMExist = m_pModelCom->Check_Exist_Material(i, aiTextureType_METALNESS);
			// Normal texture가 있을 경우
			if (true == bRMExist)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS);

				if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
					return E_FAIL;
			}

			m_pShaderCom->Begin(SHADER_LAMP);

			m_pModelCom->Render(i);
		}

		for (size_t k = 0; k < m_vStrongBloomIndex.size(); k++)
		{
			int		i = m_vStrongBloomIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_STRONGBLOOM);

			m_pModelCom->Render(i);
		}

	}
#pragma endregion


#pragma region Render_NonLightNonBlur
	if (iRenderState == CRenderer::RENDER_NONLIGHT_NONBLUR)
	{
		// 일반 마스크
		// VtxMesh - Defulat_Pass - 2번
		for (size_t k = 0; k < m_vSignMeshIndex.size(); k++)
		{
			int		i = m_vSignMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_SIGN);

			m_pModelCom->Render(i);
		}


		for (size_t k = 0; k < m_vMaskSignIndex.size(); k++)
		{
			int		i = m_vMaskSignIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_SIGN_MASK);

			m_pModelCom->Render(i);
		}
	}
#pragma endregion



	return S_OK;
}

HRESULT CMap::Render_LightDepth()
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
		m_pShaderCom->Begin(SHADER_LIGHTDEPTH);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

int CMap::Get_ObjPlaceDesc(OBJECTPLACE_DESC* objplaceDesc)
{
	/* 바이너리화하기 위한 데이터 제공 */

	XMStoreFloat4x4(&objplaceDesc->vTransform, m_pTransformCom->Get_WorldMatrix());

	//objPlaceDesc.iLayer = m_iLayerNum;

	string strName = m_pGameInstance->WstringToString(m_wstrModelName);
	strcpy_s(objplaceDesc->strModelCom, strName.c_str());

	objplaceDesc->iShaderPassNum = m_iShaderPassNum;
	objplaceDesc->iObjType = m_iObjectType;

	/* layer는 return 형식으로 */
	return m_iLayerNum;
}

CMap::MAPOBJ_DESC CMap::Get_MapObjDesc_For_AddList()
{
	CMap::MAPOBJ_DESC		mapobjDesc;
	mapobjDesc.vStartPos = m_pTransformCom->Get_WorldMatrix();
	mapobjDesc.wstrModelName = m_wstrModelName;
	mapobjDesc.iShaderPass = m_iShaderPassNum;

	return mapobjDesc;
}

void CMap::Edit_GameObject_Information(MAPOBJ_DESC mapDesc)
{
	m_iLayerNum = mapDesc.iLayer;
	m_iShaderPassNum = mapDesc.iShaderPass;
	m_iObjectType = mapDesc.iObjType;
}

CMap::MAPOBJ_DESC CMap::Send_GameObject_Information()
{
	return MAPOBJ_DESC();
}

void CMap::Add_Renderer(const _float& fTimeDelta)
{

	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_float4x4 vLocalMatrix = Meshes[i]->Get_LocalMatrix();

		XMVECTOR localPos = XMVectorSet(vLocalMatrix._41, vLocalMatrix._42, vLocalMatrix._43, 1);
		XMVECTOR worldPos = XMVectorZero();
		worldPos = XMVector3Transform(localPos, m_pTransformCom->Get_WorldMatrix());

		string meshName = Meshes[i]->Get_Name();

		// 점 위치 찾기
		size_t dotPos = meshName.find('.');

		// 점이 있으면 점 이전 부분을 추출하고, 없으면 전체 문자열을 사용
		string baseName = (dotPos != string::npos) ? meshName.substr(0, dotPos) : meshName;

		const char* baseNameCStr = baseName.c_str();

		if (false == m_bCull && false == m_bLocalCull)
		{
			// 컬링안함
			if (0 == strcmp(baseNameCStr, "DEFAULTMESH"))
			{
				m_vRenderDefaulMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "GLASSMESH"))
			{
				m_vRenderGlassMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "DECALMESH"))
			{
				m_vDecalMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "DECALLIGHTMESH"))
			{
				m_vDecalLightMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "SIGNMESH"))
			{
				m_vSignMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "LAMPMESH"))
			{
				// Nonblend + effect 둘 다 호출
				m_vLampMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "DECALBLENDMESH"))
			{
				m_vDecalBlendMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "BLOOMMESH"))
			{
				m_vBloomIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "MASKSIGNMESH"))
			{
				m_vMaskSignIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "STRONGBLOOM"))
			{
				m_vStrongBloomIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "COMBLENDDECAL"))
			{
				m_vCompulsoryDecalBlendMeshIndex.push_back(i);
			}
			else if (strstr(baseNameCStr, "DYNAMICSIGN") != nullptr)
			{
				m_vDynamicSignIndex.push_back(i);
			}
			else
			{
				// mesh 안합쳐놓으면 그냥 default로 처리
				m_vRenderDefaulMeshIndex.push_back(i);
			}
		}
		else if (false == m_bLocalCull)
		{
			float		fScale = Meshes[i]->Get_MeshScale();

			if (fScale <= 0)
				fScale = 10;

			// 컬링
			if (true == m_pGameInstance->isIn_WorldFrustum(worldPos, fScale * 1.5))
			{
				// 컬링안함
				if (0 == strcmp(baseNameCStr, "DEFAULTMESH"))
				{
					m_vRenderDefaulMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "GLASSMESH"))
				{
					m_vRenderGlassMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "DECALMESH"))
				{
					m_vDecalMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "DECALLIGHTMESH"))
				{
					m_vDecalLightMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "SIGNMESH"))
				{
					m_vSignMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "LAMPMESH"))
				{
					// Nonblend + effect 둘 다 호출
					m_vLampMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "DECALBLENDMESH"))
				{
					m_vDecalBlendMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "BLOOMMESH"))
				{
					m_vBloomIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "MASKSIGNMESH"))
				{
					m_vMaskSignIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "STRONGBLOOM"))
				{
					m_vStrongBloomIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "COMBLENDDECAL"))
				{
					m_vCompulsoryDecalBlendMeshIndex.push_back(i);
				}
				else if (strstr(baseNameCStr, "DYNAMICSIGN") != nullptr)
				{
					m_vDynamicSignIndex.push_back(i);
				}
				else
				{
					// mesh 안합쳐놓으면 그냥 default로 처리
					m_vRenderDefaulMeshIndex.push_back(i);
				}
			}
		}
		else
		{
			// 도로씬에서 다 DECAL 처리
			float		fScale = Meshes[i]->Get_MeshScale() * 0.5;

			if (fScale <= 0)
				fScale = 10;

			if (true == m_pGameInstance->isIn_WorldFrustum(worldPos, fScale))
			{
				m_vDecalMeshIndex.push_back(i);
			}
		}
	}

	// RENDER_NONBLEND 돼야하는 그룹
	if (0 < m_vRenderDefaulMeshIndex.size() || 0 < m_vSignMeshIndex.size() || 0 < m_vLampMeshIndex.size() || 0 < m_vBloomIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	// RENDER_GLASS 돼야하는 그룹
	if (0 < m_vRenderGlassMeshIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_GLASS, this);

	// RENDER_EFFECT 돼야하는 그룹
	if (0 < m_vDecalLightMeshIndex.size() || 0 < m_vLampMeshIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);

	if (0 < m_vDecalBlendMeshIndex.size() || 0 < m_vDecalMeshIndex.size() || 0 < m_vCompulsoryDecalBlendMeshIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_DECAL, this);

	// 빛 영향 안받고 원색값 유지
	if (0 < m_vSignMeshIndex.size() || 0 < m_vMaskSignIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT_NONBLUR, this);

	//Bloom
	if (0 < m_vBloomIndex.size() || 0 < m_vSignMeshIndex.size() || 0 < m_vLampMeshIndex.size() || 0 < m_vStrongBloomIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);


	for (auto& iter : m_vDecals)
		iter->Late_Tick(fTimeDelta);

}

HRESULT CMap::Add_Components(void* pArg)
{
	MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, gameobjDesc->wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	//모델 이름 추출
	string strModelName = m_pGameInstance->WstringToString(gameobjDesc->wstrModelName);
	string strRemoveName = "Prototype_Component_Model_";
	_int iPos = strModelName.find(strRemoveName);
	
	if (iPos == string::npos)
		return E_FAIL;

	strModelName = strModelName.erase(iPos, strRemoveName.size());

	wstring strMaterialName = TEXT("Prototype_Component_Material_") + m_pGameInstance->StringToWstring(strModelName);

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, strMaterialName,
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_MeshMap"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CMap* CMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMap* pInstance = new CMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMap::Clone(void* pArg)
{
	CMap* pInstance = new CMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap::Free()
{
	__super::Free();

	for (auto& iter : m_vDecals)
		Safe_Release(iter);
	m_vDecals.clear();

	for (auto& iter : m_vColliders)
		Safe_Release(iter);
	m_vColliders.clear();

	m_vRenderDefaulMeshIndex.clear();
	m_vRenderGlassMeshIndex.clear();
	m_vDecalMeshIndex.clear();
	m_vDecalLightMeshIndex.clear();
	m_vSignMeshIndex.clear();
	m_vLampMeshIndex.clear();
	m_vDecalBlendMeshIndex.clear();
	m_vBloomIndex.clear();
	m_vMaskSignIndex.clear();
	m_vDynamicSignIndex.clear();
	m_vStrongBloomIndex.clear();
	m_vCompulsoryDecalBlendMeshIndex.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSystemManager);
}
