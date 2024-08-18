#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIQTE :
    public CUIScene
{
protected:
    CUIQTE();
    CUIQTE(const CUIQTE& rhs);
    virtual ~CUIQTE() = default;

public:
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject, wstring wstrPrototypeTag = TEXT("")) override;
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

public:
    static CUIQTE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};

END