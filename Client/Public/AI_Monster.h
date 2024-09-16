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
        SKILL_RUN,
        SKILL_SHIFT,
        SKILL_SWAY,
        SKILL_HIT,
        SKILL_ATK_NORMAL,
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
        SKILL_RUN_ATK,
        SKILL_KICK,
        SKILL_RARIATTO,
        SKILL_DOWN,
        SKILL_ANGRY_CHOP,
        SKILL_ANGRY_KICK,
        SKILL_QTE,

        SKILL_GUARD,
        SKILL_DEAD,
        SKILL_END
    };
    enum PLAYER_ATK_DIR { F, B, L, R, PLAYER_ATK_DIR_END };

    typedef struct tAIMonster {
        class CAnim* pAnim[2];
        class CMonster* pThis;
        _uint* pState;
        _uint* pCurrentAnimType;
    } AI_MONSTER_DESC;

protected:
    CAI_Monster();
    CAI_Monster(const CAI_Monster& rhs);
    virtual ~CAI_Monster() = default;

public:
    _bool isSway() { return m_isSway; }
    _bool isGuard() { return m_isGuard; }
    _bool isAttack() { return m_isAttack; }
    _uint Get_DownDir();

public:
    void Set_Sync(_bool isSync) { m_isSync = isSync; }
    void Set_Start(_bool isStart) { m_isStart = isStart; }
    void Set_Adventure(_bool isAdventure) { m_isAdventer = isAdventure; }
    void Reset_AI();

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute() = 0;

    _bool Find_PlayerCurrentAnimationName(string strAnimName);

protected:
    CBTNode* m_pRootNode = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };
    CAnim* m_pAnimCom[2] = {nullptr};

    class CPlayer* m_pPlayer = { nullptr }; // �÷��̾��� �ּҸ� ������ �ֵ��� �Ѵ�.
    class CMonster* m_pThis = { nullptr }; // �ڱ� �ڽ��� �ּҸ� ������ �ֵ��� �Ѵ�.

    _uint* m_pState = { nullptr };
    _uint* m_pCurrentAnimType = { nullptr };

protected:
    _bool m_isStart = { false };
    _bool m_isAdventer = { false };
    _bool m_isAttack = { false };
    _bool m_isAngry = { false };
    _bool m_isSync = { false };
    _bool m_isBreak = { false };
    _bool m_isGuard = { false };
    _bool m_isSway = { false };
    _bool m_isRun = { false };

    _bool m_isQTE = { false }; // qte�� �ѹ��� ������ �ؼ� �ʿ���.

    _bool m_isPlayerDownAtk = { false }; // �÷��̾�� �ٿ� ������ �ѹ� ����ߴ°��� üũ

    _uint m_iSkill = { SKILL_END };
    _uint m_iMonsterType = { 0 };

    _bool m_isSycnSound = { false };

protected:
    _float m_fDelayAttackDuration = { 2.f };
    _float m_fAttackDelayTime = { 0.f }; // ������ ���� �� ���ӽð�

    _float m_fBreakDuration = { 0.f }; // �������� �ο��޴´�.
    _float m_fBreakTime = { 0.f };

    _float m_fGuardDuration = { 4.f }; // ���� ���ӽð�
    _float m_fGuardTime = { 0.f };

    _float m_fSyncDuration = { 3.f }; // ��ũ�׼� ���ӽð�
    _float m_fSyncTime = { 0.f };

protected:
    _float m_fCmbNum = { 0.f }; // ��ų �ߵ��� Ȯ�������� ��� �����Ұ����� ����.
    _float m_fCmbCount = { 0.f }; // �޺� ���� ī��Ʈ

    _float m_fGuardBroken = { 20.f };
    _float m_fGuardAtkAcc = { 0.f };

protected:
    _float m_fSoundVoice = { 1.f };
    _float m_fSoundEffect = { 1.f };

protected:
    _float2 m_fSwayDistance = {};

protected:
    //���� ���� �� ���� ���
    virtual CBTNode::NODE_STATE Check_Start();
    virtual CBTNode::NODE_STATE Start();

    //�÷��̾�� �����ִ°�?
    virtual CBTNode::NODE_STATE Chcek_Sync(); // ��ũ ����� �ؾ��ϴ°�?

    //�ٿ����
    virtual CBTNode::NODE_STATE Check_Down();
    virtual CBTNode::NODE_STATE StandUpAndDead();
    virtual CBTNode::NODE_STATE StandUp();
    virtual CBTNode::NODE_STATE Dead();

    //�÷��̾� �ٿ� �� (�ٿ����)
    CBTNode::NODE_STATE Check_PlayerDown();
    CBTNode::NODE_STATE ATK_Down();

    //������
    virtual CBTNode::NODE_STATE Check_Sway();
    virtual CBTNode::NODE_STATE Sway();

    //������ DMD
    virtual CBTNode::NODE_STATE Check_Hit();
    virtual CBTNode::NODE_STATE HitAndGuard(); //��Ʈ����� �� ������, ���带 �� ������

    virtual CBTNode::NODE_STATE Hit();
    virtual CBTNode::NODE_STATE Guard();

    //�г�
    virtual CBTNode::NODE_STATE Check_Angry(); // ȭ�� �� �������� üũ
    virtual CBTNode::NODE_STATE Angry();

    virtual CBTNode::NODE_STATE ATK_Angry_Punch(); // ȭ�� �� �����϶� ��ġ
    virtual CBTNode::NODE_STATE ATK_Angry_Kick(); // ȭ�� �� �����϶� ������

    //����
    CBTNode::NODE_STATE Check_Attack(); // ���� ������ �������� üũ

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
    _uint Check_KRH(_uint iPlayerLv, _bool isBehine, _bool isAnimChange = true);
    _uint Check_KRS(_uint iPlayerLv, _bool isBehine, _bool isAnimChange = true);
    _uint Check_KRC(_uint iPlayerLv, _bool isBehine, _bool isAnimChange = true);
    _bool Check_StandUp();

protected:
    void Reset_State() {
        m_isAttack = false;
        m_isBreak = false;
        m_isSway = false;
        m_isGuard = false;
        m_isRun = false;

        m_fBreakTime = 0.f;
        m_fAttackDelayTime = 0.f;
    }

public:
    virtual void Free();
};

END