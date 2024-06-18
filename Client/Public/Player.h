#pragma once
#include "LandObject.h"

BEGIN(Client)
class CPlayer :
    public CLandObject
{
private:
    CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    virtual HRESULT Add_Componenets() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};
END
