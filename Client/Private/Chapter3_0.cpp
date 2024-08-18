#include "Chapter3_0.h"

#include "GameInstance.h"

#include "Background.h"

CChapter3_0::CChapter3_0()
	: CMainQuest{}
{
}

HRESULT CChapter3_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

//대화가 끝나면 자동으로 다음 스테이지로 넘어감
_bool CChapter3_0::Execute()
{
	return false;
}

CChapter3_0* CChapter3_0::Create(void* pArg)
{
	CChapter3_0* pInstance = new CChapter3_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter3_0::Free()
{
	__super::Free();
}
