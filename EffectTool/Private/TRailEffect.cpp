#include "TRailEffect.h"
#include "GameInstance.h"

CTRailEffect::CTRailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect{pDevice , pContext}
{
}

CTRailEffect::CTRailEffect(const CTRailEffect& rhs)
	:CEffect{rhs}
{
}

HRESULT CTRailEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTRailEffect::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		TRAIL_DESC* pDesc = static_cast<TRAIL_DESC*>(pArg);

		m_TrailDesc = pDesc -> Trail_Desc;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vStartPos));
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CTRailEffect::Priority_Tick(const _float& fTimeDelta)
{
}

void CTRailEffect::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_pVIBufferCom->Add_Trail(fTimeDelta, m_pTransformCom->Get_WorldMatrix());
}

void CTRailEffect::Late_Tick(const _float& fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
}

HRESULT CTRailEffect::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void* CTRailEffect::Get_Instance()
{
	return &m_TrailDesc;
}

HRESULT CTRailEffect::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom),&m_TrailDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Shader_Trail"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Trail"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTRailEffect::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CTRailEffect* CTRailEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTRailEffect* pInstance = new CTRailEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTRailEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTRailEffect::Clone(void* pArg)
{
	CTRailEffect* pInstance = new CTRailEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTRailEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTRailEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
