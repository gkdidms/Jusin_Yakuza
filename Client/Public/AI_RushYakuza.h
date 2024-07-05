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




private:
    void Ready_Tree();

private:
    //공격
    CBTNode::NODE_STATE Check_Attack(); // 공격 가능한 상태인지 체크
    
    CBTNode::NODE_STATE Attack(); //공격 가능한 상태일때 공격 선택
    CBTNode::NODE_STATE Angry_Attack(); // 화가 난 상태일때 공격 선택

    CBTNode::NODE_STATE ATK_Punch();
    CBTNode::NODE_STATE ATK_CMD();

public:
    static CAI_RushYakuza* Create(void* pArg);
    virtual void Free() override;
};
END
