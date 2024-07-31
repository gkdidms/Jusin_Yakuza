#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Client)
class CTutorialManager :
    public CBase
{
private:
    CTutorialManager();
    virtual ~CTutorialManager() = default;

public:
    HRESULT Initialize();
    void Tick();

private:
    vector<class CTutorial*> m_Tutorials = {};
    _uint m_iCurrentIndex = { 0 };

private:
    HRESULT Ready_Tutorial();

   
public:
    static CTutorialManager* Create();
    virtual void Free();
};
END
