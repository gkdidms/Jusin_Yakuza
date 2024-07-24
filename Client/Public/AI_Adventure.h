#pragma once
#include "BTNode.h"

#include "Client_Defines.h"

BEGIN(Client)
class CAI_Adventure :
    public CBTNode
{
private:
    CAI_Adventure();
    CAI_Adventure(const CAI_Adventure& rhs);
    virtual ~CAI_Adventure() = default;

public:
    static CAI_Adventure* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
