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
    enum TEXTURE_TYPE { IMG, BTN, TEXT, TEXT_FORMAT, GROUP, TEXTURE_TYPE_END };
    enum MOVE_TYPE { UP, DOWN, MOVE_END };
    typedef struct {
        _uint iTextureType;
        wstring strFilePath;
        wstring strFileName;
        wstring strBtnClickFilePath;
        wstring strBtnClickFileName;
        wstring strText;
        _uint iTextType;
        _vector vColor;

        string strName;
    } OBJ_MNG_DESC;

private:
    CObject_Manager();
    virtual ~CObject_Manager() = default;

public:
    map<const wstring, vector<class CUI_Object*>> Get_Objects() { return m_Objects; }
    vector<class CUI_Object*> Get_GroupObject(const wstring& strTag);

    map<const wstring, vector<class CUI_Object*>> Get_BinaryObjects() { return m_BinaryObjects; }
    vector<class CUI_Object*> Get_GroupBinaryObject(const wstring& strTag);

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Tick(const _float& fTimeDelta);
    void Late_Tick(const _float& fTimeDelta);
    HRESULT Render();

public:
    HRESULT Add_Group(const wstring& strObjectTag);
    HRESULT Add_RenderTextureObject(const wstring& strObjectTag, void* pArg);
    HRESULT Add_BinaryObject(const wstring& strObjectTag, void* pArg);

    HRESULT Copy_Group(const wstring& strTag);
    HRESULT Remove_Group(const wstring& strTag);
    HRESULT Remove_Object(const wstring& strTag, _uint iIndex);
    HRESULT Create_Texture(_uint iIndex, const wstring& strFilePath);

    HRESULT Move_ObjectIndex(const wstring& strTag, _uint iIndex, _uint iMoveType);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };

    vector<CRenderTarget*> m_Texture2Ds;
    vector<CRenderTarget*> m_BinaryTexture2Ds;
    vector< CRenderTarget*> m_CopyBackTexture2Ds;
    map<const wstring, vector<class CUI_Object*>> m_Objects;
    map<const wstring, vector<class CUI_Object*>> m_BinaryObjects;

    
private: // Render��
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
    vector<class CUI_Object*>* Find_RenderTextureObject(const wstring& strObjectTag);
    vector<class CUI_Object*>* Find_BinaryObject(const wstring& strObjectTag);

    HRESULT Create_Texture2D();
    HRESULT Create_BinaryTexture2D();
    HRESULT Texture2D_Begin(vector<CRenderTarget*> Textures, _uint iIndex, _bool isClear = true);
    HRESULT Texture2D_End();

public: 
    static void Release_ObjectManager();
    virtual void Free();
};
END
