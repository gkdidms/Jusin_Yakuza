#pragma once
#include "Camera.h"

#include "Client_Defines.h"

BEGIN(Client)
class CDebugCamera :
    public CCamera
{
public:
    typedef struct tPlayerCameraDesc : public CAMERA_DESC
    {
        _float fSensor;
    } PLAYER_CAMERA_DESC;

private:
    CDebugCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDebugCamera(const CDebugCamera& rhs);
    virtual ~CDebugCamera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    _float m_fSensor = { 0.f };

    _bool m_isMove = { true };

public:
    static CDebugCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();

};
END
