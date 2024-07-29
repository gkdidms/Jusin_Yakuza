#pragma once
#include "CarChase_Monster.h"

BEGIN(Client)
class CCarChase_Sedan :
    public CCarChase_Monster
{
public:
    enum CAR_DIR { CAR_F, CAR_L, CAR_R, CAR_B, CAR_END };

private:
    CCarChase_Sedan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCarChase_Sedan(const CCarChase_Sedan& rhs);
    virtual ~CCarChase_Sedan() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    _uint m_iDir = { CAR_END };

private:
    virtual void Change_Animation();
    virtual HRESULT Add_Components() override;

public:
    static CCarChase_Sedan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
