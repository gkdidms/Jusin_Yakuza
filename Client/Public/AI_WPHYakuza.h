#pragma once
#include "AI_Monster.h"

BEGIN(Client)
class CAI_WPHYakuza :
    public CAI_Monster
{
private:
    CAI_WPHYakuza();
    CAI_WPHYakuza(const CAI_WPHYakuza& rhs);
    virtual ~CAI_WPHYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    virtual void Ready_Tree();

public:
    static CAI_WPHYakuza* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
