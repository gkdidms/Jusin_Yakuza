#include "UI_Texture.h"

#include "GameInstance.h"

CUI_Texture::CUI_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Object { pDevice, pContext}
{
}

CUI_Texture::CUI_Texture(const CUI_Texture& rhs)
	: CUI_Object {rhs }
{
}

HRESULT CUI_Texture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Texture::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UI_TEXTURE_DESC* pDesc = static_cast<UI_TEXTURE_DESC*>(pArg);
	m_strTextureFilePath = pDesc->strTextureFilePath;
	m_strTextureName = pDesc->strTextureFileName;
	m_iTypeIndex = pDesc->iTypeIndex;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.0f));
	
	return S_OK;
}

void CUI_Texture::Priority_Tick(const _float& fTimeDelta)
{
}

void CUI_Texture::Tick(const _float& fTimeDelta)
{
}

void CUI_Texture::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CUI_Texture::Render()
{
	return S_OK;
}

HRESULT CUI_Texture::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, m_strTextureFilePath, 1);
	if (nullptr == m_pTextureCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Texture::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
}

void CUI_Texture::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
