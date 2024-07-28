#pragma once
#include "AI_Monster.h"

BEGIN(Client)
class CAI_DefaultYakuza :
    public CAI_Monster
{
private:
    CAI_DefaultYakuza();
    CAI_DefaultYakuza(const CAI_DefaultYakuza& rhs);
    virtual ~CAI_DefaultYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    virtual void Ready_Tree();

public:
    static CAI_DefaultYakuza* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
