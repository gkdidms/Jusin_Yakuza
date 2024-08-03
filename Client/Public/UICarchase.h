#pragma once
#include "UIScene.h"

#include "Client_Defines.h"
BEGIN(Client)
class CUICarchase :
    public CUIScene
{
    enum TYPE { EIM, ROUND, TYPE_END };

protected:
    CUICarchase();
    CUICarchase(const CUICarchase& rhs);
    virtual ~CUICarchase() = default;

public:
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    virtual void Action() override;
    virtual void OverAction() override;

public:
    HRESULT Add_TargetingRound();

private:
    vector<CUI_Object*> m_Round;

public:
    static CUICarchase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
