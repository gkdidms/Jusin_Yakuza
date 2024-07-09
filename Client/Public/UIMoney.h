#pragma once
#include "UIScene.h"

BEGIN(Client)
class CUIMoney :
    public CUIScene
{
private:
    CUIMoney();
    CUIMoney(const CUIMoney& rhs);
    virtual ~CUIMoney() = default;

public:
    virtual HRESULT Initialize() override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

public:
    static CUIMoney* Create();
    virtual void Free();
};

END