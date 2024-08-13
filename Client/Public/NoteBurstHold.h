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
    CNoteBurstHold();
    virtual ~CNoteBurstHold() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void Tick() override;

private:
    _float m_fEndTime = { 0.f };
    _uint m_iGreatCount = { 6 };

public:
    static CNoteBurstHold* Create();
    virtual void Free() override;
};
END
