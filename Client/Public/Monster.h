#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CShader;
class CNavigation;
class CAnim;
END

BEGIN(Client)
class CMonster abstract:
    public CLandObject
{
public:
    typedef struct tMapMonsterObjDesc : public CGameObject::GAMEOBJECT_DESC
    {
        XMMATRIX		vStartPos;
        wstring			wstrModelName;
        int				iShaderPass;
        int             iNaviNum;
    }MONSTER_IODESC;

public:
    enum MONSTER_STATE
    {
        MONSTER_IDLE,
        MONSTER_RUN,
        MONSTER_SHIFT_F,
        MONSTER_SHIFT_L,
        MONSTER_SHIFT_R,
        MONSTER_SHIFT_B,
        MONSTER_SHIFT_FR,
        MONSTER_SHIFT_FL,
        MONSTER_SHIFT_BR,
        MONSTER_SHIFT_BL,
        MONSTER_STEP_B,
        MONSTER_STEP_L,
        MONSTER_STEP_R,
        MONSTER_STEP_F,
        MONSTER_SWAY_F,
        MONSTER_SWAY_L,
        MONSTER_SWAY_R,
        MONSTER_SWAY_B,
        MONSTER_CMD_1,
        MONSTER_CMD_2,
        MONSTER_CMD_3,
        MONSTER_CMD_4,
        MONSTER_CMD_5,
        MONSTER_CMD_A_1,
        MONSTER_CMD_A_2,
        MONSTER_CMD_A_3,
        MONSTER_CMD_B_1,
        MONSTER_CMD_B_2,
        MONSTER_CMD_B_3,
        MONSTER_CMD_HEADBUTT_1,
        MONSTER_CMD_HEADBUTT_2,
        MONSTER_CMD_RENDA_1,
        MONSTER_CMD_RENDA_2,
        MONSTER_CMD_RENDA_3,
        MONSTER_PUNCH,
        MONSTER_JAB,
        MONSTER_HEAVY_ATTACK,
        MONSTER_ATK_DOWN,
        MONSTER_GUARD_RUN,
        MONSTER_RARIATTO,
        MONSTER_HIJI_2REN,
        MONSTER_ANGRY_START,
        MONSTER_ANGRY_CHOP,
        MONSTER_ANGRY_KICK,
        MONSTER_GURAD_START,
        MONSTER_GURAD_LOOP,
        MONSTER_GURAD_END,
        MONSTER_GURAD_FLOAT,

        MONSTER_DWN_DNF_BOUND,
        MONSTER_DWN_DNB_BOUND,
        MONSTER_DWN_DNF_BOUND_G,
        MONSTER_DWN_DNB_BOUND_G,
        MONSTER_DAM_HEAD_LV01_R,
        MONSTER_DAM_HEAD_LV01_L,
        MONSTER_DAM_HEAD_LV01_F,
        MONSTER_DAM_HEAD_LV01_B,
        MONSTER_DAM_HEAD_LV02_R,
        MONSTER_DAM_HEAD_LV02_L,
        MONSTER_DAM_HEAD_LV02_F,
        MONSTER_DAM_HEAD_LV02_B,
        MONSTER_DAM_BODY_LV01_F,
        MONSTER_DAM_BODY_LV01_B,
        MONSTER_DAM_BODY_LV01_D,
        MONSTER_DAM_BODY_LV02_F,
        MONSTER_DAM_BODY_LV02_B,
        MONSTER_DAM_BODY_LV02_D,
        MONSTER_DWN_DIRECT_F_BOUND_G,
        MONSTER_DWN_DIRECT_B_BOUND_G,
        MONSTER_DWN_DIRECT_B,
        MONSTER_DWN_DIRECT_F,
        MONSTER_DWN_BODY_F,
        MONSTER_DWN_BODY_F_SP,
        MONSTER_DWN_EXPLODE_F,
        MONSTER_DWN_BODY_B,
        MONSTER_DWN_BODY_B_SP,
        MONSTER_DWN_EXPLODE_B,
        MONSTER_STANDUP_DNF_FAST,
        MONSTER_STANDUP_DNB_FAST,
        MONSTER_DED_L, // 죽음
        MONSTER_DED_R,
        MONSTER_DED_F_1,
        MONSTER_DED_F_2,
        MONSTER_DED_F_3,
        MONSTER_DED_B_1,
        MONSTER_DED_B_2,
        MONSTER_STATE_END
    };

protected:
    CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMonster(const CMonster& rhs);
    virtual ~CMonster() = default;

public:
    _bool isColl() { return m_isColl; }
    _bool isDown() { return m_isDown; }

public:
    void Set_Down(_bool isDown) { m_isDown = isDown; }
    
public:
    _float Get_HitDamage() { return m_fHitDamage; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual HRESULT Render_LightDepth() override;

    // 충돌함수
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) override;
    virtual void Animation_Event() override;
    virtual string Get_CurrentAnimationName() override;

protected:
    CShader* m_pShaderCom = { nullptr };
    CAnim* m_pAnimCom = { nullptr }; // 애니메이션만 따로 저장하고있는 애니메이션 컴포넌트
    CNavigation* m_pNavigationCom = { nullptr };
    class CAI_Monster* m_pTree = { nullptr };

protected:
    _bool m_isAnimLoop = { false };
    _uint m_iState = { 0 };
    _float m_fChangeInterval = { 4.f };

    _float m_fPrevSpeed;
    _float4         m_vPrevMove;
    _float4         m_vPrevRotation;
    _float4x4       m_ModelWorldMatrix;

protected:
    string m_strAnimName = "";
    _uint m_iAnim = { 0 };

protected:
    _float m_fHitDamage = { 0.f };
    _bool m_isColl = { false }; // 충돌되었는지 아닌지 체크해야함.
    _bool m_isDown = { false }; // 다운되었는가?

protected:
    virtual void Change_Animation();
    void Synchronize_Root(const _float& fTimeDelta);

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    virtual void Free() override;
};
END
