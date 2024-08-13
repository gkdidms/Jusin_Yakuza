#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CNoteManager :
    public CBase
{
private:
    CNoteManager();
    virtual ~CNoteManager() = default;

public:
    HRESULT Initialize();
    void Tick();

private:
    HRESULT Load_NoteData(string strSongName);

private:
    CGameInstance* m_pGameInstance = { nullptr };

    vector<class CNoteBase*> m_Notes;

public:
    virtual void Free();
};
END
