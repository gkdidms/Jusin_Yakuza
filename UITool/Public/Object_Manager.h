#pragma once
#include "Base.h"
#include "UITool_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CRenderTarget;
class CShader;
class CVIBuffer_Rect;
END

BEGIN(UITool)
class CObject_Manager :
    public CBase
{
    DECLARE_SINGLETON(CObject_Manager)

public:
    enum TEXTURE_TYPE { IMG, BTN, TEXT, TEXT_FORMAT, TEXTURE_TYPE_END };
    typedef struct {
        _uint iTextureType;
        wstring strFilePath;
        wstring strFileName;
        wstring strText;
        _uint iTextType;
        _vector vColor;
    } OBJ_MNG_DESC;

private:
    CObject_Manager();
    virtual ~CObject_Manager() = default;

public:
    map<const wstring, vector<class CUI_Texture*>> Get_Objects() { return m_Objects; }
    vector<class CUI_Texture*> Get_GroupObject(const wstring& strTag);

public:
    HRESULT Copy_Group(const wstring& strTag);
    HRESULT Remove_Group(const wstring& strTag);
    HRESULT Remove_Object(const wstring& strTag, _uint iIndex);
    HRESULT Create_Texture(_uint iIndex, const wstring& strFilePath);

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Tick(const _float& fTimeDelta);
    void Late_Tick(const _float& fTimeDelta);
    HRESULT Render();

public:
    HRESULT Add_Group(const wstring& strObjectTag);
    HRESULT Add_Object(const wstring& strObjectTag, void* pArg);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };

    vector<CRenderTarget*> m_Texture2Ds;
    map<const wstring, vector<class CUI_Texture*>> m_Objects;



private: // Render¿ë
    ID3D11RenderTargetView* m_pBackBufferView = { nullptr };
    ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };

    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

    _float m_fDebugStart = { 50.f };
    float m_iTextureHeightCount = { 0.f };
    float m_iTextureWidthCount = { 1.f };

    _float4x4 m_WorldMatrix;
    _float4x4 m_ViewMatrix;
    _float4x4 m_ProjMatrix;

private:
    vector<class CUI_Texture*>* Find_Object(const wstring& strObjectTag);

    HRESULT Create_Texture2D();
    HRESULT Texture2D_Begin(_uint iIndex);
    HRESULT Texture2D_End();

public: 
    static void Release_ObjectManager();
    virtual void Free();
};
END
