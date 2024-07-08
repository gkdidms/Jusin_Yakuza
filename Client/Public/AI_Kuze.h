#pragma once
#include "AI_Monster.h"

BEGIN(Client)
class CAI_Kuze :
    public CAI_Monster
{
private:
    CAI_Kuze();
    CAI_Kuze(const CAI_Kuze& rhs);
    virtual ~CAI_Kuze() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    void Ready_Tree();

public:
    static CAI_Kuze* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free() override;
};
END
