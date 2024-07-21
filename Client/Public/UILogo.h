#pragma once
#include "UIScene.h"
#include "Client_Defines.h"
BEGIN(Client)
class CUILogo :
    public CUIScene
{
protected:
    CUILogo();
    CUILogo(const CUILogo& rhs);
    virtual ~CUILogo() = default;
public:


    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    virtual void Action() override;
    virtual void OverAction() override;

public:
    static CUILogo* Create(void* pArg = nullptr);
    virtual void Free();
};
END
