#include "GameInstance.h"
#include "Kiryu_KRS_BattleStart.h"
#include "Player.h"

CKiryu_KRS_BattleStart::CKiryu_KRS_BattleStart()
	:CBehaviorAnimation{}
{
	// [284] p_krs_style_st[p_krs_style_st]
	m_AnimationIndex.push_back(284);
}

void CKiryu_KRS_BattleStart::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRS_BattleStart::Change_Animation()
{
	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.3f);
	m_pPlayer->Change_Animation(m_AnimationIndex[0]);
}

_bool CKiryu_KRS_BattleStart::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
		return true;
	}

	return false;
}

CBehaviorAnimation* CKiryu_KRS_BattleStart::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_BattleStart* pInstnace = new CKiryu_KRS_BattleStart();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_Adventure_Idle");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_BattleStart::Free()
{
	__super::Free();
}