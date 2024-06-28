#pragma once
#include "UI_Texture.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(UITool)
class CBtn :
    public CUI_Texture
{
public:
    typedef struct tBtnDesc : public UI_TEXTURE_DESC {
        wstring strClickFilePath; // 클릭했을때의 이미지
        wstring strClickFileName;
    }BTN_DESC;
private:
    CBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBtn(const CBtn& rhs);
    virtual ~CBtn() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    CTexture* m_pClickTextureCom = { nullptr };

    wstring m_strClickFilePath = { L"" };
    wstring m_StrClickFileName = { L"" };
    
private:
    _bool m_isClick = { false };

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    static CBtn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
