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
    //��ų ����Ʈ
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
        class CRushYakuza* pYakuza; // �ڱ� �ڽ��� ������
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
    _float m_fAttackDelayTime = { 0.f }; // ������ ���� �� ���ӽð�

    _float m_fShiftDuration = { 0.f }; // �������� �ο��޴´�.
    _float m_fShiftTime = { 0.f };

private:
    void Ready_Tree();

private:
    //����
    CBTNode::NODE_STATE Check_Death();

    //�÷��̾� ���� üũ
    CBTNode::NODE_STATE Check_Sway();
    CBTNode::NODE_STATE Sway();

    //�÷��̾�� �����ִ°�?
    CBTNode::NODE_STATE Chcek_Sync(); // ��ũ ����� �ؾ��ϴ°�?
    CBTNode::NODE_STATE Sync_Neck(); // ��ũ��� : ���

    //������ DMD
    CBTNode::NODE_STATE Check_Hit(); 
    CBTNode::NODE_STATE HitAndGuard(); //��Ʈ����� �� ������, ���带 �� ������

    // �÷��̾� ���� ��ų���� ��Ʈ��� �ٸ�.
    CBTNode::NODE_STATE Normal_Hit(); // ��Ʈ üũ�� �ϸ鼭 �׾����� ���� �ʾҴ����� ���� Ȯ���ؾ���.
    CBTNode::NODE_STATE Strong_Hit();
    
    // ����
    CBTNode::NODE_STATE Guard();

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
