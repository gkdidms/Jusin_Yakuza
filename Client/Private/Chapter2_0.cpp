#include "Chapter2_0.h"

#include "GameInstance.h"

#include "Subtitle.h"

#include "Background.h"

CChapter2_0::CChapter2_0()
	: CMainQuest{}
{
}

HRESULT CChapter2_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CSubtitle::SUBTITLE_DESC Desc{};
	Desc.iScriptChapter = m_iScriptIndex;

	if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Subtitle"), TEXT("Layer_Subtitle"), &Desc)))
		return E_FAIL;

	m_pSubtitle = dynamic_cast<CSubtitle*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Subtitle"), 0));
	Safe_AddRef(m_pSubtitle);

	return S_OK;
}

//��ȭ�� ������ �ڵ����� ���� ���������� �Ѿ
_bool CChapter2_0::Execute()
{
	if (m_pSubtitle->isFinished())
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
	Safe_Release(m_pSubtitle);
}
