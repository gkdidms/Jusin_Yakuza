#include "Chapter3_1.h"

#include "GameInstance.h"

#include "Background.h"

CChapter3_1::CChapter3_1()
	: CMainQuest{}
{
}

HRESULT CChapter3_1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
_bool CChapter3_1::Execute()
{
	return false;
}

CChapter3_1* CChapter3_1::Create(void* pArg)
{
	CChapter3_1* pInstance = new CChapter3_1();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter3_1::Free()
{
	__super::Free();
}
