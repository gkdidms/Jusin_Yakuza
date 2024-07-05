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
    typedef struct tAIMonster {
        class CAnim* pAnim;
        _uint* pState;
    } AI_MONSTER_DESC;

protected:
    CAI_Monster();
    virtual ~CAI_Monster() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute() = 0;

protected:
    CBTNode* m_pRootNode = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };
    CAnim* m_pAnimCom = { nullptr };

    _uint* m_pState = { nullptr };

protected:
    _bool m_isAttack = { false };
    _bool m_isAngry = { false };
    _bool m_isSync = { false };
    _bool m_isShift = { false };

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

protected:
    virtual void Ready_Tree();

public:
    virtual void Free();
};

END