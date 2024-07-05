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
    //죽음
    virtual CBTNode::NODE_STATE Check_Death();

    //플레이어 공격 체크
    virtual CBTNode::NODE_STATE Check_Sway();
    virtual CBTNode::NODE_STATE Sway();

    //플레이어에게 잡혀있는가?
    virtual CBTNode::NODE_STATE Chcek_Sync(); // 싱크 모션을 해야하는가?
    virtual CBTNode::NODE_STATE Sync_Neck(); // 싱크모션 : 멱살

    //데미지 DMD
    virtual CBTNode::NODE_STATE Check_Hit();
    virtual CBTNode::NODE_STATE HitAndGuard(); //히트모션을 할 것인지, 가드를 할 것인지

    // 플레이어 공격 스킬마다 히트모션 다름.
    virtual CBTNode::NODE_STATE Normal_Hit(); // 히트 체크를 하면서 죽었는지 죽지 않았는지도 같이 확인해야함.
    virtual CBTNode::NODE_STATE Strong_Hit();

    // 가드
    virtual CBTNode::NODE_STATE Guard();

protected:
    virtual void Ready_Tree();

public:
    virtual void Free();
};

END