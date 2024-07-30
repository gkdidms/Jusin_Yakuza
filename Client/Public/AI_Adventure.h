#pragma once
#include "BTNode.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CAnim;
class CAStart;
END

BEGIN(Client)
class CAI_Adventure abstract :
    public CBTNode
{
public:
    typedef struct tAIAdventureDesc {
        CAnim* pAnimCom;
        CAStart* pAStartCom;
        class CAdventure* pThis;

        _uint* pState;
    } AI_ADVENTURE_DESC;

    enum SKILL {
        SKILL_IDLE,
        SKILL_WALK,
        SKILL_TURN,
        SKILL_END
    };

protected:
    CAI_Adventure();
    CAI_Adventure(const CAI_Adventure& rhs);
    virtual ~CAI_Adventure() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute() = 0;

protected:
    CBTNode* m_pRootNode = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };
    CAnim* m_pAnimCom = { nullptr };
    CAStart* m_pAStartCom = { nullptr };

    class CPlayer* m_pPlayer = { nullptr }; // �÷��̾��� �ּҸ� ������ �ֵ��� �Ѵ�.
    class CAdventure* m_pThis = { nullptr }; // �ڱ� �ڽ��� �ּҸ� ������ �ֵ��� �Ѵ�.

    _uint* m_pState = { nullptr };

protected:
    _uint m_iSkill = { SKILL_END };
    _bool m_isWayPointFinish = { true };

    vector<ROUTE_IO> m_Routes = {};

protected:
    virtual void Ready_Root() = 0;
    _bool isRouteMoveFinish();

protected:
    //�浹 ����
    virtual CBTNode::NODE_STATE Check_Coll();
    virtual CBTNode::NODE_STATE Coll();
    //�ȱ�
    virtual CBTNode::NODE_STATE Check_Walk();
    virtual CBTNode::NODE_STATE Turn();
    virtual CBTNode::NODE_STATE Walk(); // ������ ��Ʈ�� �ȴ´�.

public:
    virtual void Free();
};
END