#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CNoteBase abstract :
    public CBase
{
public:
    enum NOTE_SCORE
    {
        GREAT, GOOD, BAD, MISS, NONE
    };

protected:
    CNoteBase();
    virtual ~CNoteBase() = default;

public:
    virtual HRESULT Initialize() = 0;
    virtual void Tick() = 0;

protected:
    CGameInstance* m_pGameInstance = { nullptr };

    _uint   m_iNoteType = { 0 };
    _float m_fStartTime = { 0.f };

    NOTE_SCORE   m_eScore = { NONE };           //NONE은 아직 지나치지않음

public:
    virtual void Free();
};
END
