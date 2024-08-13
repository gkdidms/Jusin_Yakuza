#pragma once
#include "NoteBase.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CNoteBurstHold :
    public CNoteBase
{
protected:
    CNoteBurstHold(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime);
    virtual ~CNoteBurstHold() = default;

private:
    _float m_fEndTime = { 0.f };
    _uint m_iGreatCount = { 6 };

public:
    static CNoteBurstHold* Create(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime);
    virtual void Free() override;
};
END
