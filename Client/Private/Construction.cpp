#include "..\Public\Construction.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Transform.h"

CConstruction::CConstruction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext },
	m_pSystemManager { CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CConstruction::CConstruction(const CConstruction& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager}
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CConstruction::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CConstruction::Initialize(void* pArg)
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
			//OBJCOLLIDER_DESC		objDesc = gameobjDesc->pColliderDesc[i];

			//CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

			//ColliderDesc.eType = (CCollider::TYPE)objDesc.iColliderType;
			//ColliderDesc.vExtents = objDesc.vExtents;
			//ColliderDesc.vCenter = objDesc.vCenter;
			//ColliderDesc.vRotation = objDesc.vQuaternion;

			//CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(LEVEL_TEST, TEXT("Prototype_Component_Collider"), &ColliderDesc));

			//m_vColliders.push_back(pCollider);

		}
	}

	// 물웅덩이 noiseTexture
	//if (2 == m_iShaderPassNum)
	//{
	//	/* For.Com_Shader */
	//	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Texture_NoiseTexture"),
	//		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
	//		return E_FAIL;
	//}


	return S_OK;
}

void CConstruction::Priority_Tick(const _float& fTimeDelta)
{
}

void CConstruction::Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_vDecals)
		iter->Tick(fTimeDelta);

	if (2 == m_iShaderPassNum)
	{
		m_fWaterDeltaTime += fTimeDelta*0.07;

		if (m_fWaterDeltaTime > 1)
			m_fWaterDeltaTime = 0;
	}


#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		iter->Tick(m_pTransformCom->Get_WorldMatrix());
#endif
}

void CConstruction::Late_Tick(const _float& fTimeDelta)
{
	if (0 == m_iShaderPassNum)
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	}
	else if (1 == m_iShaderPassNum)
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_GLASS, this);
	}
	else if (2 == m_iShaderPassNum)
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_PUDDLE, this);
	}
	
	if (m_pGameInstance->isShadow() && m_isFirst)
	{
		// 처음 렌더를 돌 때만 그림자를 그려준다.
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_PASSIVE_SHADOW, this);
		m_isFirst = false;
	}
		
	
	for (auto& iter : m_vDecals)
		iter->Late_Tick(fTimeDelta);	
}

HRESULT CConstruction::Render()
{
#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		m_pGameInstance->Add_DebugComponent(iter);
#endif


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

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
		

		if (1 == m_iShaderPassNum)
		{
			// 유리문 처리
			if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_NonBlendDiffuse"), m_pShaderCom, "g_RefractionTexture")))
				return E_FAIL;
		}
		else if (2 == m_iShaderPassNum)
		{
			//m_pTexture->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0);

			// 웅덩이
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fWaterDeltaTime, sizeof(float))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_NonBlendDiffuse"), m_pShaderCom, "g_RefractionTexture")))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_NonBlendDepth"), m_pShaderCom, "g_DepthTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Bind_Matrix("g_ReflectViewMatrix", m_pGameInstance->Get_ReflectViewMatrix())))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BackBlurReverse"), m_pShaderCom, "g_BlurReverseTexture")))
				return E_FAIL;

		}

		m_pShaderCom->Begin(m_iShaderPassNum);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CConstruction::Render_LightDepth()
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
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

int CConstruction::Get_ObjPlaceDesc(OBJECTPLACE_DESC* objplaceDesc)
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

CConstruction::MAPOBJ_DESC CConstruction::Get_MapObjDesc_For_AddList()
{
	CConstruction::MAPOBJ_DESC		mapobjDesc;
	mapobjDesc.vStartPos = m_pTransformCom->Get_WorldMatrix();
	mapobjDesc.wstrModelName = m_wstrModelName;
	mapobjDesc.iShaderPass = m_iShaderPassNum;

	return mapobjDesc;
}

void CConstruction::Edit_GameObject_Information(MAPOBJ_DESC mapDesc)
{
	m_iLayerNum = mapDesc.iLayer;
	m_iShaderPassNum = mapDesc.iShaderPass;
	m_iObjectType = mapDesc.iObjType;
}

CConstruction::MAPOBJ_DESC CConstruction::Send_GameObject_Information()
{
	return MAPOBJ_DESC();
}

HRESULT CConstruction::Add_Components(void* pArg)
{
	MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TEST, gameobjDesc->wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;



	return S_OK;
}

HRESULT CConstruction::Bind_ShaderResources()
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

CConstruction* CConstruction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CConstruction* pInstance = new CConstruction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CConstruction::Clone(void* pArg)
{
	CConstruction* pInstance = new CConstruction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConstruction::Free()
{
	__super::Free();

	for (auto& iter : m_vDecals)
		Safe_Release(iter);
	m_vDecals.clear();

	for (auto& iter : m_vColliders)
		Safe_Release(iter);
	m_vColliders.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSystemManager);
}
