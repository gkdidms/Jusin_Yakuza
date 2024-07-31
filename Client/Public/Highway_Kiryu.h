#pragma once
#include "LandObject.h"

BEGIN(Client)
class CHighway_Kiryu :
    public CLandObject
{
private:
    CHighway_Kiryu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CHighway_Kiryu(const CHighway_Kiryu& rhs);
    virtual ~CHighway_Kiryu() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    // CLandObject을(를) 통해 상속됨
    string Get_CurrentAnimationName() override;

protected:
    virtual void Change_Animation();

protected:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    static CHighway_Kiryu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();


};
END
