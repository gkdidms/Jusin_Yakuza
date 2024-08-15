#pragma once
#include "RoadStanding.h"

/* ÀþÀº »ç¶÷ */
BEGIN(Client)
class CRoadYNG :
    public CRoadStanding
{
private:
    CRoadYNG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRoadYNG(const CRoadYNG& rhs);
    virtual ~CRoadYNG() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

public:
    virtual HRESULT Add_Components() override;

public:
    static CRoadYNG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
