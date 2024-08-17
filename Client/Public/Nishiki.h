#pragma once
#include "NPC.h"

BEGIN(Client)
class CNishiki :
    public CNPC
{
private:
    CNishiki(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNishiki(const CNishiki& rhs);
    virtual ~CNishiki() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

protected:
    virtual void Change_Animation();
    virtual HRESULT Add_Components() override;

public:
    static CNishiki* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
