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
    const _float MIN_DISTANCE = 2.5f;

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
    // ������ �𸣰����� ī�޶� �� ������ Tick, Late_Tick ���� 
    // ���� �ѹ��� (�� 2��) ���־�� ī�޶� �ȹи�
    // ���� �� ������ �����ϴ� �Լ��̴�.
    // ���������ο� ����� ������ Late_Tick���� �ѹ��� ���־����
    void    Compute_View(const _float& fTimeDelta);


    //ó�� �����Ҷ� ����
    void    Set_StartPos();

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

    XMVECTOR    m_vCamCollisionPos = { 0,0,0,1 };
    
    bool        m_bCamCollision = { false };
    bool        m_bBlock = { false };
    bool        m_bFirstCollision = { false }; // first collision check 

    float       m_fTimer = { 0 };

    int         m_iCurLevel = { -1 };

private:
    HRESULT Add_Components();

public:
    static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
