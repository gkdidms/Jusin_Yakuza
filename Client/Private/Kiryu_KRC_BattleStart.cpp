#include "GameInstance.h"
#include "Kiryu_KRC_BattleStart.h"
#include "Player.h"

CKiryu_KRC_BattleStart::CKiryu_KRC_BattleStart()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(194);	// [194]	p_krc_style_st[p_krc_style_st]
	m_AnimationIndices.push_back(192);	// [192]	p_krc_stand_btl_lp[p_krc_stand_btl_lp]
	//m_AnimationIndices.push_back(308);	//[193]	p_krc_stand_btl_st[p_krc_stand_btl_st]
	m_AnimationIndices.push_back(191);	// [191]	p_krc_stand_btl_en[p_krc_stand_btl_en]
}

void CKiryu_KRC_BattleStart::Tick(const _float& fTimeDelta)
{
	Off_Battle(fTimeDelta);
}

void CKiryu_KRC_BattleStart::Change_Animation()
{
	if(ANIM_START == m_eAnimState)
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.3f);
	m_pPlayer->Change_Animation(m_AnimationIndices[m_eAnimState]);
}

_bool CKiryu_KRC_BattleStart::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		if (ANIM_START == m_eAnimState)
		{
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
			m_eAnimState = ANIM_LOOP;
		}
		else if (ANIM_END == m_eAnimState)
			return true;
	}

	return false;
}

void CKiryu_KRC_BattleStart::Reset()
{
	m_eAnimState = ANIM_START;
	m_fOffBattleTimer = 0.f;
}

void CKiryu_KRC_BattleStart::Off_Battle(const _float& fTimeDelta)
{
	if (ANIM_LOOP != m_eAnimState) return;

	m_fOffBattleTimer += fTimeDelta;

	if (ANIM_CHANGE_TIME <= m_fOffBattleTimer)
	{
		m_fOffBattleTimer = { 0.f };

		m_eAnimState = ANIM_END;
	}
}

CBehaviorAnimation* CKiryu_KRC_BattleStart::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_BattleStart* pInstnace = new CKiryu_KRC_BattleStart();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_BattleStart");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_BattleStart::Free()
{
	__super::Free();
}