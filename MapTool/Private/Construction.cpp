#include "..\Public\Construction.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Imgui_Manager.h"

CConstruction::CConstruction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CConstruction::CConstruction(const CConstruction& rhs)
	: CGameObject{ rhs }
{
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
		m_iObjectPropertyType = gameobjDesc->iObjPropertyType;

		for (int i = 0; i < gameobjDesc->iDecalNum; i++)
		{
			DECAL_DESC  tDecal;
			CTexture*	pTexture;
			XMMATRIX    vStartPos;

			CDecal::DECALOBJ_DESC		decalObjDesc{};
			decalObjDesc.iMaterialNum = gameobjDesc->pDecal[i].iMaterialNum;
			decalObjDesc.pTexture = m_pModelCom->Copy_DecalTexture(decalObjDesc.iMaterialNum);
			decalObjDesc.vStartPos = XMLoadFloat4x4(&gameobjDesc->pDecal[i].vTransform);

			CDecal* pDecal = dynamic_cast<CDecal*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Decal"), &decalObjDesc));

			m_vDecals.push_back(pDecal);
		}
	}


	return S_OK;
}

void CConstruction::Priority_Tick(const _float& fTimeDelta)
{
}

void CConstruction::Tick(const _float& fTimeDelta)
{
	//m_pModelCom->Play_Animation(fTimeDelta);

	for (auto& iter : m_vDecals)
		iter->Tick(fTimeDelta);
}

void CConstruction::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	for (auto& iter : m_vDecals)
		iter->Late_Tick(fTimeDelta);
}

HRESULT CConstruction::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		/*m_pShaderCom->Begin(m_iShaderPassNum);*/

		bool	bFindDecal = false;

		if (true == m_bFindDecalMesh)
		{
			for (int j = 0; j < m_iDecalMeshCnt; j++)
			{
				if (m_pDecalMeshIndex[j] == i)
				{
					bFindDecal = true;
				}
			}
		}

		if (true == bFindDecal)
		{

			/* decal�� ���Ե� �޽��� ���������� */
			m_pShaderCom->Begin(1);
		}
		else
		{
			m_pShaderCom->Begin(0);
		}

		m_pModelCom->Render(i);
	}


	return S_OK;
}

int CConstruction::Get_ObjPlaceDesc(OBJECTPLACE_DESC* objplaceDesc)
{
	/* ���̳ʸ�ȭ�ϱ� ���� ������ ���� */
	
	XMStoreFloat4x4(&objplaceDesc->vTransform, m_pTransformCom->Get_WorldMatrix());

	//objPlaceDesc.iLayer = m_iLayerNum;
	
	string strName = m_pGameInstance->WstringToString(m_wstrModelName);
	strcpy(objplaceDesc->strModelCom, strName.c_str());

	objplaceDesc->iShaderPassNum = m_iShaderPassNum;
	objplaceDesc->iObjType = m_iObjectType;
	objplaceDesc->iObjPropertyType = m_iObjectPropertyType;

	/* Decal �߰� */
	objplaceDesc->iDecalNum = m_vDecals.size();
	
	if (0 < objplaceDesc->iDecalNum)
	{
		objplaceDesc->pDecals = new DECAL_DESC_IO[objplaceDesc->iDecalNum];

		for (int i = 0; i < objplaceDesc->iDecalNum ; i++)
		{
			m_vDecals[i]->Get_Decal_Desc_IO(&objplaceDesc->pDecals[i]);
		}
	}


	/* layer�� return �������� */
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

void CConstruction::Edit_GameObject_Information(CConstruction::MAPOBJ_DESC mapDesc)
{
	m_iLayerNum = mapDesc.iLayer;
	m_iShaderPassNum = mapDesc.iShaderPass;
	m_iObjectType = mapDesc.iObjType;
	m_iObjectPropertyType = mapDesc.iObjPropertyType;
}

CConstruction::MAPOBJ_DESC CConstruction::Send_GameObject_Information()
{
	MAPOBJ_DESC		mapObjDesc;

	mapObjDesc.iLayer = m_iLayerNum;
	mapObjDesc.iShaderPass = m_iShaderPassNum;
	mapObjDesc.iObjType = m_iObjectType;
	mapObjDesc.iObjPropertyType = m_iObjectPropertyType;

	return mapObjDesc;
}

void CConstruction::On_Find_DecalMesh(int* iNumMesh, int iCnt)
{
	m_pDecalMeshIndex = iNumMesh;
	m_iDecalMeshCnt = iCnt;
	m_bFindDecalMesh = true;
}

void CConstruction::Off_Find_DecalMesh()
{
	m_bFindDecalMesh = false;
}

void CConstruction::Add_Decal(CDecal* pDecal)
{
	m_vDecals.push_back(pDecal);
	Safe_AddRef(pDecal);
}

void CConstruction::Delete_Decal(int iIndex)
{
	if (1 == m_vDecals.size())
	{
		vector<CDecal*>::iterator		iter = m_vDecals.begin();

		if (0 != iIndex)
		{
			for (int i = 0; i < iIndex; i++)
			{
				iter++;
			}
		}

		Safe_Release(*iter);
		m_vDecals.erase(iter);

		m_vDecals.clear();
	}
	else
	{
		vector<CDecal*>::iterator		iter = m_vDecals.begin();

		if (0 != iIndex)
		{
			for (int i = 0; i < iIndex; i++)
			{
				iter++;
			}
		}

		Safe_Release(*iter);
		m_vDecals.erase(iter);
	}


}

HRESULT CConstruction::Add_Components(void* pArg)
{
	MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_RUNMAP, gameobjDesc->wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Shader_VtxMesh"),
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

	if (FAILED(m_pShaderCom->Bind_ValueFloat("g_fObjID", m_fObjID)))
		return E_FAIL;

	bool	bWrite;
	if (CImgui_Manager::IDWRIE::OBJPLACE == CImgui_Manager::GetInstance()->Get_Write())
	{
		bWrite = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bWriteID", &bWrite, sizeof(bool))))
			return E_FAIL;
	}
	else
	{
		bWrite = false;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bWriteID", &bWrite, sizeof(bool))))
			return E_FAIL;
	}


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

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
