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
        _uint iTextType;
        _vector vColor;
    }TEXT_DESC;

public:
    wstring Get_Text() { return m_strText; }
    _vector Get_Color() { return m_vColor; }

public:
    void Set_Text(wstring strText) { m_strText = strText; }
    void Set_Color(_vector vColor) { m_vColor = vColor; }

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

private:
    wstring m_strText = { L"" };
    _vector m_vColor = { };

public:
    static CText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
