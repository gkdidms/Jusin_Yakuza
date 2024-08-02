#include "Tutorial.h"

#include "GameInstance.h"

#include "Player.h"

CTutorial::CTutorial()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTutorial::Intialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	TUTORIAL_DESC* pDesc = static_cast<TUTORIAL_DESC*>(pArg);
	m_iCoustance = pDesc->iConstance;
	m_strPlayerSkillName = pDesc->strPlayerSkillName;

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
	if (nullptr == m_pPlayer)
		return E_FAIL;

	return S_OK;
}

_bool CTutorial::Start()
{
	

	return false;
}

_bool CTutorial::Execute()
{
	if (string_view(m_pPlayer->Get_CurrentAnimationName()) == string_view(m_strPlayerSkillName))
	{
		if (m_pPlayer->isTutorialAttack())
			++m_iCount;
	}

	if (m_iCoustance <= m_iCount)
		return true;

	return false;
}

CTutorial* CTutorial::Create(void* pArg)
{
	CTutorial* pInstance = new CTutorial();

	if (FAILED(pInstance->Intialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTutorial::Free()
{
	Safe_Release(m_pGameInstance);
}
