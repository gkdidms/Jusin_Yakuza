#pragma once
#include "LandObject.h"

BEGIN(Client)
class CHighway_Kiryu :
    public CLandObject
{
public:
    struct CARCHASE_KIRYU_DESC
    {
        _float4x4* pTaxiWorldMatrix;
    };

    enum BATTLE_DIRECTION
    {
        LEFT, RIGHT,
    };

    enum BEHAVIOR_TYPE
    {
        AIMING, HIDE, HIT, SHOT, SWAP,
    };

    /*
    * 0~34 : ���� ���� �ִϸ��̼�
    * 35~70: ���� ���� �ִϸ��̼�
    * 71~73: ���� �ɱ� ���� �ִϸ��̼� (�ɱ� ������ _en ������ �Ͼ�� �ִϸ��̼�)
    * 74~76: ���� �ɱ� ���� �ִϸ��̼�
    * 77~78: �ڸ� �̵� (��, ��)
    */

private:
    const _uint MAX_AMMO = 15;

private:
    CHighway_Kiryu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CHighway_Kiryu(const CHighway_Kiryu& rhs);
    virtual ~CHighway_Kiryu() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    // CLandObject��(��) ���� ��ӵ�
    string Get_CurrentAnimationName() override;

private:
    virtual void Change_Animation();
    
    void Key_Input();

    /* �ִϸ��̼� */
private:
    void Play_CurrentAnimation(_float fTimeDelta);

    void Play_Animing(_float fTimeDelta);
    void Play_HideReload(_float fTimeDelta);
    void Play_Hit(_float fTimeDelta);
    void Play_Shot(_float fTimeDelta);
    void Play_Swap(_float fTimeDelta);


private:
    void Change_Behavior(BEHAVIOR_TYPE eType) {
        m_eCurrentBehavior = eType;
        m_isStarted = false;
    }
    void HideReload();

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

private:
    const _float4x4* pTaxiMatrix = { nullptr };

    _float m_fHP = { 300.f };
    _float m_fHitEye = { 100.f };

    _uint m_iCurrentAmmo = { MAX_AMMO };

    BATTLE_DIRECTION m_eBattleDirection = { LEFT };
    BEHAVIOR_TYPE   m_eCurrentBehavior = { AIMING };
    _bool           m_isStarted = { false };


    _bool m_bTest = { false };
    _float m_fTest = 0.f;

public:
    static CHighway_Kiryu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();


};
END
