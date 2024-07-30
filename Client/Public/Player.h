#pragma once
#include "LandObject.h"
#include "Navigation.h"

BEGIN(Engine)
class CShader;
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
        SWAY, DOWN, GRAB,
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
        SWAY, DOWN, GRAB,
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
        FINISHBLOW,                 //H23320 �ǴϽ� ����� ��
        GOUGEKI_C,                  // H20021 ��� �� ��Ʈ�׼� (�ָ����� �󱼶����� ��ġ��)
        HAIHEKI_KICK,               //H23070 �� ������ ����ġ�� ��ų

        /* ���� */
        KOBUSHIKUDAKI,              //H10111 �Ȳ�ġ�� ���ݸ��� ��ų
        HAIHEKI_PUNCH,              //H23060 �� ������ ������ ��ų
        OI_TRAMPLE_AO,              //H1500 �ٿ�� ��� �� ��� (�����ִ� ����), ����
        OI_KICKOVER_UTU_C,          //H1511 �ٿ�� ��� �� ���� (������ִ� ����), ����

        /* �ı��� */
        KIRYU_GSWING,               //H1010 �ٸ���� ������ ���ø� ����
        DORYU_MIN,                  //H11285 ������ ������ ���ø� ����
        NAGE_OIUCHI_NECK,           //H1540 �� �ٴڿ� ������
        POLE_KNOCK_LAPEL,           //H2040 ��ó�� ����� �ִٸ� ��տ� �ڰ��ϰ� ����
        DORAMUKAN_88,               //H3261 ū ���� (����)�� ��� ���� �а� ����ħ
        MONZETSU,                   //H11250 ���ٰ� �������� �˲����� (�ڿ��� ��������� ���)

        /* �⺻ ��Ʈ�׼� */
        WALL_KNOCK_NECK_C,          //H2011 ���� �Ӹ��ڰ��ϰ� ���� (�Ƹ� ��� ���� ��Ʈ�׼����� �� ��ó�� �����ִٸ�)
        KABE_AIRON,                 //H23000 ������ ��ġ�� ����
        OI_KICK,                    //H23010 �Ӹ�ä ��� �� �߷��� (���帰 ����)
        OI_UPPER,                   //H23020 �Ӹ�ä��� �� �ָ����� ħ (�����ִ� ����)

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
#pragma endregion

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

    _bool isAnimStart() { return m_isAnimStart; }

    const map<CUTSCENE_ANIMATION_TYPE, string>& Get_CutSceneAnims() const
    {
        return m_CutSceneAnimation;
    }

    void Set_CutSceneIndex(CUTSCENE_ANIMATION_TYPE eAnim)
    {
        m_eCutSceneType = eAnim;
    }


#endif // DEBUG

    /* Getter */
public:
    _uint Get_BattleStyle() { return m_eCurrentStyle; }

    _bool isAttack() { return m_iCurrentBehavior == static_cast<_uint>(KRS_BEHAVIOR_STATE::ATTACK); }
    _bool isDown() { return m_iCurrentBehavior == static_cast<_uint>(KRS_BEHAVIOR_STATE::DOWN); }
    _uint Get_CurrentHitLevel() { return m_iCurrentHitLevel; }

    const _bool* Get_MoveDirection() {
        return m_MoveDirection;
    }

    CLandObject* Get_TargetObject() {
        return m_pTargetObject;
    }

    CCollider* Get_PlayerCollider() {
        return m_pColliderCom;
    }
    


    /* Setter */
public:
    void    Set_StartPos(XMMATRIX    vStartPos) { m_pTransformCom->Set_WorldMatrix(vStartPos); }
    void    Set_NavigationIndex(int iIndex) { m_pNavigationCom->Set_Index(iIndex); }
    void    Set_NaviRouteIndex(int iIndex) { m_iNaviRouteNum = iIndex; }
    void    Set_SeizeOff(_bool isOff = true);                       //TODO: ���� ���¿��� off ��ų �� �θ� �Լ�
    
    void    Set_FaceAnimIndex(_uint iFaceAnimIndex) { m_iFaceAnimIndex = iFaceAnimIndex; }

    /* Virtual Funtion */
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

    // �浹�Լ�
    virtual void Attack_Event(CLandObject* pHitObject) override;
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) override;

    virtual string Get_CurrentAnimationName() override;

    /* Initial Function */
private:
    void Ready_AnimationTree();
    void Ready_CutSceneAnimation();

    /* Compute Function */
private:
    void Synchronize_Root(const _float& fTimeDelta);
    _int Compute_Target_Direction(CLandObject* pAttackedObject);

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

    void Set_CutSceneAnim(CUTSCENE_ANIMATION_TYPE eType, _uint iFaceAnimIndex);
    void Play_CutScene();
    void Reset_CutSceneEvent();

    // iHandType: 0���, 1 �޼�, 2 ������
    void On_Separation_Hand(_uint iHandType = 0);
    void Off_Separation_Hand(_uint iHandType = 0);
    void On_Separation_Face();
    void Off_Separation_Face();

private:
    void Compute_MoveDirection_FB();
    void Compute_MoveDirection_RL();

    void Effect_Control_Aura();
    void Setting_Target_Enemy();

    /* ĳ���� �������ͽ� ���� �Լ� */ 
public:
    void AccHitGauge();

    /* ���̴� ���ε� ���� �Լ� */
public:
    void Setting_RimLight();
    HRESULT Bind_RimLight();

    
    /* ���, �ൿ ���� ������ ������ */
private:
    CNavigation* m_pNavigationCom = { nullptr };

    CAnim* m_pAnimCom = { nullptr }; // �ִϸ��̼Ǹ� ���� �����ϰ��ִ� �ִϸ��̼� ������Ʈ
    CModel* m_pCameraModel = { nullptr };

    // �� ��, ����ϴ� Ű ���� �ൿ�� ���� Ű���� ������. (��Ÿ���� �迭 �ε���)
    map<_uint, class CBehaviorAnimation*> m_AnimationTree[BATTLE_STYLE_END];
    //ui
    class CUIManager* m_pUIManager = { nullptr };
    
#ifdef _DEBUG
    class CDebugManager* m_pDebugManager = { nullptr };
#endif // _DEBUG

    /* �÷��̾� ��� ���� ������ ������ */
private:
    CLandObject* m_pTargetObject = { nullptr };

    /* �ൿ, �̵� ���� ������ */
private:
    BATTLE_STYLE    m_eCurrentStyle = { KRS };
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
    ANIMATION_COMPONENT_TYPE    m_eAnimComType = { DEFAULT_ANIMAITION };
    map<CUTSCENE_ANIMATION_TYPE, string> m_CutSceneAnimation;

    _uint                       m_iCutSceneAnimIndex = { 0 };
    _uint                       m_iCutSceneCamAnimIndex = { 0 };

    /* �÷��̾� �������ͽ� ���� ������ */
private:
    _uint           m_iCurrentHitLevel = { 0 };
    _float          m_fHitGauge = { 0.f };

    int             m_iNaviRouteNum = { 0 }; //��Ʈ

    _uint           m_iFaceAnimIndex = { 0 };

#ifdef _DEBUG
    _bool m_isAnimStart = { true };
    CUTSCENE_ANIMATION_TYPE m_eCutSceneType = { OI_UPPER };
#endif // _DEBUG

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
