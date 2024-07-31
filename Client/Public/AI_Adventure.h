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

    class CPlayer* m_pPlayer = { nullptr }; // 플레이어의 주소를 가지고 있도록 한다.
    class CAdventure* m_pThis = { nullptr }; // 자기 자신의 주소를 가지고 있도록 한다.

    _uint* m_pState = { nullptr };

protected:
    _uint m_iSkill = { SKILL_END };
    _bool m_isWayPointFinish = { true };

    vector<ROUTE_IO> m_Routes = {};

protected:
    virtual void Ready_Root() = 0;
    _bool isRouteMoveFinish();

protected:
    //충돌 감지
    virtual CBTNode::NODE_STATE Check_Coll();
    virtual CBTNode::NODE_STATE Coll();
    //걷기
    virtual CBTNode::NODE_STATE Check_Walk();
    virtual CBTNode::NODE_STATE Turn();
    virtual CBTNode::NODE_STATE Walk(); // 정해진 루트로 걷는다.

public:
    virtual void Free();
};
END