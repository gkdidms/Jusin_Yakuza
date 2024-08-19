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
    CNoteLong(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime);
    virtual ~CNoteLong() = default;

public:
    _float Get_EndTime() {
        return m_fEndTime;
    };

private:
    _float m_fEndTime = { 0.f };

public:
    static CNoteLong* Create(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime);
    virtual void Free() override;
};
END
