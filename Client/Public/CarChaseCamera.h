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
        const _float4x4* pPlayerBoneMatrix;
    } CARCHASE_CAMERA_DESC;

private:
    CCarChaseCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCarChaseCamera(const CCarChaseCamera& rhs);
    virtual ~CCarChaseCamera() = default;

public:
    void Set_StageDir(_uint iDir) {
        m_iStageDir = iDir;
    }

    // 숄더뷰 반전
    void ShoulderView_On() {
        m_isShoulderView = true;
    }
    void ShoulderView_Off() {
        m_isShoulderView = false;
    }

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
    const _float4x4* m_pPlayerBoneMatrix = { nullptr };

    _float4x4 m_pPlayerWorldMatrix = {};

    _uint m_iStageDir = { DIR_END };
    _vector m_vDir = {1.f, 0.f, 0.f, 0.f };

    _bool m_isShoulderView = { false };

    /* 기본적인 카메라 정보 */
    _float m_fSensor = { 0.f };
    _float m_fCamDistance = { 4.f };
    _float m_fCamAngleY = { 0.f };
    _float m_fCamAngleX = { 0.f };

    /* 숄더뷰로 진행하는 카메라 정보 */
    _float m_fCamDistance_X = { -1.f };
    _float m_fCamDistance_Y = { 1.f };
    _float m_fCamDistance_Z = { 1.f };

    _bool m_isTargetPlayer = { false };
    _float m_fHeight = { 2.f };

private:
    void Targeting(const _float& fTimeDelta);

public:
    static CCarChaseCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END