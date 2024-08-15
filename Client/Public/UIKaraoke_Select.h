#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIKaraoke_Select :
    public CUIScene
{
protected:
    CUIKaraoke_Select();
    CUIKaraoke_Select(const CUIKaraoke_Select& rhs);
    virtual ~CUIKaraoke_Select() = default;


public:
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject) override;
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

    virtual void Action() override;
    virtual void OverAction() override;

    CUI_Object* m_Disc;
    CGroup* m_Select;
    vector<CUI_Object*> m_UIGrade;
    vector<CUI_Object*> m_UIName;


   
public:
    static CUIKaraoke_Select* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();

};
END
