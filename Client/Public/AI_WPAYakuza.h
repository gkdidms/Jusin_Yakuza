#pragma once
#include "AI_Monster.h"

BEGIN(Client)
class CAI_WPAYakuza :
    public CAI_Monster
{
private:
    CAI_WPAYakuza();
    CAI_WPAYakuza(const CAI_WPAYakuza& rhs);
    virtual ~CAI_WPAYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    virtual void Ready_Tree();

private:
    virtual CBTNode::NODE_STATE HitAndGuard() override;
    //���� ��ӹ޾Ƽ� ����
    CBTNode::NODE_STATE Attack(); //���� ������ �����϶� ���� ����
    CBTNode::NODE_STATE Angry_Attack(); // ȭ�� �� �����϶� ���� ����

    CBTNode::NODE_STATE ATK_Heavy();
    CBTNode::NODE_STATE ATK_CMD();

public:
    static CAI_WPAYakuza* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free() override; 
};
END
