#pragma once
#include "NoteBase.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CNoteLong :
    public CNoteBase
{
protected:
    CNoteLong();
    virtual ~CNoteLong() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void Tick() override;

private:
    _float m_fEndTime = { 0.f };

public:
    static CNoteLong* Create();
    virtual void Free() override;
};
END
