#include "Texture.h"

#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent{ rhs },
	m_iNumTextures{ rhs.m_iNumTextures },
	m_pTextures{ rhs.m_pTextures },
	m_TextureSize{ rhs.m_TextureSize }
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

		ID3D11Texture2D* pTexture = nullptr;
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

		ID3D11Resource* pResource = nullptr;
		pSRV->GetResource(&pResource);
		if (FAILED(pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture)))
		{
			// 오류 처리
			if (pResource) 
				pResource->Release();
			return S_OK;
		}

		D3D11_TEXTURE2D_DESC textureDesc;
		pTexture->GetDesc(&textureDesc);

		// 텍스처의 너비와 높이를 가져옵니다.
		_float2 TextureSize;
		TextureSize.x = textureDesc.Width;
		TextureSize.y = textureDesc.Height;

		m_TextureSize.emplace_back(TextureSize);

		if (pTexture) Safe_Release(pTexture);
		if (pResource) Safe_Release(pResource);
	}

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

HRESULT CTexture::Bind_ComputeShader(_uint iSlot, _uint iNumTextures)
{
	m_pContext->CSSetShaderResources(iSlot, 1, &m_pTextures[iNumTextures]);
  
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
}


