#include "UI_Texture.h"

#include "GameInstance.h"

CUI_Texture::CUI_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Object { pDevice, pContext}
{
}

CUI_Texture::CUI_Texture(const CUI_Texture& rhs)
	: CUI_Object {rhs },
	m_fX{rhs.m_fX},
	m_fY{rhs.m_fY },
	m_fSizeX{rhs.m_fSizeX },
	m_fSizeY{rhs.m_fSizeY },
	m_WorldMatrix{rhs.m_WorldMatrix },
	m_ViewMatrix{rhs.m_ViewMatrix },
	m_ProjMatrix{rhs.m_ProjMatrix },
	m_fStartUV{rhs.m_fStartUV },
	m_fEndUV{rhs.m_fEndUV },
	m_vColor{rhs.m_vColor },
	m_iShaderPass{rhs.m_iShaderPass }
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
	m_isParent = pDesc->isParent;
	m_pParentMatrix = pDesc->pParentMatrix;
	m_strParentName = pDesc->strParentName;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.01f, 1.f));

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

HRESULT CUI_Texture::Change_UV(_float2 fStartUV, _float2 fEndUV)
{
	m_fStartUV = fStartUV;
	m_fEndUV = fEndUV;

	if (FAILED(m_pVIBufferCom->EditUV(m_fStartUV, m_fEndUV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Texture::Save_binary(const string strDirectory)
{
	return E_FAIL;
}

HRESULT CUI_Texture::Save_Groupbinary( ofstream& out)
{
	return E_FAIL;
}

HRESULT CUI_Texture::Load_binary()
{
	return E_FAIL;
}

HRESULT CUI_Texture::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->EditUV(m_fStartUV, m_fEndUV)))
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
	if (m_isParent)
	{
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}

	if(4==m_iShaderPass)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;
	}
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
