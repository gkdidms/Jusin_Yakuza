#pragma once
#include "BTNode.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CAnim;
class CGameInstance;
END

BEGIN(Client)
class CAI_RushYakuza :
    public CBTNode
{
public:
    //스킬 리스트
    enum SKILL {
        SKILL_HIT,
        SKILL_SWAY,
        SKILL_SHIFT,
        SKILL_CMD,
        SKILL_PUNCH,
        SKILL_ANGRY_CHOP,
        SKILL_ANGRY_KICK,
        SKILL_END
    };

    typedef struct tAIOfficeYakuzaDest {
        class CRushYakuza* pYakuza; // 자기 자신의 포인터
        class CModel* pModel;
        class CAnim* pAnim;

        _uint* pState;
    } AI_OFFICE_YAKUZA_DESC;

private:
    CAI_RushYakuza();
    virtual ~CAI_RushYakuza() = default;

public:
    HRESULT Initialize(void* pArg);
    void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    CBTNode* m_pRootNode = { nullptr };

    CGameInstance* m_pGameInstance = { nullptr };
    CModel* m_pModelCom = { nullptr };
    CAnim* m_pAnimCom = { nullptr };

    class CRushYakuza* m_pThis = { nullptr };
    _uint* m_pState = { nullptr };

    _uint m_iSkill = { SKILL_END };

private:
    _bool m_isAttack = { false };
    _bool m_isAngry = { false };
    _bool m_isSync = { false };
    _bool m_isShift = { false };

    _float m_fDelayAttackDuration = { 10.f }; 
    _float m_fAttackDelayTime = { 0.f }; // 공격이 끝난 후 지속시간

    _float m_fShiftDuration = { 0.f }; // 랜덤으로 부여받는다.
    _float m_fShiftTime = { 0.f };

private:
    void Ready_Tree();

private:
    //죽음
    CBTNode::NODE_STATE Check_Death();

    //플레이어 공격 체크
    CBTNode::NODE_STATE Check_Sway();
    CBTNode::NODE_STATE Sway();

    //플레이어에게 잡혀있는가?
    CBTNode::NODE_STATE Chcek_Sync(); // 싱크 모션을 해야하는가?
    CBTNode::NODE_STATE Sync_Neck(); // 싱크모션 : 멱살

    //데미지 DMD
    CBTNode::NODE_STATE Check_Hit(); 
    CBTNode::NODE_STATE HitAndGuard(); //히트모션을 할 것인지, 가드를 할 것인지

    // 플레이어 공격 스킬마다 히트모션 다름.
    CBTNode::NODE_STATE Normal_Hit(); // 히트 체크를 하면서 죽었는지 죽지 않았는지도 같이 확인해야함.
    CBTNode::NODE_STATE Strong_Hit();
    
    // 가드
    CBTNode::NODE_STATE Guard();

    //분노
    CBTNode::NODE_STATE Check_Angry(); // 화가 난 상태인지 체크
    CBTNode::NODE_STATE Angry();

    //공격
    CBTNode::NODE_STATE Check_Attack(); // 공격 가능한 상태인지 체크
    

    CBTNode::NODE_STATE Attack(); //공격 가능한 상태일때 공격 선택
    CBTNode::NODE_STATE Angry_Attack(); // 화가 난 상태일때 공격 선택

    CBTNode::NODE_STATE ATK_Punch();
    CBTNode::NODE_STATE ATK_CMD();
    CBTNode::NODE_STATE ATK_Angry_Punch(); // 화가 난 상태일때 펀치
    CBTNode::NODE_STATE ATK_Angry_Kick(); // 화가 난 상태일때 발차기

    //Stand 
    CBTNode::NODE_STATE Check_Shift(); // 걷고 있는가?
    //걸을 것 인가?
    CBTNode::NODE_STATE Shift(); // 걸을 것인지 분기처리

    //Idle
    CBTNode::NODE_STATE Idle();

public:
    static CAI_RushYakuza* Create(void* pArg);
    virtual void Free() override;
};
END
