#include "RimChecker.h"
#include "GameInstance.h"

CRimChecker::CRimChecker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSocketObject{ pDevice, pContext }
{
}

CRimChecker::CRimChecker(const CRimChecker& rhs)
	: CSocketObject{ rhs }
{
}

HRESULT CRimChecker::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRimChecker::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CVIBuffer_Instance_Rect::INSTANCE_RECT_DESC RectDesc{};

		RIM_CHECKER_DESC* pDesc = static_cast<RIM_CHECKER_DESC*>(pArg);
		RectDesc.pNeckMatrix = pDesc->pNeckMatrix;
		RectDesc.pLHandMatrix = pDesc->pLHandMatrix;
		RectDesc.pRHandMatrix = pDesc->pRHandMatrix;
		RectDesc.pLFootMatrix = pDesc->pLFootMatrix;
		RectDesc.pRFootMatrix = pDesc->pRFootMatrix;
	
		RectDesc.iNumInstance = 5;
		RectDesc.vRectSize = _float2(1.0f, 1.0f);
		RectDesc.vSize = _float2(20.f, 20.f);
		RectDesc.WorldMatrix = m_pParentMatrix;

	

	if (FAILED(Add_Components(&RectDesc)))
		return E_FAIL;

    return S_OK;
}

void CRimChecker::Priority_Tick(const _float& fTimeDelta)
{
}

void CRimChecker::Tick(const _float& fTimeDelta)
{
	m_pVIBufferCom->Rim_tick(fTimeDelta);
}

void CRimChecker::Late_Tick(const _float& fTimeDelta)
{
	//__super::Late_Tick(fTimeDelta);
	//자기 * 뼈 * 부모
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pParentMatrix));	
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

}

HRESULT CRimChecker::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);

	m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CRimChecker::Add_Components(void* pArg)
{

	/* For.Com_VIBuffer *///본 받아와서 뼈정보 잡아서 넣어주자.
	CVIBuffer_Instance_Rect::INSTANCE_RECT_DESC* pDesc = static_cast<CVIBuffer_Instance_Rect::INSTANCE_RECT_DESC*>(pArg);

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_VIBuffer_Instance_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), pDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture *///텍스처 범위 용 고정
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Texture_Sphere"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

    return S_OK;
}

HRESULT CRimChecker::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
//	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CRimChecker* CRimChecker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRimChecker* pInstance = new CRimChecker(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CRimChecker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRimChecker::Clone(void* pArg)
{
	CRimChecker* pInstance = new CRimChecker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRimChecker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRimChecker::Free()
{
    __super::Free();
}
