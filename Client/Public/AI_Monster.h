#pragma once
#include "BTNode.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CAnim;
END

BEGIN(Client)
class CAI_Monster :
    public CBTNode
{
public:
    //��ų ����Ʈ
    enum SKILL {
        SKILL_IDLE,
        SKILL_SHIFT,
        SKILL_SWAY,
        SKILL_HIT, 
        SKILL_CMD,
        SKILL_CMD_A,
        SKILL_CMD_B,
        SKILL_CMD_HEADBUTT,
        SKILL_CMD_RENDA,
        SKILL_JAB,
        SKILL_HIJI_2REN,
        SKILL_RENDA,
        SKILL_PUNCH,
        SKILL_HEAVY,
        SKILL_GUARD_RUN,
        SKILL_RARIATTO, 
        SKILL_DOWN,
        SKILL_ANGRY_CHOP,
        SKILL_ANGRY_KICK,

        SKILL_DAM_BODY_LV01_B,
        SKILL_DAM_BODY_LV01_F,
        SKILL_DAM_BODY_LV02_B,
        SKILL_DAM_BODY_LV02_F,
        SKILL_DAM_BODY_LV02_L,
        SKILL_DAM_BODY_LV02_R,
        SKILL_DAM_DUB_L,
        SKILL_DAM_DUB_R,
        SKILL_DAM_DUB_TRAMPLE,
        SKILL_DAM_DNF_L,
        SKILL_DAM_DNF_R,
        SKILL_DAM_DNF_TRAMPLE,
        SKILL_DAM_HEAD_LV01_B,
        SKILL_DAM_HEAD_LV01_F,
        SKILL_DAM_HEAD_LV01_L,
        SKILL_DAM_HEAD_LV01_R,
        SKILL_DAM_HEAD_LV02_B,
        SKILL_DAM_HEAD_LV02_F,
        SKILL_DAM_HEAD_LV02_L,
        SKILL_DAM_HEAD_LV02_R,
        SKILL_DAM_LEG_LV01_B,
        SKILL_DAM_LEG_LV01_F,
        SKILL_DAM_LEG_LV01_L,
        SKILL_DAM_LEG_LV01_R,
        SKILL_DAM_LEG_LV02_L,
        SKILL_DAM_LEG_LV02_R,

        SKILL_DED_B_01,
        SKILL_DED_B_02,
        SKILL_DED_DND_01,
        SKILL_DED_DND_02,
        SKILL_DED_DND_L,
        SKILL_DED_DND_R,
        SKILL_DED_DNF_01,
        SKILL_DED_DNF_02,
        SKILL_DED_DNF_L,
        SKILL_DED_DNF_R,
        SKILL_DED_F_01,
        SKILL_DED_F_02,
        SKILL_DED_F_03,
        SKILL_DED_FALL_B,
        SKILL_DED_FALL_F,
        SKILL_DED_L,
        SKILL_DED_R,
        SKILL_DED_B,
        SKILL_DWN_B_PAIN_BODY_EN,
        SKILL_DWN_B_PAIN_BODY_LP,
        SKILL_DWN_B_PAIN_BODY_ST,
        SKILL_DWN_B_PAIN_HEAD_EN,
        SKILL_DWN_B_PAIN_HEAD_LP,
        SKILL_DWN_B_PAIN_HEAD_ST,
        SKILL_DWN_B_PAIN_LEG_EN,
        SKILL_DWN_B_PAIN_LEG_LP,
        SKILL_DWN_B_PAIN_LEG_ST,
        SKILL_DWN_BACK_B,
        SKILL_DWN_BACK_F,
        SKILL_DWN_BODY_D,
        SKILL_DWN_BODY_F,
        SKILL_DWN_BODY_L,
        SKILL_DWN_BODY_R,
        SKILL_END
    };

    typedef struct tAIMonster {
        class CAnim* pAnim;
        class CMonster* pThis;
        _uint* pState;
    } AI_MONSTER_DESC;

protected:
    CAI_Monster();
    CAI_Monster(const CAI_Monster& rhs);
    virtual ~CAI_Monster() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute() = 0;

protected:
    CBTNode* m_pRootNode = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };
    CAnim* m_pAnimCom = { nullptr };

    class CPlayer* m_pPlayer = { nullptr }; // �÷��̾��� �ּҸ� ������ �ֵ��� �Ѵ�.
    class CMonster* m_pThis = { nullptr }; // �ڱ� �ڽ��� �ּҸ� ������ �ֵ��� �Ѵ�.

    _uint* m_pState = { nullptr };

protected:
    _bool m_isAttack = { false };
    _bool m_isAngry = { false };
    _bool m_isSync = { false };
    _bool m_isShift = { false };
    _bool m_isBreak = { false };

    _uint m_iSkill = { SKILL_END };

protected:
    _float m_fDelayAttackDuration = { 2.f };
    _float m_fAttackDelayTime = { 0.f }; // ������ ���� �� ���ӽð�

    _float m_fBreakDuration = { 0.f }; // �������� �ο��޴´�.
    _float m_fBreakTime = { 0.f };

    _float m_fCmbNum = { 0.f }; // ��ų �ߵ��� Ȯ�������� ��� �����Ұ����� ����.
    _float m_fCmbCount = { 0.f }; // �ߺ� ���� ī��Ʈ

protected:
    //����
    virtual CBTNode::NODE_STATE Check_Death();

    //�÷��̾� ���� üũ
    virtual CBTNode::NODE_STATE Check_Sway();
    virtual CBTNode::NODE_STATE Sway();

    //�÷��̾�� �����ִ°�?
    virtual CBTNode::NODE_STATE Chcek_Sync(); // ��ũ ����� �ؾ��ϴ°�?
    virtual CBTNode::NODE_STATE Sync_Neck(); // ��ũ��� : ���

    //������ DMD
    virtual CBTNode::NODE_STATE Check_Hit();
    virtual CBTNode::NODE_STATE HitAndGuard(); //��Ʈ����� �� ������, ���带 �� ������

    // �÷��̾� ���� ��ų���� ��Ʈ��� �ٸ�.
    virtual CBTNode::NODE_STATE Normal_Hit(); // ��Ʈ üũ�� �ϸ鼭 �׾����� ���� �ʾҴ����� ���� Ȯ���ؾ���.
    virtual CBTNode::NODE_STATE Strong_Hit();

    // ����
    virtual CBTNode::NODE_STATE Guard();

    //�г�
    virtual CBTNode::NODE_STATE Check_Angry(); // ȭ�� �� �������� üũ
    virtual CBTNode::NODE_STATE Angry();

    virtual CBTNode::NODE_STATE ATK_Angry_Punch(); // ȭ�� �� �����϶� ��ġ
    virtual CBTNode::NODE_STATE ATK_Angry_Kick(); // ȭ�� �� �����϶� ������

    //Break
    virtual CBTNode::NODE_STATE Check_Break(); // ���� Ÿ���ΰ�?
    virtual CBTNode::NODE_STATE ShiftAndIdle(); // ���� ������ �б�ó��
    virtual CBTNode::NODE_STATE Shift(); // ����Ʈ(�ȱ�)
    virtual CBTNode::NODE_STATE Idle(); // ����

protected:
    virtual void Ready_Tree();
    _float DistanceFromPlayer();
    _bool isBehine();
    void LookAtPlayer();

protected:
    _bool Find_CurrentAnimationName(string strAnimName);
    void Check_KRH(_uint iPlayerLv);
    void Check_KRS(_uint iPlayerLv);
    void Check_KRC(_uint iPlayerLv);

public:
    virtual void Free();
};

END