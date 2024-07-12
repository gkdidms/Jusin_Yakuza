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
    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

public:
    static CUIMoney* Create(void* pArg = nullptr);
    virtual void Free();
};

END