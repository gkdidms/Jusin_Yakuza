#pragma once
#include "AI_Monster.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CAI_RushYakuza :
    public CAI_Monster
{
public:
    //스킬 리스트
    enum SKILL{
        SKILL_HIT,
        SKILL_SWAY,
        SKILL_SHIFT,
        SKILL_CMD,
        SKILL_PUNCH,
        SKILL_ANGRY_CHOP,
        SKILL_ANGRY_KICK,
        SKILL_END
    };

    typedef struct tAIOfficeYakuzaDest : public CAI_Monster::AI_MONSTER_DESC {
        class CModel* pModel;
        
    } AI_OFFICE_YAKUZA_DESC;

private:
    CAI_RushYakuza();
    virtual ~CAI_RushYakuza() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    CModel* m_pModelCom = { nullptr };
    _uint m_iSkill = { SKILL_END };

private:
    _float m_fDelayAttackDuration = { 10.f }; 
    _float m_fAttackDelayTime = { 0.f }; // 공격이 끝난 후 지속시간

    _float m_fShiftDuration = { 0.f }; // 랜덤으로 부여받는다.
    _float m_fShiftTime = { 0.f };

private:
    void Ready_Tree();

private:
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
