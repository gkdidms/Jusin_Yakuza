#include "..\Public\Construction.h"

#include "GameInstance.h"
#include "Transform.h"

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

	//if (nullptr != pArg)
	//{
	//	GAMEOBJECT_DESC* gameobjDesc = (GAMEOBJECT_DESC*)pArg;
	//	m_iObjID = gameobjDesc->iObjID;
	//}

	if (FAILED(Add_Components()))
		return E_FAIL;



	return S_OK;
}

void CConstruction::Priority_Tick(const _float& fTimeDelta)
{
}

void CConstruction::Tick(const _float& fTimeDelta)
{
	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CConstruction::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CConstruction::Render()
{
	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;


	//_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
	//		return E_FAIL;

	//	m_pShaderCom->Begin(0);

	//	m_pModelCom->Render(i);
	//}


	return S_OK;
}

HRESULT CConstruction::Add_Components()
{
	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Shrub"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	///* For.Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh_Shrub"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CConstruction::Bind_ShaderResources()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
	//	return E_FAIL;

	return S_OK;
}

CConstruction* CConstruction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CConstruction* pInstance = new CConstruction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPalm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CConstruction::Clone(void* pArg)
{
	CConstruction* pInstance = new CConstruction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPalm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConstruction::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
