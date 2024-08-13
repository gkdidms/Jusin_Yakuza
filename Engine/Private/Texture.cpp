#include "Texture.h"

#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent{ rhs },
	m_iNumTextures{ rhs.m_iNumTextures },
	m_pTextures{ rhs.m_pTextures }
{
	for (auto& iter : m_pTextures)
		Safe_AddRef(iter);
}

HRESULT CTexture::Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures)
{
	m_pTextures.reserve(iNumTextures);

	m_iNumTextures = iNumTextures;
	_tchar szTextureFile[MAX_PATH] = L"";
	_tchar szExt[MAX_PATH] = L"";

	for (size_t i = 0; i < iNumTextures; ++i)
	{
		wsprintf(szTextureFile, strTextureFilePath.c_str(), i);
		_wsplitpath_s(szTextureFile, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH); // 파일 경로에서 확장자 저장

		ID3D11ShaderResourceView* pSRV = { nullptr };
		HRESULT hr = {};

		if (!lstrcmp(szExt, L".dds"))
		{
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFile, nullptr, &pSRV);
		}
		else if (!lstrcmp(szExt, L".tgr"))
		{
			MSG_BOX("Error : TextureFilePath Ext : .tgr");
			hr = E_FAIL;
		}
		else
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFile, nullptr, &pSRV);

		memcpy_s(&m_szTextureFile, sizeof(szTextureFile), &szTextureFile, sizeof(szTextureFile));

		if (FAILED(hr))
			return E_FAIL;

		m_pTextures.push_back(pSRV);
	}

	if (FAILED(Ready_Buffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iNumTextures)
{
	if (m_iNumTextures < iNumTextures)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_pTextures[iNumTextures]);
}

HRESULT CTexture::Ready_Buffer()
{
	for (size_t i = 0; i < m_pTextures.size(); ++i)
	{
		// UAV와 SRV를 지원하는 텍스처 생성
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = 1280;
		textureDesc.Height = 720;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // 예: 32비트 플로트 포맷
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ID3D11Texture2D* pTexture = nullptr;
		if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture)))
			return E_FAIL;

		// SRV 생성
		ID3D11ShaderResourceView* pSRV = nullptr;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, &pSRV)))
			return E_FAIL;

		m_pBufferSRVTexture.emplace_back(pSRV);

		// UAV 생성
		ID3D11UnorderedAccessView* pUAV = nullptr;
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = textureDesc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pDevice->CreateUnorderedAccessView(pTexture, &uavDesc, &pUAV)))
			return E_FAIL;

		m_pBufferUAVTexture.emplace_back(pUAV);
	}

	return S_OK;
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTextures)))
	{
		wstring strFile;
		strFile.assign(pInstance->m_szTextureFile, pInstance->m_szTextureFile + MAX_PATH);
		wstring strAlertFile = TEXT("Failed To Created : CTexture\nFailed Path: ");
		strAlertFile += strFile;
		
		MessageBox(nullptr, strAlertFile.c_str(), L"System Message", MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& iter : m_pTextures)
		Safe_Release(iter);

	for (auto& iter : m_pBufferSRVTexture)
		Safe_Release(iter);

	for (auto& iter : m_pBufferUAVTexture)
		Safe_Release(iter);
}
