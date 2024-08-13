#include "NoteSingle.h"
#include "GameInstance.h"

CNoteSingle::CNoteSingle()
    : CNoteBase()
{
}

HRESULT CNoteSingle::Initialize()
{
    return S_OK;
}


void CNoteSingle::Tick()
{
}

CNoteSingle* CNoteSingle::Create()
{
	CNoteSingle* pInstance = new CNoteSingle();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CNoteSingle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNoteSingle::Free()
{
    __super::Free();
}