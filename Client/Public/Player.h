#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Client)
class CPlayer :
    public CLandObject
{
public:
    const _float PLAYER_HITGAUGE_LEVEL_INTERVAL = 50.f;

public:
    //KRS: 불한당, KRH: 러쉬, KRC: 파괴자
    enum BATTLE_STYLE
    {
        ADVENTURE, KRS, KRH, KRC, BATTLE_STYLE_END
    };

    /* 06.28 혜원 추가
    *  스타일마다 겹치는 행동이 있어서 enum class 사용 
    *  행동은 임시로 일부만 잡아둔 상태로 지속적으로 추가될 예정임
    */
    enum class ADVENTURE_BEHAVIOR_STATE
    {
        IDLE, WALK, RUN, ADVENTURE_BEHAVIOR_END
    };    
    enum class KRS_BEHAVIOR_STATE
    {
        BTL_START, IDLE, WALK, RUN, ATTACK, HIT, 
        SWAY, DOWN, 
        SKILL_FLY_KICK, SKILL_KICK_COMBO,
        KRS_BEHAVIOR_END
    };

    enum class KRH_BEHAVIOR_STATE
    {
        BTL_START, IDLE, WALK, RUN, ATTACK, HIT,
        SWAY, DOWN, 
        KRH_BEHAVIOR_END
    };

    enum class KRC_BEHAVIOR_STATE
    {
        BTL_START, IDLE, WALK, RUN, ATTACK, HIT,
        SWAY, DOWN, GAURD,
        KRC_BEHAVIOR_END
    };


    enum MOVE_DIRECTION
    {
        F, B, L, R, MOVE_DIRECTION_END
    };


private:
    const _float ANIM_INTERVAL = 4.f;
    
private:
    CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() = default;
    
#ifdef _DEBUG
public:

public:
    void Set_AnimStart(_bool isAnim) { m_isAnimStart = isAnim; }
#endif // DEBUG

    /* Getter */
public:
    _uint Get_BattleStyle() { return m_eCurrentStyle; }
    _bool isAnimStart() { return m_isAnimStart; }
    _bool isAttack() { return m_iCurrentBehavior == static_cast<_uint>(KRS_BEHAVIOR_STATE::ATTACK); }
    _uint Get_CurrentHitLevel() { return m_iCurrentHitLevel; }

    const _bool* Get_MoveDirection() {
        return m_MoveDirection;
    }

    CLandObject* Get_TargetObject() {
        return m_pTargetObject;
    }

    /* Setter */
public:
    void  Set_StartPos(XMMATRIX    vStartPos) { m_pTransformCom->Set_WorldMatrix(vStartPos); }

    /* Virtual Funtion */
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

    virtual string Get_CurrentAnimationName() override;

    /* Initial Function */
private:
    void Ready_AnimationTree();
    void Synchronize_Root(const _float& fTimeDelta);

    //키 입력관련함수들
private:
    void KeyInput(const _float& fTimeDelta);       //현재 플레이어의 상태에 따른 키입력처리 하는 함수

    // 어드벤처 모드에서의 키입력
    void Adventure_KeyInput(const _float& fTimeDelta);      
    // 불한당 모드 키입력
    void KRS_KeyInput(const _float& fTimeDelta);  
    // 러쉬 모드 키입력
    void KRH_KeyInput(const _float& fTimeDelta);      
    // 파괴자 모드 키입력
    void KRC_KeyInput(const _float& fTimeDelta);

public:
    void Change_Animation(_uint iAnimIndex, _float fInterval = 4.f);
    void Style_Change(BATTLE_STYLE eStyle);
    void Reset_MoveDirection();

private:
    void Compute_MoveDirection_FB();
    void Compute_MoveDirection_RL();

    void Effect_Control_Aura();
    void Setting_Target_Enemy();

public:
    void AccHitGauge();
    
    /* 출력, 행동 관련 포인터 변수들 */
private:
    CShader*                m_pShaderCom = { nullptr };

    // 이 때, 사용하는 키 값은 행동에 대한 키값을 가진다. (스타일은 배열 인덱스)
    map<_uint, class CBehaviorAnimation*> m_AnimationTree[BATTLE_STYLE_END];
    //ui
    class CUIManager* m_pUIManager = { nullptr };
    
#ifdef _DEBUG
    class CDebugManager* m_pDebugManager = { nullptr };
#endif // _DEBUG

    /* 플레이어 기능 관련 포인터 변수들 */
private:
    CLandObject* m_pTargetObject = { nullptr };

    /* 행동, 이동 관련 변수들 */
private:
    BATTLE_STYLE    m_eCurrentStyle = { ADVENTURE };
    // 스타일마다 겹치는 행동이 있을 수 있어서 int값으로 저장하고 형변환하여 저장한다.
    _uint           m_iCurrentBehavior = static_cast<_uint>(ADVENTURE_BEHAVIOR_STATE::IDLE);

    // 스웨이용 키입력 디렉션 (카메라 기준으로 처리하기 위함)
    _bool           m_InputDirection[MOVE_DIRECTION_END];
    _bool           m_MoveDirection[MOVE_DIRECTION_END];

    _float          m_fPrevSpeed = { 0.f };
    _float4         m_vPrevMove;
    _float4         m_vPrevRotation;
    _float4x4       m_ModelWorldMatrix;

    _bool          m_isAuraOn = { false };

    /* 플레이어 스테이터스 관련 변수들 */
private:
    _uint           m_iCurrentHitLevel = { 1 };
    _float          m_fHitGauge = { 100.f };



#ifdef _DEBUG
    _bool m_isAnimStart = { true };
#endif // _DEBUG

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};
END
