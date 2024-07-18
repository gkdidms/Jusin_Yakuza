#pragma once
#include "AI_Monster.h"

BEGIN(Client)
class CAI_Kuze :
    public CAI_Monster
{
private:
    CAI_Kuze();
    CAI_Kuze(const CAI_Kuze& rhs);
    virtual ~CAI_Kuze() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    void Ready_Tree();

private:
    CBTNode::NODE_STATE Check_PlayerDown();
    CBTNode::NODE_STATE ATK_Down();

    CBTNode::NODE_STATE Check_Attack(); // ���� ������ �������� üũ
    CBTNode::NODE_STATE Attack(); //���� �����Ҷ� ���� ����
    CBTNode::NODE_STATE ATK_Heavy();
    CBTNode::NODE_STATE ATK_Hiji_2Ren();
    CBTNode::NODE_STATE ATK_Jab();
    CBTNode::NODE_STATE ATK_CMD_A();
    CBTNode::NODE_STATE ATK_CMD_B();
    CBTNode::NODE_STATE ATK_HeadButt();
    CBTNode::NODE_STATE ATK_Renda();

    CBTNode::NODE_STATE Check_Distance();
    CBTNode::NODE_STATE Run();


public:
    static CAI_Kuze* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free() override;
};
END
