#pragma once
#include "AI_Monster.h"

BEGIN(Client)
class CAI_Yoneda :
    public CAI_Monster
{
private:
    CAI_Yoneda();
    CAI_Yoneda(const CAI_Yoneda& rhs);
    virtual ~CAI_Yoneda() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    void Ready_Tree();

private:
    

public:
    static CAI_Yoneda* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free() override;
};
END
