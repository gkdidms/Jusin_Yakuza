#pragma once
#include "Adventure.h"

BEGIN(Client)
class CAdv_Passersby :
    public CAdventure
{
private:
    CAdv_Passersby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CAdv_Passersby(const CAdv_Passersby& rhs);
    virtual ~CAdv_Passersby() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

public:
    virtual HRESULT Add_Components() override;
    
public:
    static CAdv_Passersby* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
