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

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
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
