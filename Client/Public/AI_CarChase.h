#pragma once
#include "BTNode.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CAnim;
class CGameInstance;
END

BEGIN(Client)
class CAI_CarChase abstract:
    public CBTNode
{
public:
    typedef struct tAICarChaseDesc {
        CAnim* pAnim;

        class CCarChase_Monster* pThis;
    } AI_CARCHASE_DESC;

    enum SKILL {
        SKILL_END
    };

private:
    CAI_CarChase();
    CAI_CarChase(const CAI_CarChase& rhs);
    virtual ~CAI_CarChase() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute() = 0;

    _bool Find_PlayerCurrentAnimationName(string strAnimName);

protected:
    virtual void Ready_Tree() = 0;
    _float DistanceFromPlayer();
    _bool isBehine();
    void LookAtPlayer();

protected:
    CGameInstance* m_pGameInstance = { nullptr };
    CAnim* m_pAnimCom = { nullptr };
    class CPlayer* m_pPlayer = { nullptr };
    class CCarChase_Monster* m_pThis = { nullptr };

    CBTNode* m_pRoot = { nullptr };

protected:
    _uint m_iSkill = { SKILL_END };

private:
    CBTNode::NODE_STATE Check_Down();
    CBTNode::NODE_STATE Down();

    CBTNode::NODE_STATE Check_Hit();
    CBTNode::NODE_STATE Hit();

    CBTNode::NODE_STATE Check_Sync();
    CBTNode::NODE_STATE Sync();

    CBTNode::NODE_STATE Check_Attck();
    

public:
    virtual void Free();
};
END
