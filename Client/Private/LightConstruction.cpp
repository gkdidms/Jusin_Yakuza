#include "..\Public\LightConstruction.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Transform.h"

#include "Mesh.h"

CLightConstruction::CLightConstruction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CLightConstruction::CLightConstruction(const CLightConstruction& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CLightConstruction::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CLightConstruction::Initialize(void* pArg)
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
			OBJCOLLIDER_DESC		objDesc = gameobjDesc->pColliderDesc[i];

			CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

			ColliderDesc.eType = (CCollider::TYPE)objDesc.iColliderType;
			ColliderDesc.vExtents = objDesc.vExtents;
			ColliderDesc.vCenter = objDesc.vCenter;
			ColliderDesc.vRotation = objDesc.vQuaternion;

			CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(LEVEL_TEST, TEXT("Prototype_Component_Collider"), &ColliderDesc));

			m_vColliders.push_back(pCollider);

		}
	}

	m_Casecade = { 0.f, 10.f, 24.f, 40.f };

	return S_OK;
}

void CLightConstruction::Priority_Tick(const _float& fTimeDelta)
{
}

void CLightConstruction::Tick(const _float& fTimeDelta)
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

void CLightConstruction::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);

	for (auto& iter : m_vDecals)
		iter->Late_Tick(fTimeDelta);
}

HRESULT CLightConstruction::Render()
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

		m_pShaderCom->Begin(m_iShaderPassNum);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CLightConstruction::Render_LightDepth()
{
	//케스케이드 그림자맵을 위한 절두체
	_float3 vFrustum[]{
		{-1.f, 1.f, 0.f},
		{1.f, 1.f, 0.f},
		{1.f, -1.f, 0.f},
		{-1.f, -1.f, 0.f},

		{-1.f, 1.f, 1.f},
		{1.f, 1.f, 1.f},
		{1.f, -1.f, 1.f},
		{-1.f, -1.f, 1.f}
	};

	// NDC좌표계 -> 월드 좌표계 변환 행렬
	_matrix ViewMatrixInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrixInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ);

	_float4	vPoints[8] = {};
	_float4x4 ViewMatrixArray[3] = {};
	_float4x4 ProjMatrixArray[3] = {};

	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat4(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&vFrustum[i]), ProjMatrixInverse));
	}
	for (size_t i = 0; i < 8; i++)
		XMStoreFloat3(&vFrustum[i], XMVector3Transform(XMLoadFloat4(&vPoints[i]), ViewMatrixInverse));

	for (size_t i = 0; i < m_Casecade.size() - 1; ++i)
	{
		//큐브의 정점을 시야절두체 구간으로 변경
		_float3 Frustum[8];
		for (size_t j = 0; j < 8; ++j)
			Frustum[j] = vFrustum[j];

		for (size_t j = 0; j < 4; ++j)
		{
			//앞에서 뒤쪽으로 향하는 벡터
			_vector vTemp = XMVector3Normalize(XMLoadFloat3(&Frustum[j + 4]) - XMLoadFloat3(&Frustum[j]));

			//구간 시작, 끝으로 만들어주는 벡터
			_vector n = vTemp * m_Casecade[i];
			_vector f = vTemp * m_Casecade[i + 1];

			//구간 시작, 끝으로 설정
			XMStoreFloat3(&Frustum[j + 4], XMLoadFloat3(&Frustum[j]) + f);
			XMStoreFloat3(&Frustum[j], XMLoadFloat3(&Frustum[j]) + n);
		}

		//해당 구간을 포함할 바운딩구의 중심을 계산
		_vector vCenter{};
		for (auto& v : Frustum)
		{
			//_matrix matCamInv = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
			//XMStoreFloat3(&v, XMVector3TransformCoord(XMLoadFloat3(&v), matCamInv));
			vCenter = vCenter + XMLoadFloat3(&v);
		}

		vCenter = vCenter / 8.f;
		vCenter.m128_f32[3] = 1.f;

		//바운딩구의 반지름을 계산
		_float fRadius = 0;
		for (auto& v : Frustum)
			fRadius = max(fRadius, XMVector3Length(XMLoadFloat3(&v) - vCenter).m128_f32[0]);

		fRadius = ceil(fRadius * 16.f) / 16.f;

		/* 광원 기준의 뷰 변환행렬. */
		_float4x4		ViewMatrix, ProjMatrix;

		_vector vLightDir = m_pSystemManager->Get_ShadowViewPos();
		_vector  shadowLightPos = vCenter + (vLightDir * -fRadius);
		shadowLightPos.m128_f32[3] = 1.f;

		XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(shadowLightPos, vCenter, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(fRadius * 2.f, fRadius * 2.f, 0.1f, 1000.f));

		ViewMatrixArray[i] = ViewMatrix;
		ProjMatrixArray[i] = ProjMatrix;
	}

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
			return E_FAIL;

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}

	m_pGameInstance->Set_ShadowTransformViewMatrix(ViewMatrixArray);
	m_pGameInstance->Set_ShadowTransformProjMatrix(ProjMatrixArray);

	return S_OK;
}

HRESULT CLightConstruction::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
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

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
}


HRESULT CLightConstruction::Add_Components(void* pArg)
{
	MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TEST, gameobjDesc->wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxMeshLight"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLightConstruction::Bind_ShaderResources()
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

CLightConstruction* CLightConstruction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightConstruction* pInstance = new CLightConstruction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLightConstruction::Clone(void* pArg)
{
	CLightConstruction* pInstance = new CLightConstruction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightConstruction::Free()
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
