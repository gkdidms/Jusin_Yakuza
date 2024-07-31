#include "Highway_Kiryu.h"

#include "GameInstance.h"
#include "Mesh.h"

CHighway_Kiryu::CHighway_Kiryu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CHighway_Kiryu::CHighway_Kiryu(const CHighway_Kiryu& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CHighway_Kiryu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHighway_Kiryu::Initialize(void* pArg)
{
	CARCHASE_KIRYU_DESC* pDesc = static_cast<CARCHASE_KIRYU_DESC*>(pArg);
	pTaxiMatrix = pDesc->pTaxiWorldMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_wstrModelName = TEXT("Kiryu_CarChase");

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	return S_OK;
}

void CHighway_Kiryu::Priority_Tick(const _float& fTimeDelta)
{
}

void CHighway_Kiryu::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pModelCom->Set_AnimationIndex(0);
	m_pModelCom->Play_Animation(fTimeDelta, false);
}

void CHighway_Kiryu::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CHighway_Kiryu::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		if (pMesh->Get_AlphaApply())
			m_pShaderCom->Begin(1);     //블랜드
		else
			m_pShaderCom->Begin(0);		//디폴트

		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}

void CHighway_Kiryu::Change_Animation()
{
}

HRESULT CHighway_Kiryu::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Kiryu_CarChase"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHighway_Kiryu::Bind_ResourceData()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", pTaxiMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CHighway_Kiryu* CHighway_Kiryu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHighway_Kiryu* pInstance = new CHighway_Kiryu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CHighway_Kiryu::Clone(void* pArg)
{
	CHighway_Kiryu* pInstance = new CHighway_Kiryu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHighway_Kiryu::Free()
{
	__super::Free();
}

string CHighway_Kiryu::Get_CurrentAnimationName()
{
	return m_pModelCom->Get_AnimationName(m_pModelCom->Get_CurrentAnimationIndex());
}
