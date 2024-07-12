#pragma once
#include "Camera.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CPlayerCamera :
    public CCamera
{
private:
    const _float MAX_DISTANCE = 4.f;
    const _float MIN_DISTANCE = 1.f;

public:
    typedef struct tPlayerCameraDesc : public CAMERA_DESC
    {
        _float fSensor;
        const _float4x4* pPlayerMatrix;
    } PLAYER_CAMERA_DESC;

private:
    CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPlayerCamera(const CPlayerCamera& rhs);
    virtual ~CPlayerCamera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    // 왜인진 모르겠으나 카메라 값 연산을 Tick, Late_Tick 에서 
    // 각각 한번씩 (총 2번) 해주어야 카메라가 안밀림
    // 따라서 그 연산을 수행하는 함수이다.
    // 파이프라인에 뷰행렬 적용은 Late_Tick에서 한번만 해주어야함
    void Compute_View(const _float& fTimeDelta);

private:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CCollision_Manager* m_pCollisionManager = { nullptr };
    CCollider* m_pColliderCom = { nullptr };
    _float m_fSensor = { 0.f };
    const _float4x4* m_pPlayerMatrix = { nullptr };

    _float m_fCamDistance = { 4.f };            //해당 값을 변경하면 카메라의 거리를 조절할 수 있음.

    _float fCamAngleX = 45.f;
    _float fCamAngleY = -90.f;

private:
    HRESULT Add_Components();

public:
    static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
