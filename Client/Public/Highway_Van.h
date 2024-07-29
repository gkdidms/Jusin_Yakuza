#pragma once
#include "Highway.h"

BEGIN(Client)
class CHighway_Van :
    public CHighway
{
private:
    CHighway_Van(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CHighway_Van(const CHighway_Van& rhs);
    virtual ~CHighway_Van() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

protected:
    virtual void Change_Animation();

protected:
    virtual HRESULT Add_Components() override;

public:
    static CHighway_Van* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
