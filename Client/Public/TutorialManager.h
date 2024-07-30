#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Client)
class CTutorialManager :
    public CBase
{
    DECLARE_SINGLETON(CTutorialManager)
private:
    CTutorialManager();
    virtual ~CTutorialManager() = default;

public:
    HRESULT Initialize();

public:
    virtual void Free();
};
END
