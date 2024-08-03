#include "..\Public\Item.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Transform.h"

#include "Mesh.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CItem::CItem(const CItem& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CItem::Initialize(void* pArg)
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

			CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(m_iCurrentLevel, TEXT("Prototype_Component_Collider"), &ColliderDesc));

			m_vColliders.push_back(pCollider);

		}

		// 플레이어 matrix 추가
		m_pPlayerMatrix = gameobjDesc->vPlayerMatrix;

		// 잠깐 초기화해두기 -> 나중에 OFFSET으로 사용 (플레이어어와 충돌했을 경우 위치 조절)
		m_vOffsetMatrix = gameobjDesc->vOffsetMatrix;

		m_WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	}

	m_Casecade = { 0.f, 10.f, 24.f, 40.f };


	m_eItemMode = ITEM_IDLE;

	return S_OK;
}

void CItem::Priority_Tick(const _float& fTimeDelta)
{
}

void CItem::Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_vDecals)
		iter->Tick(fTimeDelta);

	// 잡을 수 있을때와 아닐때를 구별
	Set_Item_Mode();

	// Bright 처리
	bool		bBright = m_bBright;

	if (0 != m_fBrightTime && ITEM_BRIGHT != m_eItemMode)
	{
		// bright 모드 아닌데 아직 깜빡거림이 안끝났을때
		m_bBright = true;

		m_fBrightTime += 0.04;

		if (4 < m_fBrightTime)
		{
			m_fBrightTime = 0;
			m_bBright = false;
		}
	}
	else if (ITEM_BRIGHT == m_eItemMode)
	{
		// bright 모드 
		m_bBright = true;

		m_fBrightTime += 0.04;

		if (4 < m_fBrightTime)
		{
			m_fBrightTime = 0;
		}
	}
	else if (0 == m_fBrightTime && ITEM_BRIGHT != m_eItemMode)
	{
		// bright 모드 아님
		m_bBright = false;
	}
	else if (true == m_bCurGrab)
	{
		// grab이면 바로 꺼짐
		m_bBright = false;
		m_fBrightTime = 0;
	}






	if (ITEM_GRAB == m_eItemMode)
	{
		// 잡고 있을때 
		_matrix		offsetMatrix = XMLoadFloat4x4(&m_vOffsetMatrix);

		offsetMatrix.r[0] = XMVector3Normalize(offsetMatrix.r[0]);
		offsetMatrix.r[1] = XMVector3Normalize(offsetMatrix.r[1]);
		offsetMatrix.r[2] = XMVector3Normalize(offsetMatrix.r[2]);

		_matrix PlayerMatrix, ParentMatrix;
		PlayerMatrix = ParentMatrix = XMMatrixIdentity();

		if (nullptr != m_pPlayerMatrix)
			PlayerMatrix = XMLoadFloat4x4(m_pPlayerMatrix);

		if (nullptr != m_vParentMatrix)
			ParentMatrix = XMLoadFloat4x4(m_vParentMatrix);

		XMStoreFloat4x4(&m_WorldMatrix, /*offsetMatrix **/ ParentMatrix * PlayerMatrix);

		XMMATRIX worldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
		m_pTransformCom->Set_WorldMatrix(worldMatrix);
	}
	else
	{
		//XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
		//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix));
	}

	if (m_isThrowing)
		Throwing(fTimeDelta);	

	_matrix		worldMatrix = m_pTransformCom->Get_WorldMatrix();

#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		iter->Tick(worldMatrix);
#endif

}

void CItem::Late_Tick(const _float& fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		if (true == m_bBright)
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
		}
		else
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
		}


		for (auto& iter : m_vDecals)
			iter->Late_Tick(fTimeDelta);
	}

}

HRESULT CItem::Render()
{
#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		m_pGameInstance->Add_DebugComponent(iter);
#endif

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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightTime", &m_fBrightTime, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bBright", &m_bBright, sizeof(_bool))))
			return E_FAIL;


		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CItem::Render_LightDepth()
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
		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
}



CCollider* CItem::Get_Collider()
{
	return dynamic_cast<CCollider*>(*m_vColliders.begin());
}

_bool CItem::Decrease_Life()
{
	m_iLife--;

	// 감소 이후 생명이 0보다 작아진다면 오브젝트 사망처리
	if (0 > m_iLife)
	{
		Set_ObjectDead();
		return false;
	}

	return true;
}

void CItem::Throw_On(THROW_INFO_DESC& Desc)
{
	if (m_isThrowing) return;

	m_eItemMode = ITEM_IDLE;

	m_isThrowing = true;
	m_ThrowInfo.fThrowSpeed = Desc.fThrowSpeed;
	m_ThrowInfo.vThrowDir = Desc.vThrowDir;

	m_pPlayerMatrix = nullptr;
	m_vParentMatrix = nullptr;
}

void CItem::Throwing(const _float& fTimeDelta)
{
	m_fThrowTimer += fTimeDelta;
	if (THROW_TIME <= m_fThrowTimer)
	{
		m_fThrowTimer = 0.f;
		m_isThrowing = false;


		// TODO: 오브젝트를 죽음처리하면 디졸브를 실행해야한다.
		Set_ObjectDead();
	}

	m_pTransformCom->Go_Move_Custum(m_ThrowInfo.vThrowDir, m_ThrowInfo.fThrowSpeed, fTimeDelta);
	//m_pTransformCom->Go_Straight(fTimeDelta);
}

void CItem::Set_Item_Mode()
{
	if (nullptr == m_pPlayerMatrix) return;

	_vector vPlayerPosition;
	memcpy(&vPlayerPosition, m_pPlayerMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));

	float fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vPlayerPosition));

	// 현재 잡고 있지 않음 + 근처에 있을때
	if (fDistance < BRIGHT_DISTANCE && false == m_bCurGrab)
	{	
		m_bBright = true;
		
		m_eItemMode = ITEM_BRIGHT;			
	}
	else if (false == m_bCurGrab)
	{
		// 잡고 있지도 않고 근처에 없을때
		m_eItemMode = ITEM_IDLE;

		m_bBright = false;
	}

}

HRESULT CItem::Add_Components(void* pArg)
{
	MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, gameobjDesc->wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_MeshItem"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;



	return S_OK;
}

HRESULT CItem::Bind_ShaderResources()
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

CItem* CItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem* pInstance = new CItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem::Clone(void* pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
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
