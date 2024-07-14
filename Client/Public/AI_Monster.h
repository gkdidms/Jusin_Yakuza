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
    //스킬 리스트
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

    class CPlayer* m_pPlayer = { nullptr }; // 플레이어의 주소를 가지고 있도록 한다.
    class CMonster* m_pThis = { nullptr }; // 자기 자신의 주소를 가지고 있도록 한다.

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
    _float m_fAttackDelayTime = { 0.f }; // 공격이 끝난 후 지속시간

    _float m_fBreakDuration = { 0.f }; // 랜덤으로 부여받는다.
    _float m_fBreakTime = { 0.f };

    _float m_fCmbNum = { 0.f }; // 스킬 발동시 확률적으로 몇번 공격할것인지 저장.
    _float m_fCmbCount = { 0.f }; // 콥보 누적 카운트

protected:
    //죽음
    virtual CBTNode::NODE_STATE Check_Death();

    //플레이어 공격 체크
    virtual CBTNode::NODE_STATE Check_Sway();
    virtual CBTNode::NODE_STATE Sway();

    //플레이어에게 잡혀있는가?
    virtual CBTNode::NODE_STATE Chcek_Sync(); // 싱크 모션을 해야하는가?
    virtual CBTNode::NODE_STATE Sync_Neck(); // 싱크모션 : 멱살

    //데미지 DMD
    virtual CBTNode::NODE_STATE Check_Hit();
    virtual CBTNode::NODE_STATE HitAndGuard(); //히트모션을 할 것인지, 가드를 할 것인지

    // 플레이어 공격 스킬마다 히트모션 다름.
    virtual CBTNode::NODE_STATE Normal_Hit(); // 히트 체크를 하면서 죽었는지 죽지 않았는지도 같이 확인해야함.
    virtual CBTNode::NODE_STATE Strong_Hit();

    // 가드
    virtual CBTNode::NODE_STATE Guard();

    //분노
    virtual CBTNode::NODE_STATE Check_Angry(); // 화가 난 상태인지 체크
    virtual CBTNode::NODE_STATE Angry();

    virtual CBTNode::NODE_STATE ATK_Angry_Punch(); // 화가 난 상태일때 펀치
    virtual CBTNode::NODE_STATE ATK_Angry_Kick(); // 화가 난 상태일때 발차기

    //Break
    virtual CBTNode::NODE_STATE Check_Break(); // 쉬는 타임인가?
    virtual CBTNode::NODE_STATE ShiftAndIdle(); // 걸을 것인지 분기처리
    virtual CBTNode::NODE_STATE Shift(); // 쉬프트(걷기)
    virtual CBTNode::NODE_STATE Idle(); // 정좌

protected:
    virtual void Ready_Tree();
    _float DistanceFromPlayer();
    _bool isBehine();
    void LookAtPlayer();

public:
    virtual void Free();
};

END