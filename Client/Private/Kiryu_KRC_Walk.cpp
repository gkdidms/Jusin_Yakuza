#include "GameInstance.h"
#include "Kiryu_KRC_Walk.h"
#include "Player.h"

CKiryu_KRC_Walk::CKiryu_KRC_Walk()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	m_AnimationIndices.push_back(186);		//[186]	p_krc_shift_f[p_krc_shift_f]

	/*
	*	+[184]	p_krc_shift_b[p_krc_shift_b]
		+[185]	p_krc_shift_br[p_krc_shift_br]
		+[186]	p_krc_shift_f[p_krc_shift_f]
		+[187]	p_krc_shift_fl[p_krc_shift_fl]
		+[188]	p_krc_shift_fr[p_krc_shift_fr]
		+[189]	p_krc_shift_l[p_krc_shift_l]
		+[190]	p_krc_shift_r[p_krc_shift_r]
	*/
}

void CKiryu_KRC_Walk::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRC_Walk::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRC_Walk::Get_AnimationEnd()
{
	//CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	//if (pModelCom->Get_AnimFinished())
	//{
	//	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	//	return true;
	//}

	return false;
}

CBehaviorAnimation* CKiryu_KRC_Walk::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Walk* pInstnace = new CKiryu_KRC_Walk();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Walk");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Walk::Free()
{
	__super::Free();
}