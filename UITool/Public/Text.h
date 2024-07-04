#pragma once
#include "UI_Texture.h"


BEGIN(UITool)
class CText :
    public CUI_Texture
{
public:
    typedef struct tTextDesc : public UI_TEXTURE_DESC
    {
        wstring strText;

    }TEXT_DESC;

public:
    wstring Get_Text() { return m_strText; }

public:
    void Set_Text(wstring strText) { m_strText = strText; }

private:
    CText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CText(const CText& rhs);
    virtual ~CText() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    virtual HRESULT Save_binary(const string strDirectory)override;
    virtual HRESULT Save_Groupbinary(ofstream& out)override;
private:

    wstring m_strText = { L"" };

public:
    static CText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
