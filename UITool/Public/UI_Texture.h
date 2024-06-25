#pragma once
#include "GameObject.h"
#include "UITool_Defines.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(UITool)
class CUI_Texture :
    public CGameObject
{
public:
    typedef struct tUITextureDesc : public CGameObject::GAMEOBJECT_DESC {
        wstring strTextureFilePath;
        wstring strTextureFileName;
    } UI_TEXTURE_DESC;

public:
    void Set_SizeX(_float fSizeX) { m_fSizeX = fSizeX; }
    void Set_SizeY(_float fSizeY) { m_fSizeY = fSizeY; }
    void Set_ShaderPass(_uint iPass) { m_iShaderPass = iPass; }

public:
    _float Get_SizeX() { return m_fSizeX; }
    _float Get_SizeY() { return m_fSizeY; }
    _uint Get_ShaderPass() { return m_iShaderPass; }

    wstring Get_FileName() { return m_strTextureName; }
    wstring Get_FilePath() { return m_strTextureFilePath; }

protected:
    CUI_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_Texture(const CUI_Texture& rhs);
    virtual ~CUI_Texture() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

protected:
    _float							m_fX, m_fY, m_fSizeX, m_fSizeY;
    _float4x4						m_ViewMatrix, m_ProjMatrix;

protected:
    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };

    wstring m_strTextureFilePath = { L"" };
    wstring m_strTextureName = { L"" };

protected:
    _uint m_iShaderPass = { 0 };

protected:
    HRESULT Add_Components();
    HRESULT Bind_ResourceData();

public:
    virtual void Free();
};
END
