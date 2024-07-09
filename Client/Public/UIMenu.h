#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIMenu :
    public CUIScene
{

private:
    CUIMenu();
    CUIMenu(const CUIMenu& rhs);
    virtual ~CUIMenu() = default;

public:
    virtual HRESULT Initialize() override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

    virtual void Action(_int EventNum) override;
public:
    static CUIMenu* Create();
    virtual void Free();
};
END
