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
public:
    enum CAM_STATE
    {
        CAM_IDLE,
        CAM_COLLISION_BLOCK,
        CAM_LERP,
        CAM_GOTO_SAFE_POS,
        CAM_STATE_END
    };

private:
    const _float MAX_DISTANCE = 2.9f;
    const _float MIN_DISTANCE = 2.0f;

public:
    typedef struct tPlayerCameraDesc : public CAMERA_DESC
    {
        _float      fSensor;
        int         iCurLevel;
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
    void    Compute_View_During_Collision(const _float& fTimeDelta);

    //처음 시작할때 설정
    void    Set_StartPos();
    void    Adjust_Camera_Angle();

    // 이전 월드매트릭스로 선형보간하는 함수
    void    Return_PrevWorld(const _float& fTimeDelta);
    void    Reset_RetureVariables();            // 관련 변수 초기화 함수




public:
    void Store_PrevMatrix() {
        m_PrevMatrix = m_WorldMatrix;
    };
    void Store_StartMatrix(_fmatrix mat) {
        XMStoreFloat4x4(&m_StartMatrix, mat);
    };

    void Set_StartFov(_float f)
    {
        m_fStartFov = f;
    }

    void On_Return() {
        m_isReturn = true;
    }

private:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CCollision_Manager* m_pCollisionManager = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };
    CCollider* m_pColliderCom = { nullptr };

    _float m_fSensor = { 0.f };
    const _float4x4* m_pPlayerMatrix = { nullptr };

    _float m_fCamDistance = { 4.f };            // 해당 값을 변경하면 카메라의 거리를 조절할 수 있음.

    _float m_fCamAngleX = 45.f;
    _float m_fCamAngleY = -90.f;

    _float m_fPreMouseMoveY; // 이전 각도 값 저장
    _float m_fPreMouseMoveX;

    XMVECTOR    m_vPlayerPos = { 0,0,0,1 }; // 카메라 충돌 되기 전 마지막 플레이어 위치 저장
    XMVECTOR    m_vLatestCollisionPos;      // 가장 최근 충돌 위치
    XMVECTOR    m_vSafePos;      // 가장 최근 충돌 위치
    
    bool        m_bPreCamCollision = { false };
    bool        m_bCamCollision = { false };
    bool        m_bLerp = { false };
    bool        m_bBlock = { false };
    int         m_iCollisionNum = { 0 }; // first collision check 

    _float       m_fTimer = { 0 };

    _int         m_iCurLevel = { -1 };

    // 카메라 돌아가는거 관련
    _bool       m_isReturn = { false };
    _float4x4    m_StartMatrix;         // 컷신 캠으로 변경 시 지금상태의 매트릭스를 저장
    _float4x4    m_PrevMatrix;         // 컷신 캠으로 변경 시 지금상태의 매트릭스를 저장
    _float       m_fLerpRatio = { 0.f };    // 보간 비율
    float       m_fElapsedTime = 0.0f; // 경과 시간
    float       m_fTotalLerpTime = 0.5f; // 보간에 걸리는 총 시간 (초 단위)
    float       m_fStartFov = 0.0f; // 보간에 걸리는 총 시간 (초 단위)

    CAM_STATE        m_eCamState = { CAM_STATE_END };

private:
    HRESULT Add_Components();


public:
    static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
