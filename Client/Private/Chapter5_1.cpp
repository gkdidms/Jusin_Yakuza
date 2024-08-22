#include "Chapter5_1.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "Background.h"

CChapter5_1::CChapter5_1()
	: CMainQuest{}
{
}

HRESULT CChapter5_1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//�� ī�޶� �̵� �� ��ũ��Ʈ �ؽ��� ����
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	Player_Stop(true);
	PlayerCom_Stop(true);

	return S_OK;
}

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
_bool CChapter5_1::Execute()
{
	if (m_pUIManager->isTalkFinished())
	{
		Player_Stop(false);
		PlayerCom_Stop(false);
		return true;
	}
		
	return false;
}

CChapter5_1* CChapter5_1::Create(void* pArg)
{
	CChapter5_1* pInstance = new CChapter5_1();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter5_1::Free()
{
	__super::Free();
}
