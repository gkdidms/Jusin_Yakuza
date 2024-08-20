#include "Chapter3_0.h"

#include "GameInstance.h"
#include "UIManager.h"

#include "Nishiki.h"

CChapter3_0::CChapter3_0()
	: CMainQuest{}
{
}

HRESULT CChapter3_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//�� ī�޶� �̵� �� ��ũ��Ʈ �ؽ��� ����
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	m_pNishiki = dynamic_cast<CNishiki*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Nishiki"), 0));
	Safe_AddRef(m_pNishiki);

	m_pNishiki->Set_State(CNishiki::TALK);

	return S_OK;
}

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
_bool CChapter3_0::Execute()
{
	if (m_pUIManager->isTalkFinished())
	{
		m_pNishiki->Set_State(CNishiki::IDLE);
		return true;
	}

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

	Safe_Release(m_pNishiki);
}
