#include "NoteBase.h"
#include "GameInstance.h"

CNoteBase::CNoteBase()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CNoteBase::Initialize()
{
    return S_OK;
}


void CNoteBase::Tick()
{
}

void CNoteBase::Free()
{
    Safe_Release(m_pGameInstance);
}