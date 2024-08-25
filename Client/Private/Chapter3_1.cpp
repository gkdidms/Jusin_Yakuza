#include "Chapter3_1.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "FileTotalMgr.h"

#include "Player.h"
#include "Nishiki.h"


CChapter3_1::CChapter3_1()
	: CMainQuest{}
{
}

HRESULT CChapter3_1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//�� ī�޶� �̵� �� ��ũ��Ʈ �ؽ��� ����
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	return S_OK;
}

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
_bool CChapter3_1::Execute()
{
	if (m_pUIManager->isTalkFinished())
		return true;

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
