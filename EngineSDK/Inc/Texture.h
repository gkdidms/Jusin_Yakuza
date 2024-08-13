#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final:
    public CComponent
{
private:
    CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTexture(const CTexture& rhs);
    virtual ~CTexture() = default;

public:
    virtual HRESULT Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures);
    virtual HRESULT Initialize(void* pArg) override;

public: // 한장만 있다고 생각하고 구현
    ID3D11ShaderResourceView* Get_TextureSRV() { return m_pTextures[0]; } // 텍스쳐 dds 버퍼값
    _float2 Get_TextureSize(_uint iIndex) { return m_TextureSize[iIndex]; }

public:
    HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iNumTextures = 0);
    HRESULT Bind_ComputeShader(_uint iSlot, _uint iNumTextures = 0);

private:
    _tchar m_szTextureFile[MAX_PATH] = L"";
    _uint m_iNumTextures = { 0 };
    vector<ID3D11ShaderResourceView*> m_pTextures;

    vector<_float2> m_TextureSize = {};

public:
    static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures = 1);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END
