#pragma once
#include "Camera.h"

#include "Client_Defines.h"

BEGIN(Client)
class CCarChaseCamera :
    public CCamera
{
public:
    typedef struct tCarChaseCameraDesc : public CAMERA_DESC
    {
        _float fSensor;
        const _float4x4* pPlayerMatrix;
    } CARCHASE_CAMERA_DESC;

private:
    CCarChaseCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCarChaseCamera(const CCarChaseCamera& rhs);
    virtual ~CCarChaseCamera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    class CSystemManager* m_pSystemManager = { nullptr };
    const _float4x4* m_pPlayerMatrix = { nullptr };

    _float m_fSensor = { 0.f };

private:
    void Targeting();

public:
    static CCarChaseCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END