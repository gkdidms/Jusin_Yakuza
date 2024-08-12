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

    ID3DBlob* pShaderBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

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
	return S_OK;
}

HRESULT CComputeShader::Render(_uint iGroupCount)
{

    m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);

    m_pContext->Dispatch(iGroupCount, 1, 1);

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
}
