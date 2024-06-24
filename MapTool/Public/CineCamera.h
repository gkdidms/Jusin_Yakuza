#pragma once
#include "Camera.h"

#include "Client_Defines.h"

BEGIN(Client)
class CCineCamera :
    public CCamera
{
public:
    typedef struct tPlayerCameraDesc : public CAMERA_DESC
    {
        _float fSensor;
    } PLAYER_CAMERA_DESC;

private:
    CCineCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCineCamera(const CCineCamera& rhs);
    virtual ~CCineCamera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    void			Mouse_Fix();

private:
    _float      m_fSensor = { 0.f };
    _bool		m_bCameraFix = false;
    vector<CAMERAOBJ_DESC>							m_vCamerasObjDesc;

public:
    static CCineCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();

};
END
