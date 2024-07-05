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
    //��ų ����Ʈ
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
    _float m_fAttackDelayTime = { 0.f }; // ������ ���� �� ���ӽð�

    _float m_fShiftDuration = { 0.f }; // �������� �ο��޴´�.
    _float m_fShiftTime = { 0.f };

private:
    void Ready_Tree();

private:
    //�г�
    CBTNode::NODE_STATE Check_Angry(); // ȭ�� �� �������� üũ
    CBTNode::NODE_STATE Angry();

    //����
    CBTNode::NODE_STATE Check_Attack(); // ���� ������ �������� üũ
    

    CBTNode::NODE_STATE Attack(); //���� ������ �����϶� ���� ����
    CBTNode::NODE_STATE Angry_Attack(); // ȭ�� �� �����϶� ���� ����

    CBTNode::NODE_STATE ATK_Punch();
    CBTNode::NODE_STATE ATK_CMD();
    CBTNode::NODE_STATE ATK_Angry_Punch(); // ȭ�� �� �����϶� ��ġ
    CBTNode::NODE_STATE ATK_Angry_Kick(); // ȭ�� �� �����϶� ������

    //Stand 
    CBTNode::NODE_STATE Check_Shift(); // �Ȱ� �ִ°�?
    //���� �� �ΰ�?
    CBTNode::NODE_STATE Shift(); // ���� ������ �б�ó��

    //Idle
    CBTNode::NODE_STATE Idle();

public:
    static CAI_RushYakuza* Create(void* pArg);
    virtual void Free() override;
};
END
