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
        wstring strClickFilePath; // Ŭ���������� �̹���
        wstring strClickFileName;
        _float2 ClickStartUV;
        _float2 ClickEndUV = { 1.f,1.f };
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

public:
    HRESULT Chage_ClickUV(_float2 StartUV, _float2 EndUV);
    
    _float2 Get_ClickStartUV() { return m_ClickStartUV; }
    _float2 Get_ClickEndUV() { return m_ClickEndUV; }

    wstring Get_ClickFileName() { return m_StrClickFileName; }
    wstring Get_ClickFilePath() { return m_strClickFilePath; }

    void Set_Click(_bool Click) { m_isClick = Click; }
    _bool Get_Click() { return m_isClick; }

public:
    virtual HRESULT Save_binary(const string strDirectory)override;
    virtual HRESULT Save_Groupbinary(ofstream& out)override;
private:
    CTexture* m_pClickTextureCom = { nullptr };
    CVIBuffer_Rect* m_pClickVIBufferCom = { nullptr };

    wstring m_strClickFilePath = { L"" };
    wstring m_StrClickFileName = { L"" };
    
private:
    _bool m_isClick = { false };
    _float2 m_ClickStartUV = { 0.f ,0.f };
    _float2 m_ClickEndUV = { 1.f, 1.f };

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    static CBtn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
