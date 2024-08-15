#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CShader;
class CAnim;
class CNavigation;
class CAStart;
class CCell;
END

BEGIN(Client)
class CAdventure abstract :
    public CLandObject
{
public:
    typedef struct tMapAdventureObjDesc : public CGameObject::GAMEOBJECT_DESC
    {
        XMMATRIX		vStartPos;
        wstring			wstrModelName;
        int				iShaderPass;
        int             iNaviNum;
        int             iNaviRouteNum;
    }ADVENTURE_IODESC;

public:
    enum ADVENTURE_STATE
    {
        ADVENTURE_IDLE,
        ADVENTURE_WALK,
        ADVENTURE_WALK_S,
        ADVENTURE_WALK_EN,
        ADVENTURE_HIT_L,
        ADVENTURE_HIT_R,
        ADVENTURE_TURN,
        ADVENTURE_TURN90_R,
        ADVENTURE_TURN90_L,
    };

public:
    void Start_Root(_int iGoalIndex);

protected:
    CAdventure(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CAdventure(const CAdventure& rhs);
    virtual ~CAdventure() = default;

public:
    _bool isColl() { return m_isColl; }

public:
    void Set_Move();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    // �浹�Լ�
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) override;
    virtual void Animation_Event() override;
    virtual string Get_CurrentAnimationName() override;

    _bool Checked_Animation_Ratio(_float fRatio);

protected:
    CAnim* m_pAnimCom = { nullptr }; // �ִϸ��̼Ǹ� ���� �����ϰ��ִ� �ִϸ��̼� ������Ʈ 
    CAStart* m_pAStartCom = { nullptr };
    
protected:
    _bool m_isAnimLoop = { false };
    _uint m_iState = { 0 };
    _float m_fChangeInterval = { 4.f };
    _float m_fOffset = { 0.8f };

    _float          m_fPrevSpeed;
    _float4         m_vPrevMove;
    _float4         m_vPrevRotation;
    _float4x4       m_ModelWorldMatrix;

protected:
    string  m_strAnimName = "";
    _uint   m_iAnim = { 0 };

    _bool   m_isColl = { false };

    int     m_iNaviRouteNum = { 0 };

    _float m_fSpeed = { 2.f };

protected:
    virtual void Change_Animation();
    void Synchronize_Root(const _float& fTimeDelta);
    void Check_Separation();

protected:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    virtual void Free();
};
END
