#include "Chapter6_1.h"

#include "GameInstance.h"
#include "FileTotalMgr.h"
#include "UIManager.h"
#include "Background.h"

#include "Player.h"
#include "Shakedown.h"

CChapter6_1::CChapter6_1()
	: CMainQuest{}
{
}

HRESULT CChapter6_1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Player_Stop(true);
	PlayerCom_Stop(true);

	m_pShakedown = dynamic_cast<CShakedown*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"), 0));
	Safe_AddRef(m_pShakedown);
	m_pShakedown->Set_Script(true);
	m_pShakedown->Set_Animation("e_kta_stand", true, false);

	m_pFileTotalMgr->Setting_Start_Cinemachine(63);

	return S_OK;
}

//대화가 끝나면 자동으로 다음 스테이지로 넘어감
_bool CChapter6_1::Execute()
{
	m_fTotalTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));
	m_pUIManager->Set_AlwayUI(false);

	if (m_fDuration <= m_fTotalTime)
	{
		m_pShakedown->Set_Script(false);
		Player_Stop(false);
		PlayerCom_Stop(false);
		
		return true;
	}

	return false;
}

CChapter6_1* CChapter6_1::Create(void* pArg)
{
	CChapter6_1* pInstance = new CChapter6_1();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter6_1::Free()
{
	__super::Free();

	Safe_Release(m_pShakedown);
}
