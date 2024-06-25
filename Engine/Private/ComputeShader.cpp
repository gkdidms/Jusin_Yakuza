#include "ComputeShader.h"
#include"GameInstance.h"
CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CComputeShader::CComputeShader(const CComputeShader& rhs)
	: CComponent{ rhs },
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

    _tchar str[MAX_PATH];
    HRESULT hr = FindDXSDKShaderFileCch(str, MAX_PATH, strShaderFilePath.c_str());
    if (FAILED(hr))
        return hr;

    ID3DBlob* pShaderBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    const D3D_SHADER_MACRO defines[] =
    {
        "USE_STRUCTURED_BUFFERS", "1",
        nullptr, nullptr
    };

    LPCSTR pProfile = (m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";   


    if (FAILED(D3DCompileFromFile(str, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CSMain", pProfile, iHlslFlag, 0, &pShaderBlob, &pErrorBlob)))
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
	return S_OK;
}

HRESULT CComputeShader::Render(void* pArg)
{



    if (nullptr != pArg)
    {
        COMPUTE_DESC* pDesc = static_cast<COMPUTE_DESC*>(pArg);

        m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);  

        m_pContext->CSSetUnorderedAccessViews(0, 1, &pDesc->OutputBuffer, nullptr);    

        _uint groupCountX = (pDesc->NumElement + 255) / 256;
        m_pContext->Dispatch(groupCountX, 1, 1);

        m_pContext->CSSetShader(nullptr, nullptr, 0);   
    }


    return S_OK;
}


HRESULT CComputeShader::FindDXSDKShaderFileCch(WCHAR* strDestPath,
    int cchDest,
    LPCWSTR strFilename)
{
    if (!strFilename || strFilename[0] == 0 || !strDestPath || cchDest < 10)
        return E_INVALIDARG;

    // Get the exe name, and exe path
    WCHAR strExePath[MAX_PATH] =
    {
        0
    };
    WCHAR strExeName[MAX_PATH] =
    {
        0
    };
    WCHAR* strLastSlash = nullptr;
    GetModuleFileName(nullptr, strExePath, MAX_PATH);
    strExePath[MAX_PATH - 1] = 0;
    strLastSlash = wcsrchr(strExePath, TEXT('\\'));
    if (strLastSlash)
    {
        wcscpy_s(strExeName, MAX_PATH, &strLastSlash[1]);

        // Chop the exe name from the exe path
        *strLastSlash = 0;

        // Chop the .exe from the exe name
        strLastSlash = wcsrchr(strExeName, TEXT('.'));
        if (strLastSlash)
            *strLastSlash = 0;
    }

    // Search in directories:
    //      .\
    //      %EXE_DIR%\..\..\%EXE_NAME%

    wcscpy_s(strDestPath, cchDest, strFilename);
    if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
        return S_OK;

    swprintf_s(strDestPath, cchDest, L"%s\\..\\..\\%s\\%s", strExePath, strExeName, strFilename);
    if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
        return S_OK;

    // On failure, return the file as the path but also return an error code
    wcscpy_s(strDestPath, cchDest, strFilename);

    return E_FAIL;
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
}
