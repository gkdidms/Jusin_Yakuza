#include "Chapter5_0.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "Background.h"

CChapter5_0::CChapter5_0()
	: CMainQuest{}
{
}

HRESULT CChapter5_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//�� ī�޶� �̵� �� ��ũ��Ʈ �ؽ��� ����
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	return S_OK;
}

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
_bool CChapter5_0::Execute()
{
	if (m_pUIManager->isTalkFinished())
		return true;

	return false;
}

CChapter5_0* CChapter5_0::Create(void* pArg)
{
	CChapter5_0* pInstance = new CChapter5_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter5_0::Free()
{
	__super::Free();
}
