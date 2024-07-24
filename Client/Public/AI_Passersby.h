#pragma once
#include "AI_Adventure.h"

BEGIN(Client)
class CAI_Passersby :
    public CAI_Adventure
{
private:
    CAI_Passersby();
    CAI_Passersby(const CAI_Passersby& rhs);
    virtual ~CAI_Passersby() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    virtual void Ready_Root();

public:
    static CAI_Passersby* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
