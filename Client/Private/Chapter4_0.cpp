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

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
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
