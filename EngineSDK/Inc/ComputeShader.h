#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CComputeShader final :
    public CShader
{
public:
    typedef struct tCOMPUTE_DESC {
        ID3D11UnorderedAccessView* OutputBuffer;
        _uint NumElement;
    }COMPUTE_DESC;

    struct MatrixBuffer {
        _float4x4* matrix;
    };

private:
    CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CComputeShader(const CComputeShader& rhs);
    virtual ~CComputeShader() = default;

public:
    virtual HRESULT Initialize_Prototype(const wstring& strShaderFilePath);
    virtual HRESULT Initialize(void* pArg) override;
    HRESULT Render();

public:
    HRESULT Bind_Matrix(const _float4x4* pMatrix);
    HRESULT Bind_SRV();

private:
    HRESULT Ready_Texture2D(_uint iSizeX, _uint iSizeY);

private:
    ID3D11ComputeShader* m_pComputeShader;
    ID3D11ShaderResourceView* m_pSRV = { nullptr };
    ID3D11UnorderedAccessView* m_pUAV = { nullptr };

    ID3D11Buffer* m_pBoneMatrix = { nullptr };

    _float4 m_vClearColor = {};

public:
    static CComputeShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END