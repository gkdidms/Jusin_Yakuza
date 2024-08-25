#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIQTE :
    public CUIScene
{
public:
    enum QTE{BUTTON , CIRCLE ,QTE_END};

protected:
    CUIQTE();
    CUIQTE(const CUIQTE& rhs);
    virtual ~CUIQTE() = default;

public:
    virtual HRESULT Show_Scene()override;//ui �ִ� �غ�(�ʱ�ȭ/���� ������ ����)
    virtual HRESULT Close_Scene()override;//ui �ִ� �غ�(�ʱ�ȭ/������ �ݴ������)

public:
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject, wstring wstrPrototypeTag = TEXT("")) override;
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

    void Set_Key(wstring strKey);
    void Press() { m_isPress = true; }

private:
    _bool m_isPress = false;

public:
    static CUIQTE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};

END