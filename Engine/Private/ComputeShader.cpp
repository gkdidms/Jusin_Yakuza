#include "ComputeShader.h"

#include"GameInstance.h"

CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CShader{ pDevice, pContext }
{
}

CComputeShader::CComputeShader(const CComputeShader& rhs)
	: CShader{ rhs },
	m_pComputeShader{rhs.m_pComputeShader }
{
	Safe_AddRef(m_pComputeShader);
}

HRESULT CComputeShader::Initialize_Prototype(const wstring& strShaderFilePath)
{
    _uint iHlslFlag = 0;
#ifdef _DEBUG
    iHlslFlag =  D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL0;
#endif // DEBUG
//
//    _tchar str[MAX_PATH];
//    HRESULT hr = FindDXSDKShaderFileCch(str, MAX_PATH, strShaderFilePath.c_str());
//    if (FAILED(hr))
//        return hr;
//
    ID3DBlob* pShaderBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;
//
//    const D3D_SHADER_MACRO defines[] =
//    {
//        "USE_STRUCTURED_BUFFERS", "1",
//        nullptr, nullptr
//    };
//
//    LPCSTR pProfile = (m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";   
//
//
    if (FAILED(D3DCompileFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS_MAIN", "cs_5_0", iHlslFlag, 0, &pShaderBlob, &pErrorBlob)))
    {
        return E_FAIL;
   }

    if (FAILED(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &m_pComputeShader)))
    {
        Safe_Release(pShaderBlob);
        return E_FAIL;
    }

    Safe_Release(pShaderBlob); 

    return S_OK;
}

HRESULT CComputeShader::Initialize(void* pArg)
{
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = 512;
    cbDesc.Usage = D3D11_USAGE_DEFAULT;

    if (FAILED(m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pBoneMatrix)))
        return E_FAIL;

    Ready_Texture2D(1280, 720);

	return S_OK;
}

HRESULT CComputeShader::Render()
{

    m_pContext->CSSetConstantBuffers(0, 1, &m_pBoneMatrix);
    m_pContext->CSSetShaderResources(0, 1, &m_pSRV);
    m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pUAV, nullptr);
    m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);

    m_pContext->Dispatch(21, 13, 1);

    m_pContext->CSSetShader(nullptr, nullptr, 0);
    m_pContext->CSSetShaderResources(0, 0, nullptr);
    m_pContext->CSSetUnorderedAccessViews(0, 0, nullptr, nullptr);
    m_pContext->CSSetConstantBuffers(0, 0, nullptr);

    return S_OK;
}

HRESULT CComputeShader::Bind_Matrix(const _float4x4* pMatrix)
{

    m_pContext->UpdateSubresource(m_pBoneMatrix, 0, nullptr, pMatrix, 0, 0);

    return S_OK;
}

HRESULT CComputeShader::Bind_SRV()
{
    m_pContext->VSSetShaderResources(0, 1, &m_pSRV);
    return S_OK;
}

//만들어두고 안되면 이거 실행
HRESULT CComputeShader::Ready_Texture2D(_uint iSizeX, _uint iSizeY)
{
    //D3D11_VIEWPORT ViewPort{};
        //_uint iViewPortNum = 1;
        //m_pContext->RSGetViewports(&iViewPortNum, &ViewPort);

    ID3D11Texture2D* pTexture2D = nullptr;
    D3D11_TEXTURE2D_DESC	TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = iSizeX;
    TextureDesc.Height = iSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture2D)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(pTexture2D, nullptr, &m_pSRV)))
        return E_FAIL;

    D3D11_UNORDERED_ACCESS_VIEW_DESC UavDesc{};
    UavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    UavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    UavDesc.Texture2D.MipSlice = 0;
    if (FAILED(m_pDevice->CreateUnorderedAccessView(pTexture2D, &UavDesc, &m_pUAV)))
        return E_FAIL;

    m_vClearColor = _float4(0.f, 0.f, 0.f, 0.f);

    Safe_Release(pTexture2D);

    return S_OK;
}

CComputeShader* CComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath)
{
    CComputeShader* pInstance = new CComputeShader(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strShaderFilePath)))
    {
        MSG_BOX("Failed To Created : CComputeShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CComputeShader::Clone(void* pArg)
{
    CComputeShader* pInstance = new CComputeShader(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CComputeShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CComputeShader::Free()
{
    __super::Free();

    Safe_Release(m_pComputeShader);
    m_pBoneMatrix->Release();

    Safe_Release(m_pSRV);
    Safe_Release(m_pUAV);
}
