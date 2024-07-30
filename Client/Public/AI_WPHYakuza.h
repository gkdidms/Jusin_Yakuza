#pragma once
#include "AI_Monster.h"

/*
* ������������ ���ڵ�� �ִ� ����
*/

BEGIN(Client)
class CAI_WPHYakuza :
    public CAI_Monster
{
private:
    CAI_WPHYakuza();
    CAI_WPHYakuza(const CAI_WPHYakuza& rhs);
    virtual ~CAI_WPHYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    _float m_fHitDuration = { 0.3f };
    _float m_fHitTime = { 0.f };

    _bool m_isHit = { false };

private:
    virtual void Ready_Tree();

private:
    //���� ��ӹ޾Ƽ� ����
    virtual CBTNode::NODE_STATE Check_Down() override;
    virtual CBTNode::NODE_STATE Dead() override;

    virtual CBTNode::NODE_STATE Check_Hit() override;
    CBTNode::NODE_STATE Attack(); //���� ������ �����϶� ���� ����

    CBTNode::NODE_STATE ATK_Heavy();

    CBTNode::NODE_STATE Idle() override;

public:
    static CAI_WPHYakuza* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
