#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUILife :
    public CUIScene
{
private:
    CUILife();
    CUILife(const CUILife& rhs);
    virtual ~CUILife() = default;

public:

    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

public:
    static CUILife* Create(void* pArg=nullptr);
    virtual void Free();
};
END
