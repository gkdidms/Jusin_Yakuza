#include "Chapter7_0.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "Background.h"

CChapter7_0::CChapter7_0()
	: CMainQuest{}
{
}

HRESULT CChapter7_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//�� ī�޶� �̵� �� ��ũ��Ʈ �ؽ��� ����
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	return S_OK;
}

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
_bool CChapter7_0::Execute()
{
	if (m_pUIManager->isTalkFinished())
		return true;

	return false;
}

CChapter7_0* CChapter7_0::Create(void* pArg)
{
	CChapter7_0* pInstance = new CChapter7_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter7_0::Free()
{
	__super::Free();
}
