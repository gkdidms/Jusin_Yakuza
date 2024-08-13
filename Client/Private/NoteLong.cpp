#include "NoteLong.h"
#include "GameInstance.h"

CNoteLong::CNoteLong()
    : CNoteBase()
{
}

HRESULT CNoteLong::Initialize()
{
    return S_OK;
}


void CNoteLong::Tick()
{
}

CNoteLong* CNoteLong::Create()
{
	CNoteLong* pInstance = new CNoteLong();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CNoteLong");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNoteLong::Free()
{
    __super::Free();
}