#include "Chapter2_0.h"

#include "GameInstance.h"

#include "UIManager.h"
#include "ScriptManager.h"
#include "UITutorial.h"

#include "Background.h"

CChapter2_0::CChapter2_0()
	: CMainQuest{}
{
}

HRESULT CChapter2_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//�� ī�޶� �̵� �� ��ũ��Ʈ �ؽ��� ����
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex); // 0��° ��ȭ

	return S_OK;
}

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
_bool CChapter2_0::Execute()
{
	if (m_pUIManager->isTalkFinished())
		return true;

	return false;
}

CChapter2_0* CChapter2_0::Create(void* pArg)
{
	CChapter2_0* pInstance = new CChapter2_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter2_0::Free()
{
	__super::Free();
}
