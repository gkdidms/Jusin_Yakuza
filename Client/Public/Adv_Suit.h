#pragma once
#include "Adventure.h"

BEGIN(Client)
class CAdv_Suit :
    public CAdventure
{
private:
    CAdv_Suit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CAdv_Suit(const CAdv_Suit& rhs);
    virtual ~CAdv_Suit() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    HRESULT Add_Components();
    
public:
    static CAdv_Suit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
