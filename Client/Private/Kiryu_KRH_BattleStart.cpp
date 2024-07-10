#include "GameInstance.h"
#include "Kiryu_KRH_BattleStart.h"
#include "Player.h"

CKiryu_KRH_BattleStart::CKiryu_KRH_BattleStart()
	:CBehaviorAnimation{}
{
	// [284] p_krs_style_st[p_krs_style_st]
	m_AnimationIndices.push_back(284);
}

void CKiryu_KRH_BattleStart::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRH_BattleStart::Change_Animation()
{
	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.3f);
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRH_BattleStart::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
		return true;
	}

	return false;
}

CBehaviorAnimation* CKiryu_KRH_BattleStart::Create(CPlayer* pPlayer)
{
	CKiryu_KRH_BattleStart* pInstnace = new CKiryu_KRH_BattleStart();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_BattleStart");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRH_BattleStart::Free()
{
	__super::Free();
}