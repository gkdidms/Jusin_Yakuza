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
    typedef struct tMapMonsterObjDesc : public CLandObject::LANDOBJ_MAPDESC
    {
    }MONSTER_IODESC;

public:
    enum MONSTER_TYPE { RUSH, WPA, MONSTER_DEFAULT, WPH, SHAKDDOWN, YONEDA, KUZE, MONSTER_TYPE_END };
    enum MONSTER_STATE
    {
        //���� ���
        MONSTER_IDLE,
        MONSTER_ADVENTURE_IDLE_1,
        MONSTER_ADVENTURE_IDLE_2,
        MONSTER_ADVENTURE_IDLE_3,
        MONSTER_ADVENTURE_IDLE_4,
        MONSTER_BTLST,
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
        MONSTER_ATK_NORAML,
        MONSTER_CMB_1,
        MONSTER_CMB_2,
        MONSTER_CMB_3,
        MONSTER_CMB_4,
        MONSTER_CMB_5,
        MONSTER_CMB_A_1,
        MONSTER_CMB_A_2,
        MONSTER_CMB_A_3,
        MONSTER_CMB_B_1,
        MONSTER_CMB_B_2,
        MONSTER_CMB_B_3,
        MONSTER_CMB_HEADBUTT_1,
        MONSTER_CMB_HEADBUTT_2,
        MONSTER_CMB_RENDA_1,
        MONSTER_CMB_RENDA_2,
        MONSTER_CMB_RENDA_3,
        MONSTER_PUNCH,
        MONSTER_JAB,
        MONSTER_HEAVY_ATTACK,
        MONSTER_HEAVY_RENDA,
        MONSTER_ATK_DOWN,
        MONSTER_GUARD_RUN,
        MONSTER_RARIATTO,
        MONSTER_HIJI_2REN,
        MONSTER_3REN,
        MONSTER_KICK,
        MONSTER_RUN_ATK,
        MONSTER_ANGRY_START,
        MONSTER_ANGRY_CHOP,
        MONSTER_ANGRY_KICK,
        MONSTER_GURAD_START,
        MONSTER_GURAD_LOOP,
        MONSTER_GURAD_END,
        MONSTER_GURAD_FLOAT,

        //�´� ���
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
        MONSTER_DWN_STANDUP_HEAD_R_EN,
        MONSTER_STANDUP_DNF_FAST,
        MONSTER_STANDUP_DNB_FAST,
        MONSTER_DED_L, // ����
        MONSTER_DED_R,
        MONSTER_DED_F_1,
        MONSTER_DED_F_2,
        MONSTER_DED_F_3,
        MONSTER_DED_B_1,
        MONSTER_DED_B_2,

        //��ũ�׼�
        MONSTER_KTA_SYNC1_ATK_TO_MOUNT_ST,//e_kta_sync1_atk_to_mount_st
        MONSTER_KTA_SYNC1_GURAD_REVERSE_EN, //e_kta_sync1_gurad_reverse_en
        MONSTER_KTA_SYNC1_GURAD_REVERSE_LP, //e_kta_sync1_gurad_reverse_lp
        MONSTER_KTA_SYNC1_GURAD_REVERSE_ST, // e_kta_sync1_gurad_reverse_st
        MONSTER_KTA_SYNC1_LAPEL_RESIST, //e_kta_sync1_lapel_resist
        MONSTER_KTA_SYNC1_LEG_NAGE_B, //e_kta_sync1_leg_nage_b
        MONSTER_KTA_SYNC1_LEG_NAGE_F, //e_kta_sync1_leg_nage_f 
        MONSTER_KTA_SYNC1_NECK_RESIST, //e_kta_sync1_neck_resist
        MONSTER_KUZ_PIPE_SYNC1_ANTI_SWAY,//e_kuz_pipe_sync1_anti_sway
        MONSTER_KUZ_SYNC1_ANTI_SWAY,//e_kuz_sync1_anti_sway
        MONSTER_NSK_SYNC1_ANTIWPN_NAGE,//e_nsk_sync1_antiwpn_nage
        MONSTER_NSK_SYNC1_LAPEL_YA_NAGE,//e_nsk_sync1_lapel_ya_nage
        MONSTER_NSK_SYNC1_RUNTACKLE_NAGE,//e_nsk_sync1_runtackle_nage
        MONSTER_SAE_SYNC1_AIR_HEAD_B, //e_sae_sync1_air_head_b
        MONSTER_SAE_SYNC1_AIR_HEAD_F, //e_sae_sync1_air_head_f
        MONSTER_SAE_SYNC1_AIR_LEG_B, //e_sae_sync1_air_leg_b
        MONSTER_SAE_SYNC1_AIR_LEF_F, //e_sae_sync1_air_leg_f
        MONSTER_SAE_SYNC1_LAPEL_ATK_EN, //e_sae_sync1_lapel_atk_en
        MONSTER_SAE_SYNC1_LAPEL_ATK_TAME_EN, // e_sae_sync1_lapel_atk_tame_en
        MONSTER_SAE_SYNC1_LAPEL_ATK_TAME_ST, //e_sae_sync1_lapel_atk_tame_st
        MONSTER_SAE_SYNC1_LAPEL_CMB_3, //e_sae_sync1_lapel_cmb_03
        MONSTER_SAE_SYNC1_LAPEL_NAGE, //e_sae_sync1_lapel_nage
        MONSTER_SAE_SYNC1_LEG_ATK_EN, //e_sae_sync1_leg_atk_en
        MONSTER_SAE_SYNC1_LEG_ATK_TAME_EN,//e_sae_sync1_leg_atk_tame_en
        MONSTER_SAE_SYNC1_LEG_ATK_TAME_ST, //e_sae_sync1_leg_atk_tame_st
        MONSTER_SAE_SYNC1_LEG_CMB_01,//e_sae_sync1_leg_cmb_01
        MONSTER_SAE_SYNC1_LEG_CMB_02,//e_sae_sync1_leg_cmb_02
        MONSTER_SAE_SYNC1_LEG_CMB_03, //e_sae_sync1_leg_cmb_03
        MONSTER_SAE_SYNC1_LEG_NAGE, //e_sae_sync1_leg_nage
        MONSTER_SAE_SYNC1_NECK_ATK_EN, // e_sae_sync1_neck_atk_en
        MONSTER_SAE_SYNC1_NECK_ATK_TAME_EN, //e_sae_sync1_neck_atk_tame_en
        MONSTER_SAE_SYNC1_NECK_ATK_TAME_ST,//e_sae_sync1_neck_atk_tame_st
        MONSTER_SAE_SYNC1_NECK_CMB_03,//e_sae_sync1_neck_cmb_03
        MONSTER_SAE_SYNC1_NECK_NAGE,//e_sae_sync1_neck_nage
        MONSTER_SAE_SYNC1_RUN_B,//e_sae_sync1_run_b
        MONSTER_SAE_SYNC1_RUN_F,//e_sae_sync1_run_f
        MONSTER_KRC_SYNC1_GUARD_COUNTER_F,//p_krc_sync1_guard_counter_f
        MONSTER_KRC_SYNC1_KAIHI_NAGE_B,//p_krc_sync1_kaihi_nage_b
        MONSTER_KRC_SYNC1_KAIHI_NAGE_F,//p_krc_sync1_kaihi_nage_f
        MONSTER_KRC_SYNC1_LAPEL_ATK_HEAVY,//p_krc_sync1_lapel_atk_heavy
        MONSTER_KRC_SYNC1_LAPEL_ATK_PUNCH,//p_krc_sync1_lapel_atk_punch
        MONSTER_KRC_SYNC1_LAPEL_NAGE,//p_krc_sync1_lapel_nage
        MONSTER_KRC_SYNC1_NECK_ATK_HEAVY,//p_krc_sync1_neck_atk_heavy
        MONSTER_KRC_SYNC1_NECK_ATK_PUNCH,//p_krc_sync1_neck_atk_punch
        MONSTER_KRC_SYNC1_NECK_NAGE,//p_krc_sync1_neck_nage
        MONSTER_KRH_SYNC1_GUARD_SABAKI,//p_krh_sync1_guard_sabaki
        MONSTER_KRS_SYNC1_CMB_03_FIN,//p_krs_sync1_cmb_03_fin
        MONSTER_KRS_SYNC1_CMB_03_FIN_B,//p_krs_sync1_cmb_03_fin_b
        MONSTER_KRU_SYNC1_LAPEL_ATK_PUNCH,//p_kru_sync1_lapel_atk_punch
        MONSTER_KRU_SYNC1_LAPEL_CMB_01,//p_kru_sync1_lapel_cmb_01
        MONSTER_KRU_SYNC1_LAPEL_CMB_02,//p_kru_sync1_lapel_cmb_02
        MONSTER_KRU_SYNC1_LAPEL_CMB_03,//p_kru_sync1_lapel_cmb_03
        MONSTER_KRU_SYNC1_LAPEL_LP,//p_kru_sync1_lapel_lp
        MONSTER_KRU_SYNC1_LAPEL_NAGE,//p_kru_sync1_lapel_nage
        MONSTER_KRU_SYNC1_LAPEL_OFF,//p_kru_sync1_lapel_off
        MONSTER_KRU_SYNC1_LAPEL_OFF_HIZA,//p_kru_sync1_lapel_off_hiza
        MONSTER_KRU_SYNC1_LAPEL_PRESS,//p_kru_sync1_lapel_press
        MONSTER_KRU_SYNC1_LAPEL_RESIST,//p_kru_sync1_lapel_resist
        MONSTER_KRU_SYNC1_LAPEL_ST,//p_kru_sync1_lapel_st
        MONSTER_KRU_SYNC1_LAPEL_WALK,//p_kru_sync1_lapel_walk
        MONSTER_KRU_SYNC1_NECK_ATK_KICK,//p_kru_sync1_neck_atk_kick
        MONSTER_KRU_SYNC1_NECK_CMB_01,//p_kru_sync1_neck_cmb_01
        MONSTER_KRU_SYNC1_NECK_CMB_02,//p_kru_sync1_neck_cmb_02
        MONSTER_KRU_SYNC1_NECK_CMB_03,//p_kru_sync1_neck_cmb_03
        MONSTER_KRU_SYNC1_NECK_LP,//p_kru_sync1_neck_lp
        MONSTER_KRU_SYNC1_NECK_NAGE,//p_kru_sync1_neck_nage
        MONSTER_KRU_SYNC1_NECK_OFF,//p_kru_sync1_neck_off
        MONSTER_KRU_SYNC1_NECK_OFF_URAKEN,//p_kru_sync1_neck_off_uraken
        MONSTER_KRU_SYNC1_NECK_PRESS,//p_kru_sync1_neck_press
        MONSTER_KRU_SYNC1_NECK_RESIST,//p_kru_sync1_neck_resist
        MONSTER_KRU_SYNC1_NECK_ST,//p_kru_sync1_neck_st
        MONSTER_KRU_SYNC1_NECK_WALK, //p_kru_sync_neck_walk
        MONSTER_SYNC1_COUNTER_F,//p_sync1_counter_f
        MONSTER_SYNC1_HEAD_B,//p_sync1_head_b
        MONSTER_SYNC1_HEAD_F,//p_sync1_head_f
        MONSTER_SYNC1_LAPEL_TO_NECK,//p_sync1_lapel_to_neck
        MONSTER_SYNC1_LEG_ATK_KICK,//p_sync1_leg_atk_kick
        MONSTER_SYNC1_LEG_LP,//p_sync1_leg_lp
        MONSTER_SYNC1_LEG_NAGE,//p_sync1_leg_nage
        MONSTER_SYNC1_LEG_OFF,//p_sync1_leg_off
        MONSTER_SYNC1_LEG_ST_B,//p_sync1_leg_st_b
        MONSTER_SYNC1_LEG_ST_F,//p_sync1_leg_st_f
        MONSTER_SYNC1_LEG_WALK,//p_sync1_leg_walk
        MONSTER_SYNC1_NECK_TO_LAPEL,//p_sync1_neck_to_lapel

        //�� �׼�
        MONSTER_H11250_000_1,
        MONSTER_H20021_000_2,
        MONSTER_H23000_000_6,
        MONSTER_A60300_000_2,
        MONSTER_A60320_000_4,
        MONSTER_A60330_000_2,
        MONSTER_A60350_000_2,
        MONSTER_H1010_000_1,
        MONSTER_H10111_000_1,
        MONSTER_H11285_000_1,
        MONSTER_H1511_000_4,
        MONSTER_H1540_000_1,
        MONSTER_H1620_000_1,
        MONSTER_H2011_000_1,
        MONSTER_H2040_000_1,
        MONSTER_H23010_000_4,
        MONSTER_H23020_000_4,
        MONSTER_H23060_000_4,
        MONSTER_H23070_000_4,
        MONSTER_H23250_000_2,
        MONSTER_H23320_000_4,
        MONSTER_H3261_000_4,
        MONSTER_H1500_000_1,
        MONSTER_STATE_END
    };

protected:
    CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMonster(const CMonster& rhs);
    virtual ~CMonster() = default;

public:
    _bool isColl() { return m_isColl; }
    _bool isDown() { return m_isDown; }
    _bool isAttack();
    _bool isSynchronizing() { return m_isSynchronizing; }
    _bool isTutorialAttack() {
        _bool isResult = m_isTutorialAttack;
        m_isTutorialAttack = false;
        m_Info.fHp = m_Info.fMaxHP; // �ʱ�ȭ ���ֱ�

        return isResult;
    }
    _bool isScript() { return m_isScript; }
    _bool isQTEResult() { return m_iQTEResult; }

public:
    void Set_Down(_bool isDown) { m_isDown = isDown; }
    void Set_Sync(string strPlayerAnim, _bool isKeepSynchronizing = false);
    void Set_Adventure(_bool isAdventure);
    virtual void Off_Sync();
    void Set_RimLight(_float isRimLight) { m_isRimLight = isRimLight; }
    void Set_Start(_bool isStart); // ���� ���� �� 
    void Set_Script(_bool isScript) { m_isScript = isScript; m_iCurrentAnimType = DEFAULT; }
    void Set_Animation(string strAnimName, _bool isLoop = true);

    void Set_QTEResult(_uint iValue) { m_iQTEResult = iValue; }

    void Reset_Monster();

    void Setting_SyncAnim_EndPosition();

    void Set_isColl(_bool isColl) {
        m_isColl = isColl;
    }

public:
    _float Get_HitDamage() { return m_fHitDamage; }
    _uint Get_DownDir();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

    // �浹�Լ�
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) override;
    virtual void Animation_Event() override;
    virtual string Get_CurrentAnimationName() override;

    // ���: Change_Animation�Լ� ���� �� �ʿ��� �ִϸ��̼� �������ִ� �κп� �����ϰ�,
    // Change_Animation �Լ� ������ ���� �ִϸ��̼� ���濩�θ� Ȯ���ϴ�
    // m_pModelCom->Set_AnimationIndex �Լ� ���� �κ� ���� m_isShaked�� false�� �ʱ�ȭ���ָ� ��
    // 1����: �ش� �ִϸ��̼ǿ��� ����ŷ�� �����ų ��ġ ����
    // 2����: ����ŷ ����
    // 3����: ����ŷ ����
    virtual void Shaking(_float fRatio, _float fShakeDuration = 0.3f, _float fShakeMagnitude = 0.3f);
    void Reset_Shaking_Variable()
    {
        m_isShaked = { false };
        m_iShakedCount = { 0 };
    };

    _bool Checked_Animation_Ratio(_float fRatio);

    void BloodEffect_Event();
    virtual void Sound_Event() override;

protected:
    CAnim* m_pAnimCom[ANIM_TYPE_END] = {nullptr}; // �ִϸ��̼Ǹ� ���� �����ϰ��ִ� �ִϸ��̼� ������Ʈ
    class CAI_Monster* m_pTree = { nullptr };

    class CTutorialManager* m_pTutorialManager = { nullptr };
    class CEffectManager* m_pEffectManager = { nullptr };

protected:
    _bool m_isAnimLoop = { false };
    _uint m_iState = { 0 };
    _uint m_iCurrentAnimType = { ANIM_TYPE_END };
    _uint m_iPreAnimType = { ANIM_TYPE_END };
    _float m_fChangeInterval = { 4.f };

    _float          m_fPrevSpeed;
    _float4         m_vPrevMove;
    _float4         m_vPrevRotation;
    _float4x4       m_ModelWorldMatrix;

    _vector         m_vPlayerDistance;          // �÷��̾�� ��ũ�׼��� ���߱����� �̵��Ÿ��� �����صд�

protected:
    string m_strAnimName = "";
    _uint m_iAnim = { 0 };

    _uint m_iMonsterType = { MONSTER_TYPE_END };

protected:
    _float m_fHitDamage = { 0.f };
    _bool m_isColl = { false }; // �浹�Ǿ����� �ƴ��� üũ�ؾ���.
    _bool m_isDown = { false }; // �ٿ�Ǿ��°�?
    _bool m_isTutorialAttack = { false }; // �浹 �Ǿ����� üũ�� (Ʃ�丮�� ��)    

    /* ����ŷ ���� ������ */
    _bool m_isShaked = { false };       // ����ŷ�� ����Ǿ�����?
    _uint m_iShakedCount = { 0 };       // ��� ° ����Ǿ�����? (�� �ִϸ��̼� ���� ������ ����Ǿ���� ��� ���)

    _bool m_isSynchronizing = { false };            // ��ũ�ִϸ��̼�������?
    _bool m_isKeepSynchronizing = { false };            // ��ũ�ִ��� �����Ұ�����?
    _bool m_isGrabStandup = { false };            // �ִϸ��̼� ȸ���� ���߱� ���� ����

    int     m_iNaviRouteNum = { 0 };

    _uint m_iQTEResult = { 0 };         //0: None(��������) 1: ���� 2: ����

private: // ��ũ��Ʈ ���� ��
    _bool m_isScript = { false };

protected:
    virtual void Change_Animation();
    HRESULT Setup_Animation();
    virtual void Synchronize_Root(const _float& fTimeDelta);

protected:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    virtual void Free() override;
};
END
