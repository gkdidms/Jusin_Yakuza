#include "NoteLong.h"
#include "GameInstance.h"

CNoteBurstHold::CNoteBurstHold()
    : CNoteBase()
{
}

HRESULT CNoteBurstHold::Initialize()
{
    return S_OK;
}


void CNoteBurstHold::Tick()
{
}

CNoteBurstHold* CNoteBurstHold::Create()
{
	CNoteBurstHold* pInstance = new CNoteBurstHold();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CNoteLong");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNoteBurstHold::Free()
{
    __super::Free();
}