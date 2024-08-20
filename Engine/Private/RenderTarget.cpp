#include "RenderTarget.h"

#include "GameInstance.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isCompute, _uint iArrayCount)
{
	//D3D11_VIEWPORT ViewPort{};
	//_uint iViewPortNum = 1;
	//m_pContext->RSGetViewports(&iViewPortNum, &ViewPort);

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = iArrayCount;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = isCompute ? D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS : D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (!isCompute)
	{
		if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
			return E_FAIL;
	}
	else
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = ePixelFormat;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pTexture2D, &uavDesc, &m_pUAV)))
			return E_FAIL;
	}


	if (iArrayCount == 1)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC ShaderDesc;
		ShaderDesc.Format = ePixelFormat;
		ShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		ShaderDesc.Texture2D.MipLevels = 1;
		ShaderDesc.Texture2D.MostDetailedMip = 0;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, &ShaderDesc, &m_pSRV)))
			return E_FAIL;
	}
	else
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC ShaderDesc{ ePixelFormat, D3D11_SRV_DIMENSION_TEXTURE2DARRAY, 0, 0 };
		ShaderDesc.Texture2DArray.ArraySize = iArrayCount;
		ShaderDesc.Texture2DArray.FirstArraySlice = 0;
		ShaderDesc.Texture2DArray.MipLevels = 1;
		ShaderDesc.Texture2DArray.MostDetailedMip = 0;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, &ShaderDesc, &m_pSRV)))
			return E_FAIL;
	}

	m_vClearColor = vClearColor;

	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);

	return S_OK;
}

HRESULT CRenderTarget::Bind_SVR(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRV(pConstantName, m_pSRV);
}

void CRenderTarget::Bind_ComputeSVR(_uint iSlot)
{
	m_pContext->CSSetShaderResources(iSlot, 1, &m_pSRV);
}

void CRenderTarget::Bind_ComputeUAV(_uint iSlot)
{
	m_pContext->CSSetUnorderedAccessViews(iSlot, 1, &m_pUAV, nullptr);
}

HRESULT CRenderTarget::Copy_Resource(ID3D11Texture2D* pDesc)
{
	m_pContext->CopyResource(pDesc, m_pTexture2D);

	return S_OK;
}

HRESULT CRenderTarget::Create_Texture(const wstring& strSaveFilePath)
{
	return SaveDDSTextureToFile(m_pContext, m_pTexture2D, strSaveFilePath.c_str());
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	D3D11_VIEWPORT ViewportDesc{};
	_uint iNumViewport = 1;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = fSizeX;
	m_WorldMatrix._22 = fSizeY;
	m_WorldMatrix._41 = fX - ViewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -fY + ViewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer, _bool isArray)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_isArray", &isArray, sizeof(_bool))))
		return E_FAIL;

	if (isArray)
	{
		if (FAILED(pShader->Bind_SRV("g_TextureArray", m_pSRV)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(pShader->Bind_SRV("g_Texture", m_pSRV)))
			return E_FAIL;
	}


	pShader->Begin(0);
	pVIBuffer->Render();

	return S_OK;
}
#endif // _DEBUG

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isCompute, _uint iArrayCount)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePixelFormat, vClearColor, isCompute, iArrayCount)))
		Safe_Release(pInstance);

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pTexture2D);
	Safe_Release(m_pRTV);
	Safe_Release(m_pSRV);
	Safe_Release(m_pUAV);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}