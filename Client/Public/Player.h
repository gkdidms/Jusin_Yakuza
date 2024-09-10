#pragma once
#include "LandObject.h"
#include "Navigation.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CNavigation;
class CAnim;
END

BEGIN(Client)
class CPlayer :
    public CLandObject
{
public:
    const _float PLAYER_HITGAUGE_LEVEL_INTERVAL = 50.f;

#pragma region �̳Ѱ���
public:
    //KRS: ���Ѵ�, KRH: ����, KRC: �ı���
    enum BATTLE_STYLE
    {
        ADVENTURE, KRS, KRH, KRC, BATTLE_STYLE_END
    };

    /* 06.28 ���� �߰�
    *  ��Ÿ�ϸ��� ��ġ�� �ൿ�� �־ enum class ��� 
    *  �ൿ�� �ӽ÷� �Ϻθ� ��Ƶ� ���·� ���������� �߰��� ������
    */
    enum class ADVENTURE_BEHAVIOR_STATE
    {
        IDLE, WALK, RUN, ADVENTURE_BEHAVIOR_END
    };    

    // ����: SKILL_FLY_KICK
    enum class KRS_BEHAVIOR_STATE
    {
        BTL_START, IDLE, WALK, RUN, ATTACK, HIT, 
        SWAY, DOWN, GRAB, PICK_UP,
        SKILL_FLY_KICK, SKILL_KICK_COMBO,
        KRS_BEHAVIOR_END
    };

    // ����: -
    enum class KRH_BEHAVIOR_STATE
    {
        BTL_START, IDLE, WALK, RUN, ATTACK, HIT,
        SWAY, DOWN, 
        KRH_BEHAVIOR_END
    };

    // ����: GUARD
    enum class KRC_BEHAVIOR_STATE
    {
        BTL_START, IDLE, WALK, RUN, ATTACK, HIT,
        SWAY, DOWN, GRAB, PICK_UP,
        GUARD,
        KRC_BEHAVIOR_END
    };

    enum MOVE_DIRECTION
    {
        F, B, L, R, MOVE_DIRECTION_END
    };

    enum CUTSCENE_ANIMATION_TYPE
    {
        /* ���Ѵ� ��Ʈ�׼� */
        FINISHBLOW,                 //h23320 �ǴϽ� ����� ��
        GOUGEKI_C,                  //h20021 ��� �� ��Ʈ�׼� (�ָ����� �󱼶����� ��ġ��)
        HAIHEKI_KICK,               //h23070 �� ������ ����ġ�� ��ų

        /* ���� */
        KOBUSHIKUDAKI,              //h10111 �Ȳ�ġ�� ���ݸ��� ��ų
        HAIHEKI_PUNCH,              //h23060 �� ������ ������ ��ų
        OI_TRAMPLE_AO,              //h1500 �ٿ�� ��� �� ��� (�����ִ� ����), ����
        OI_KICKOVER_UTU_C,          //h1511 �ٿ�� ��� �� ���� (������ִ� ����), ����

        /* �ı��� */
        KIRYU_GSWING,               //h1010 �ٸ���� ������ ���ø� ����
        DORYU_MIN,                  //h11285 ������ ������ ���ø� ����
        LAPEL_OIUCHI_NECK,          //h1530 �� �ٴڿ� ������(����)
        NAGE_OIUCHI_NECK,           //h1540 �� �ٴڿ� ������(����)
        POLE_KNOCK_LAPEL,           //h2040 ��ó�� ����� �ִٸ� ��տ� �ڰ��ϰ� ����
        DORAMUKAN_88,               //h3261 ū ���� (����)�� ��� ���� �а� ����ħ
        MONZETSU,                   //h11250 ���ٰ� �������� �˲����� (�ڿ��� ��������� ���)

        /* �⺻ ��Ʈ�׼� */
        WALL_KNOCK_NECK_C,          //h2011 ���� �Ӹ��ڰ��ϰ� ���� (�Ƹ� ��� ���� ��Ʈ�׼����� �� ��ó�� �����ִٸ�)
        KABE_AIRON,                 //h23000 ������ ��ġ�� ����
        OI_KICK,                    //h23010 �Ӹ�ä ��� �� �߷��� (���帰 ����)
        OI_UPPER,                   //h23020 �Ӹ�ä��� �� �ָ����� ħ (�����ִ� ����)

        /* ��״� �ƽ� */
        YONEDA_H,                   //A60300 ��״� ����
        YONEDA_DOWN_ATTACK,         //A60330 ��״� ����
        YONEDA_DOSU,                //a60350 ��״� ȭ���

        KUZE_RUSH,                  //h23250 ��״� ȭ���

        CUTSCENE_ANIMATION_END
    };

    // ǥ�� �ִϸ��̼�
    // [0] f201_special_01[f_kiryu]             // �Ѱ������� ���� �����ϴ� �ƽſ��� ����
    // [1] f202_special_02[f_kiryu]             // �λ�(ȭ��)
    // [2] f203_special_03[f_kiryu]             // �λ�(�������� �̷�ǥ��) Ȳ�����ϴµ�
    // [3] f204_special_04[f_kiryu]             // �λ󾲰��ִµ� �����ϰ� ����
    // [4] f205_special_05[f_kiryu]             // �����𸣰ڴ� ������ ǥ��
    // [5] f206_special_06[f_kiryu]             // �̽�½�� ����
    // [6] f207_special_07[f_kiryu]             // ������ �� ���ϵ�?
    // [7] f208_special_08[f_kiryu]             // ���� �Ⱦ����Ժ��°Ű��ƺ���
    // [8] f209_special_09[f_kiryu]             // ũ�� ȭ�� ��
    // [9] f210_special_10[f_kiryu]             // �� �θ��ߴ�
    // [10] f211_special_11[f_kiryu]            // �λ󾲰� �Ǵ��߰�
    // [11] f212_special_12[f_kiryu]            // �� ũ�Զ� (�����)

    struct PLAYER_INFO {
        _uint           iCurrentHitLevel = { 0 };
        _float          fHitGauge = { 0.f };
        _uint           iMoney = { 0 };
        LAND_OBJ_INFO   Info;
    };
    static PLAYER_INFO PlayerInfo;

#pragma endregion

private:
    const _float ANIM_INTERVAL = 4.f;
    
private:
    CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() = default;
    
#ifdef _DEBUG


public:
    void Set_AnimStart(_bool isAnim) { m_isAnimStart = isAnim; }

    _bool isAnimStart() { return m_isAnimStart; }


#endif // DEBUG
public:
    _bool CanHitAction() { return m_CanHitAction; }

    const map<CUTSCENE_ANIMATION_TYPE, string>& Get_CutSceneAnims() const
    {
        return m_CutSceneAnimation;
    }

    void Set_CutSceneIndex(CUTSCENE_ANIMATION_TYPE eAnim)
    {
        m_eCutSceneType = eAnim;
    }

    void Set_PlayerStop(_bool isStop) { m_isStop = isStop; }


    void Set_TargetObject(class CGameObject* pTargetObject) {
        m_pTargetObject = reinterpret_cast<CMonster*>(m_pTargetObject);
    }


    /* Getter */
public:
    _uint Get_BattleStyle() { return m_eCurrentStyle; }

    _bool isAttack() { return m_iCurrentBehavior == static_cast<_uint>(KRS_BEHAVIOR_STATE::ATTACK); }
    _bool isDown() { return m_iCurrentBehavior == static_cast<_uint>(KRS_BEHAVIOR_STATE::DOWN); }
    _bool isGrab() { return (m_eCurrentStyle == KRS || m_eCurrentStyle == KRC) &&
        m_iCurrentBehavior == static_cast<_uint>(KRS_BEHAVIOR_STATE::GRAB); }
    _uint Get_CurrentHitLevel() { return CPlayer::PlayerInfo.iCurrentHitLevel; }
    _float Get_HitGauage() {  return CPlayer::PlayerInfo.fHitGauge;}

    const _bool* Get_MoveDirection() {
        return m_MoveDirection;
    }

    class CLandObject* Get_TargetObject() {
        return reinterpret_cast<CLandObject*>(m_pTargetObject);
    }

    CCollider* Get_PlayerCollider() {
        return m_pColliderCom;
    }

    class CItem* Get_CurrentItem() {
        return m_pTargetItem;
    }

    _uint Get_CurrentAnimType() {
        return m_eAnimComType;
    }

    /* Setter */
public:
    //void    Set_StartPos(XMMATRIX    vStartPos) { m_pTransformCom->Set_WorldMatrix(vStartPos); }
    //void    Set_NavigationIndex(int iIndex) { m_pNavigationCom->Set_Index(iIndex); }
    //void    Set_NaviRouteIndex(int iIndex) { m_iNaviRouteNum = iIndex; }
    void    Set_SeizeOff(_bool isOff = true);
    void    Set_ItemOff();
    void    Set_CurrentStyle(BATTLE_STYLE iStyle) { m_eCurrentStyle = iStyle; }
    void    Set_CurrentBehavior(_uint iStyle) { m_iCurrentBehavior = iStyle; }

    void Battle_Start();

    void Play_Kuze_QTE(class CMonster* pMonster);


    /* Virtual Funtion */
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void Sound_Event() override;

    // �浹�Լ�
    virtual void Attack_Event(CGameObject* pHitObject, _bool isItem = false) override;
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) override;

    virtual string Get_CurrentAnimationName() override;

    /* Initial Function */
private:
    void Ready_AnimationTree();
    void Ready_CutSceneAnimation();
    void Ready_AuraEffect();
    void Ready_RootFalseAnimation();

    /* Compute Function */
private:
    void Synchronize_Root(const _float& fTimeDelta);
    // �� ��ü�� Look���������ؼ� ���ֺ����ִ���, �����ִ��� ���� �Ǵ�
    // ��� ��ü�� ���� �ٶ󺸰� ������ ��������, ��/��/��/�� ��� ���� �������� �Ǵ��ϴ� �Լ�
    _int Compute_Target_Direction(CLandObject* pAttackedObject);                

    // ��� ��ü�� �������� �������� �����
    // �� ��ġ�� �������� ��/��/��/�� ��� �����ϴ����� �Ǵ�
    _int Compute_Target_Direction_Pos(_fvector vTargetPos);


    //Ű �Է°����Լ���
private:
    void KeyInput(const _float& fTimeDelta);       //���� �÷��̾��� ���¿� ���� Ű�Է�ó�� �ϴ� �Լ�

    // ��庥ó ��忡���� Ű�Է�
    void Adventure_KeyInput(const _float& fTimeDelta);      
    // ���Ѵ� ��� Ű�Է�
    void KRS_KeyInput(const _float& fTimeDelta);  
    // ���� ��� Ű�Է�
    void KRH_KeyInput(const _float& fTimeDelta);      
    // �ı��� ��� Ű�Է�
    void KRC_KeyInput(const _float& fTimeDelta);

    /* �ִϸ��̼� ���� �Լ��� */
public:
    void Change_Animation(_uint iAnimIndex, _float fInterval = 4.f);
    void Change_ResetAnimaition(_uint iAnimIndex, _float fInterval = 4.f);
    void Style_Change(BATTLE_STYLE eStyle);
    void Reset_MoveDirection();

    void Set_CutSceneStartMotion(CUTSCENE_ANIMATION_TYPE eType);
    void Set_CutSceneAnim(CUTSCENE_ANIMATION_TYPE eType);
    void Play_CutScene();
    void Reset_CutSceneEvent();

    void HitAction_Down();
    void HitAction_WallBack();
    void HitAction_CounterElbow();

    void Update_RootFalseAnim();

private:
    void Compute_MoveDirection_FB();
    void Compute_MoveDirection_RL();

    void Effect_Control_Aura();
    void On_Aura(BATTLE_STYLE eStyle);
    void Off_Aura(BATTLE_STYLE eStyle);

    void Setting_Target_Enemy();
    void Setting_Target_Item();
    void Setting_Target_Wall();

    void Radial_Event();
    void Face_Event();


    /* ĳ���� �������ͽ� ���� �Լ� */ 
public:
    void AccHitGauge();

    /* ���̴� ���ε� ���� �Լ� */
public:
    void Setting_RimLight();
    HRESULT Bind_RimLight();

    /* ��Ÿ ��� */
public:
    void HitFreeze_On();
    void HitFreeze_Off();
    void HitFreeze_Timer(const _float& fTimeDelta);

    void HitRadial_On();
    void HitRadial_Off();
    void HitRadial_Timer(const _float& fTimeDelta);

    void HitZoomIn_On();
    void HitZoomIn_Off();
    void HitZoomIn_Timer(const _float& fTimeDelta);

    
    /* ���, �ൿ ���� ������ ������ */
private:
    CAnim* m_pAnimCom = { nullptr };                // �ƽ� ���� �� ����ϴ� �ִϸ��̼� ������Ʈ
    CModel* m_pCameraModel = { nullptr };           // ī�޶� ���� �� ���� �� (������������)

    // �� ��, ����ϴ� Ű ���� �ൿ�� ���� Ű���� ������. (��Ÿ���� �迭 �ε���)
    map<_uint, class CBehaviorAnimation*> m_AnimationTree[BATTLE_STYLE_END];
    //ui
    class CUIManager* m_pUIManager = { nullptr };
    
#ifdef _DEBUG
    class CDebugManager* m_pDebugManager = { nullptr };
#endif // _DEBUG

    /* �÷��̾� ��� ���� ������ ������ */
private:
    class CMonster* m_pTargetObject = { nullptr };
    class CItem* m_pTargetItem = { nullptr };
    CCollider* m_pTargetWall = { nullptr };

    /* �ൿ, �̵� ���� ������ */
private:
    BATTLE_STYLE    m_eCurrentStyle = { ADVENTURE };
    // ��Ÿ�ϸ��� ��ġ�� �ൿ�� ���� �� �־ int������ �����ϰ� ����ȯ�Ͽ� �����Ѵ�.
    _uint           m_iCurrentBehavior = static_cast<_uint>(KRS_BEHAVIOR_STATE::BTL_START);

    // �����̿� Ű�Է� �𷺼� (ī�޶� �������� ó���ϱ� ����)
    _bool                       m_InputDirection[MOVE_DIRECTION_END];
    _bool                       m_MoveDirection[MOVE_DIRECTION_END];

    _float                      m_fPrevSpeed = { 0.f };
    _float4                     m_vPrevMove;
    _float4                     m_vPrevRotation;
    _float4x4                   m_ModelWorldMatrix;

    _bool                       m_isAuraOn = { false };

    /* �ִϸ��̼� ���� */
private:
    ANIMATION_COMPONENT_TYPE    m_eAnimComType = { DEFAULT };
    _bool                       m_isCutSceneStartMotion = { false };
    map<CUTSCENE_ANIMATION_TYPE, string> m_CutSceneAnimation;
    vector<_uint>               m_RootFalseAnims;

    _uint                       m_iCutSceneAnimIndex = { 0 };
    _uint                       m_iCutSceneCamAnimIndex = { 0 };

    _uint                       m_iDefaultAnimIndex = { 0 };

    _bool                       m_CanHitAction = { false };

    int                         m_iNaviRouteNum = { 0 }; //��Ʈ

    _bool                       m_isRootAnim = { true };


    /* �÷��̾� ���� ����Ʈ ��Ƶα� */
private:
    vector<CSocketEffect*> m_HooliganAura;
    vector<CSocketEffect*> m_RushAura;
    vector<CSocketEffect*> m_DestroyerAura;

private:
    _bool m_isHitFreeze = { false };
    const _float m_fHitFreezeTime = { 0.5f };
    _float m_fHitFreezeTimer = { 0.f };

    _bool m_isHitRadial = { false };
    const _float m_fHitRadialTime = { 0.2f };
    _float m_fHitRadialTimer = { 0.f };

    _bool m_isHitZoom = { false };
    const _float m_fHitZoomTime = { 0.2f };
    _float m_fHitZoomTimer = { 0.f };

#ifdef _DEBUG
    _bool m_isAnimStart = { true };

#endif // _DEBUG
    CUTSCENE_ANIMATION_TYPE m_eCutSceneType = { OI_UPPER };

    class CQteManager* m_pQTEMgr = { nullptr };

    public:
        void Set_Money(_uint iMoney) { m_Money += iMoney; }
        _uint Get_Money() { return m_Money; }
private:
    _uint m_Money = { 1000 };
    _bool m_isStop = { 0 };

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;
    virtual void Compute_Height() override;

public:
    static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};
END
