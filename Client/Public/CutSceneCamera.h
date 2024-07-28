#pragma once
#include "Camera.h"

#include "Client_Defines.h"

BEGIN(Client)
class CCutSceneCamera :
    public CCamera
{
private:
    CCutSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCutSceneCamera(const CCutSceneCamera& rhs);
    virtual ~CCutSceneCamera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    void Compute_ViewMatrix();

private:
    class CSystemManager* m_pSystemManager = { nullptr };

private:
    _float fConvertX = {90.f};        //90
    _float fConvertY = {-180.f};      //-180
    _float fConvertZ = {90.f};        //90

public:
    static CCutSceneCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();

};
END
