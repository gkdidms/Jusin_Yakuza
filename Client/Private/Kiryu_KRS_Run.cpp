#include "GameInstance.h"
#include "Kiryu_KRS_Run.h"
#include "Player.h"

CKiryu_KRS_Run::CKiryu_KRS_Run()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	//[308]	p_mov_run_btl[p_mov_run_btl]
	m_AnimationIndex.push_back(308);
}

void CKiryu_KRS_Run::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRS_Run::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndex[0]);
}

_bool CKiryu_KRS_Run::Get_AnimationEnd()
{
	//CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	//if (pModelCom->Get_AnimFinished())
	//{
	//	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	//	return true;
	//}

	return false;
}

void CKiryu_KRS_Run::Stop()
{
}

CBehaviorAnimation* CKiryu_KRS_Run::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Run* pInstnace = new CKiryu_KRS_Run();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_Adventure_Idle");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Run::Free()
{
	__super::Free();
}