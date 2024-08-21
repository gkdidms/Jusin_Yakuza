#include "Chapter7_1.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "Background.h"

CChapter7_1::CChapter7_1()
	: CMainQuest{}
{
}

HRESULT CChapter7_1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//�� ī�޶� �̵� �� ��ũ��Ʈ �ؽ��� ����
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	return S_OK;
}

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
_bool CChapter7_1::Execute()
{
	if (m_pUIManager->isTalkFinished())
		return true;

	return false;
}

CChapter7_1* CChapter7_1::Create(void* pArg)
{
	CChapter7_1* pInstance = new CChapter7_1();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter7_1::Free()
{
	__super::Free();
}
