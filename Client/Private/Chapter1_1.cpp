#include "Chapter1_1.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "ScriptManager.h"
#include "Nishiki.h"

#include "Nishiki.h"
#include "PlayerCamera.h"
#include "Player.h"

CChapter1_1::CChapter1_1()
	: CMainQuest{}
{
}

HRESULT CChapter1_1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//씬 카메라 이동 후 스크립트 텍스쳐 나옴
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER))->Set_RotationBlock(true);
	m_pNishiki = dynamic_cast<CNishiki*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Nishiki"), 0));
	Safe_AddRef(m_pNishiki);

	m_pNishiki->Set_State(CNishiki::TALK);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
	pPlayer->Set_PlayerStop(true);
	//pPlayer->Get_TransformCom()->(true);

	return S_OK;
}

_bool CChapter1_1::Execute()
{
	if (m_pUIManager->isTalkFinished())
	{
		m_pNishiki->Set_State(CNishiki::IDLE);
		dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), 0))->Set_RotationBlock(false);
		dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0))->Set_PlayerStop(false);
		return true;
	}
		
	return false;
}

CChapter1_1* CChapter1_1::Create(void* pArg)
{
	CChapter1_1* pInstance = new CChapter1_1();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter1_1::Free()
{
	__super::Free();

	Safe_Release(m_pNishiki);
}
