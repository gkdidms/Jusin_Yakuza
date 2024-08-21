#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIFade :
    public CUIScene
{
protected:
    CUIFade();
    CUIFade(const CUIFade& rhs);
    virtual ~CUIFade() = default;

public:
    _bool isFinished() { return m_isFinished; }

public:
    virtual HRESULT Show_Scene()override;//ui �ִ� �غ�(�ʱ�ȭ/���� ������ ����)
    virtual HRESULT Close_Scene()override;//ui �ִ� �غ�(�ʱ�ȭ/������ �ݴ������)

public:
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject, wstring wstrPrototypeTag = TEXT("")) override;
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    
public:
    void Fade_Out();
    void Fade_In();

private:
    _bool m_isFinished = { false };

public:
    static CUIFade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
