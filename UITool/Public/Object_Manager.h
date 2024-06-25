#pragma once
#include "Base.h"
#include "UITool_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(UITool)
class CObject_Manager :
    public CBase
{
    DECLARE_SINGLETON(CObject_Manager)

public:
    enum TEXTURE_TYPE { IMG, BTN, TEXTURE_TYPE_END };
    typedef struct {
        _uint iTextureType;
        wstring strFilePath;
        wstring strFileName;
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

    vector<ID3D11Texture2D*> m_Texture2Ds;
    map<const wstring, vector<class CUI_Texture*>> m_Objects;


private:
    vector<class CUI_Texture*>* Find_Object(const wstring& strObjectTag);

public: 
    static void Release_ObjectManager();
    virtual void Free();
};
END
