#pragma once
#include "UIScene.h"

BEGIN(Client)
class CUIMoney :
    public CUIScene
{
protected:
    CUIMoney();
    CUIMoney(const CUIMoney& rhs);
    virtual ~CUIMoney() = default;

public:
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

public:
    static CUIMoney* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,void* pArg = nullptr);
    virtual void Free();
};

END