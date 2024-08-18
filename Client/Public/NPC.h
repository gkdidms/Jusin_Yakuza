#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CNeoShader;
END

BEGIN(Client)
class CNPC abstract:
    public CLandObject
{
protected:
    CNPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNPC(const CNPC& rhs);
    virtual ~CNPC() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

protected:
    virtual void Change_Animation();
    virtual string Get_CurrentAnimationName() override;

protected:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    virtual void Free();
};
END
