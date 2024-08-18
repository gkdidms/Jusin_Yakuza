#include "Chapter4_0.h"

#include "GameInstance.h"

#include "Background.h"

CChapter4_0::CChapter4_0()
	: CMainQuest{}
{
}

HRESULT CChapter4_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

//대화가 끝나면 자동으로 다음 스테이지로 넘어감
_bool CChapter4_0::Execute()
{
	return false;
}

CChapter4_0* CChapter4_0::Create(void* pArg)
{
	CChapter4_0* pInstance = new CChapter4_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter4_0::Free()
{
	__super::Free();
}
