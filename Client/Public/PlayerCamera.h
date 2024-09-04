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
        CAM_ADV,
        CAM_BATTLE,
        CAM_STATE_END
    };


private:
    //����
    //const _float MAX_DISTANCE = 3.1f;
    //const _float MIN_DISTANCE = 2.5f;

    _float MIN_LERP_DISTANCE = 2.5f;

    const _float MAX_DISTANCE = 4.1f;
    const _float MIN_DISTANCE = 3.5f;

    _float MAX_DISTANCE_ADV = 2.8;
    _float MIN_DISTANCE_ADV = 1.5;

    _float MAX_DISTANCE_BATTLE = 4.1f;
    _float MIN_DISTANCE_BATTLE = 3.5f;

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
    void    Camera_FightMode_Change(const _float& fTimeDelta);

    //ó�� �����Ҷ� ����
    void    Set_StartPos();
    void    Adjust_Camera_Angle();

    // ���� �����Ʈ������ ���������ϴ� �Լ�
    void    Return_PrevWorld(const _float& fTimeDelta);
    void    Reset_ReturnVariables();            // ���� ���� �ʱ�ȭ �Լ�

    void    Play_FovLerp(const _float& fTimeDelta);



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

    void Set_RotationBlock(_bool bBlock) {
        // ȸ������ �Լ� - true�� ������
        m_bBlockRotation = bBlock;
    }

private:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CCollision_Manager* m_pCollisionManager = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };
    CCollider* m_pColliderCom = { nullptr };

    _float m_fSensor = { 0.f };
    const _float4x4* m_pPlayerMatrix = { nullptr };

    _float m_fCamDistance = { 4.f };            // �ش� ���� �����ϸ� ī�޶��� �Ÿ��� ������ �� ����.

    _float m_fCamAngleX = 45.f;
    _float m_fCamAngleY = -90.f;

    _float m_fPreMouseMoveY; // ���� ���� �� ����
    _float m_fPreMouseMoveX;

    _vector     m_vPlayerPos = { 0,0,0,1 }; // ī�޶� �浹 �Ǳ� �� ������ �÷��̾� ��ġ ����
    _vector     m_vLatestCollisionPos;      // ���� �ֱ� �浹 ��ġ
    
    
    bool        m_bPreCamCollision = { false };
    bool        m_bCamCollision = { false };
    bool        m_bLerp = { false };
    bool        m_bBlock = { false };
    bool        m_bFightLerp = { false }; // ��� �������� ���� lerp ó��
    bool        m_bBlockRotation = { false }; // ī�޶� ���콺 �̵� ����

    int         m_iCollisionNum = { 0 }; // first collision check 

    _float       m_fTimer = { 0 };

    _int         m_iCurLevel = { -1 };

    // ī�޶� ���ư��°� ����
    _bool       m_isReturn = { false };
    _float4x4    m_StartMatrix;         // �ƽ� ķ���� ���� �� ���ݻ����� ��Ʈ������ ����
    _float4x4    m_PrevMatrix;         // �ƽ� ķ���� ���� �� ���ݻ����� ��Ʈ������ ����

    _float       m_fStartFov = 0.0f; 

    CAM_STATE        m_eCurCamState = { CAM_STATE_END };
    CAM_STATE        m_ePreCamState = { CAM_STATE_END };

    _float          m_fMaxDistance = { MAX_DISTANCE }; // ��� �ٲ���
    _float          m_fMinDistance = { MIN_DISTANCE }; // ��� �ٲ���


    _float          m_fLerpDelta = { 0 };

private:
    HRESULT Add_Components();


public:
    static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
