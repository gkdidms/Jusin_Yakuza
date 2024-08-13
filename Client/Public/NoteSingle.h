#pragma once
#include "NoteBase.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CNoteSingle :
    public CNoteBase
{
protected:
    CNoteSingle();
    virtual ~CNoteSingle() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void Tick() override;

public:
    static CNoteSingle* Create();
    virtual void Free() override;
};
END
