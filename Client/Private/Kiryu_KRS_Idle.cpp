#include "GameInstance.h"
#include "Kiryu_KRS_Idle.h"
#include "Player.h"

CKiryu_KRS_Idle::CKiryu_KRS_Idle()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	m_AnimationIndices.push_back(348);	//[348]	p_stand_btl_lp[p_stand_btl_lp]
	m_AnimationIndices.push_back(282);	//[282]	p_krs_stand_btl_en[p_krs_stand_btl_en]
	m_AnimationIndices.push_back(351);	//p_stand_nml 351
}

void CKiryu_KRS_Idle::Tick(const _float& fTimeDelta)
{
	Off_Battle(fTimeDelta);
}

void CKiryu_KRS_Idle::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_eCurrentBehavior]);
}

_bool CKiryu_KRS_Idle::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		m_eCurrentBehavior = IDLE;

		return true;
	}

	return false;
}

void CKiryu_KRS_Idle::Reset()
{
	m_eCurrentBehavior = BATTLE_ON;
}

void CKiryu_KRS_Idle::Off_Battle(const _float& fTimeDelta)
{
	if (BATTLE_ON != m_eCurrentBehavior) return;

	m_fOffBattleTimer += fTimeDelta;

	if (ANIM_CHANGE_TIME <= m_fOffBattleTimer)
	{
		m_fOffBattleTimer = { 0.f };

		m_eCurrentBehavior = BATTLE_OFF;
	}
}

CBehaviorAnimation* CKiryu_KRS_Idle::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Idle* pInstnace = new CKiryu_KRS_Idle();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_Adventure_Idle");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Idle::Free()
{
	__super::Free();
}