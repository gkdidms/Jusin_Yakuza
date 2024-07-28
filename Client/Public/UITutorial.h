#pragma once
#include "UIScene.h"
#include "Client_Defines.h"
BEGIN(Client)
class CUITutorial :
    public CUIScene
{
protected:
    CUITutorial();
    CUITutorial(const CUITutorial& rhs);
    virtual ~CUITutorial() = default;
public:


    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    virtual void Action() override;
    virtual void OverAction() override;

public:
    static CUITutorial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
