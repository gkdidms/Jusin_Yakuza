#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CTutorial :
    public CBase
{
public:
    typedef struct tTutorialDesc {
        _uint iConstance;
        string strPlayerSkillName;
    }TUTORIAL_DESC;

private:
    CTutorial();
    virtual ~CTutorial() = default;

public:
    HRESULT Intialize(void* pArg);
    _bool Execute();

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CPlayer* m_pPlayer = { nullptr };
    _uint m_iCoustance = { false };
    _uint m_iCount = { 0 };
    
    string m_strPlayerSkillName = { "" };

public:
    static CTutorial* Create(void* pArg);
    virtual void Free();
};
END
