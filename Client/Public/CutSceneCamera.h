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

public:
    void Store_PrevMatrix() {
        m_PrevMatrix = m_WorldMatrix;
    };
    void Store_StartMatrix(_fmatrix mat) {
        XMStoreFloat4x4(&m_StartMatrix, mat);
    };

    void On_Return() {
        m_isReturn = true;
    }

    void Set_StartFov(_float f)
    {
        m_fStartFov = f;
    }


private:
    // ���� �����Ʈ������ ���������ϴ� �Լ�
    void    Return_PrevWorld(const _float& fTimeDelta);
    
    void    Play_FovLerp(const _float& fTimeDelta);

public:
    void    Reset_ReturnVariables();            // ���� ���� �ʱ�ȭ �Լ�

private:
    class CSystemManager* m_pSystemManager = { nullptr };

private:
    _float fConvertX = {90.f};        //90
    _float fConvertY = {-180.f};      //-180
    _float fConvertZ = {90.f};        //90

    // ī�޶� ���ư��°� ����
    _bool       m_isReturn = { false };
    _float4x4    m_StartMatrix;         // �ƽ� ķ���� ���� �� ���ݻ����� ��Ʈ������ ����
    _float4x4    m_PrevMatrix;         // �ƽ� ķ���� ���� �� ���ݻ����� ��Ʈ������ ����
    _float       m_fLerpRatio = { 0.f };    // ���� ����
    float       m_fElapsedTime = 0.0f; // ��� �ð�
    float       m_fTotalLerpTime = 0.5f; // ������ �ɸ��� �� �ð� (�� ����)
    float       m_fStartFov = 0.0f; // �����ϴ� Fov��

public:
    static CCutSceneCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();

};
END
