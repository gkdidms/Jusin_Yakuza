#include "GameInstance.h"
#include "Kiryu_KRS_Walk.h"
#include "Player.h"

CKiryu_KRS_Walk::CKiryu_KRS_Walk()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(376);			//+[376]	p_krs_shift_f[p_krs_shift_f]

	/*
		+[373]	p_krs_shift_b[p_krs_shift_b]
		+[374]	p_krs_shift_bl[p_krs_shift_bl]
		+[375]	p_krs_shift_br[p_krs_shift_br]
		+[376]	p_krs_shift_f[p_krs_shift_f]
		+[377]	p_krs_shift_fl[p_krs_shift_fl]
		+[378]	p_krs_shift_fr[p_krs_shift_fr]
		+[379]	p_krs_shift_l[p_krs_shift_l]
		+[380]	p_krs_shift_r[p_krs_shift_r]
	*/
}

void CKiryu_KRS_Walk::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRS_Walk::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRS_Walk::Get_AnimationEnd()
{
	//CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	//if (pModelCom->Get_AnimFinished())
	//{
	//	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	//	return true;
	//}

	return false;
}

CBehaviorAnimation* CKiryu_KRS_Walk::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Walk* pInstnace = new CKiryu_KRS_Walk();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Walk");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Walk::Free()
{
	__super::Free();
}