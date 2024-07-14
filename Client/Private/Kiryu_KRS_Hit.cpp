#include "GameInstance.h"
#include "Kiryu_KRS_Hit.h"
#include "Player.h"

CKiryu_KRS_Hit::CKiryu_KRS_Hit()
	:CBehaviorAnimation{}
{
	// 몸통 맞을 때
	m_AnimationIndices.push_back(0);		//[0] c_dam_body_lv01_b[c_dam_body_lv01_b]
	m_AnimationIndices.push_back(1);		//[1] c_dam_body_lv01_f[c_dam_body_lv01_f]
	m_AnimationIndices.push_back(2);		//[2] c_dam_body_lv02_b[c_dam_body_lv02_b]
	m_AnimationIndices.push_back(3);		//[3] c_dam_body_lv02_f[c_dam_body_lv02_f]
	m_AnimationIndices.push_back(4);		//[4] c_dam_body_lv02_l[c_dam_body_lv02_l]
	m_AnimationIndices.push_back(5);		//[5] c_dam_body_lv02_r[c_dam_body_lv02_r]

	// 머리 맞을 때
	m_AnimationIndices.push_back(16);		//[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]
	m_AnimationIndices.push_back(17);		//[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]
	m_AnimationIndices.push_back(18);		//[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]
	m_AnimationIndices.push_back(19);		//[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]
	m_AnimationIndices.push_back(20);		//[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]
	m_AnimationIndices.push_back(21);		//[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]
	m_AnimationIndices.push_back(22);		//[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]
	m_AnimationIndices.push_back(23);		//[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]
	m_AnimationIndices.push_back(30);		//[30]	c_dam_push_head_b[c_dam_push_head_b]
	m_AnimationIndices.push_back(31);		//[31] c_dam_push_head_f[c_dam_push_head_f]

	// 다리맞을때
	m_AnimationIndices.push_back(24);		//[24]	c_dam_leg_lv01_b[c_dam_leg_lv01_b]
	m_AnimationIndices.push_back(25);		//[25]	c_dam_leg_lv01_f[c_dam_leg_lv01_f]
	m_AnimationIndices.push_back(26);		//[26]	c_dam_leg_lv01_l[c_dam_leg_lv01_l]
	m_AnimationIndices.push_back(27);		//[27]	c_dam_leg_lv01_r[c_dam_leg_lv01_r]
	m_AnimationIndices.push_back(28);		//[28]	c_dam_leg_lv02_l[c_dam_leg_lv02_l]
	m_AnimationIndices.push_back(29);		//[29]	c_dam_leg_lv02_r[c_dam_leg_lv02_r]
	
	// 넘어지는거
	m_AnimationIndices.push_back(33);		//[33]	c_dwn_body_b[c_dwn_body_b]
	m_AnimationIndices.push_back(34);		//[34]	c_dwn_body_f[c_dwn_body_f]
	m_AnimationIndices.push_back(35);		//[35]	c_dwn_body_l[c_dwn_body_l]
	m_AnimationIndices.push_back(36);		//[36]	c_dwn_body_r[c_dwn_body_r]

	// 가드깨지면서 넘어지는거
	m_AnimationIndices.push_back(37);		//[37]	c_dwn_break_guard_b[c_dwn_break_guard_b]
	m_AnimationIndices.push_back(38);		//[38]	c_dwn_break_guard_f[c_dwn_break_guard_f]
	m_AnimationIndices.push_back(39);		//[39]	c_dwn_break_guard_l[c_dwn_break_guard_l]
	m_AnimationIndices.push_back(40);		//[40]	c_dwn_break_guard_r[c_dwn_break_guard_r]
	
	// 엎어지는거
	m_AnimationIndices.push_back(41);		//[41]	c_dwn_direct_b[c_dwn_direct_b]
	m_AnimationIndices.push_back(42);		//[42]	c_dwn_direct_f[c_dwn_direct_f]
	m_AnimationIndices.push_back(43);		//[43]	c_dwn_direct_l[c_dwn_direct_l]
	m_AnimationIndices.push_back(44);		//[44]	c_dwn_direct_r[c_dwn_direct_r]

	// 다운 루프
	m_AnimationIndices.push_back(32);		//[32]	c_dwn_b[c_dwn_b]
	m_AnimationIndices.push_back(45);		//[45]	c_dwn_f[c_dwn_f]
	
	// 누운상태에서 차여서 굴러갈때
	m_AnimationIndices.push_back(8);		//[8]	c_dam_dnb_l[c_dam_dnb_l]
	m_AnimationIndices.push_back(9);		//[9]	c_dam_dnb_r[c_dam_dnb_r]
	m_AnimationIndices.push_back(10);		//[11]	c_dam_dnf_l[c_dam_dnf_l]
	m_AnimationIndices.push_back(11);		//[12]	c_dam_dnf_r[c_dam_dnf_r]

	// 누운 상태에서 밟힐 때
	m_AnimationIndices.push_back(10);		//[10]	c_dam_dnb_trample[c_dam_dnb_trample]
	m_AnimationIndices.push_back(13);		//[13]	c_dam_dnf_trample[c_dam_dnf_trample]

	// 일어나는 모션
	m_AnimationIndices.push_back(46);		//[46]	c_standup_dnb_fast[c_standup_dnb_fast]
	m_AnimationIndices.push_back(47);		//[47]	c_standup_dnf_fast[c_standup_dnf_fast]

	/*
		// 쓸지 안쓸지 모르겠음
		[6]	c_dam_break_attack[c_dam_break_attack]
		[7]	c_dam_break_guard[c_dam_break_guard]
		[14]	c_dam_fire_b[c_dam_fire_b]
		[15]	c_dam_fire_f[c_dam_fire_f]
	*/
}

void CKiryu_KRS_Hit::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRS_Hit::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRS_Hit::Get_AnimationEnd()
{
	//CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	//if (pModelCom->Get_AnimFinished())
	//{
	//	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	//	return true;
	//}

	return false;
}

void CKiryu_KRS_Hit::Reset()
{
}

void CKiryu_KRS_Hit::Setting_Value(void* pValue)
{
	KRS_Hit_DESC* pDesc = static_cast<KRS_Hit_DESC*>(pValue);

	string strAnimName = pDesc->strAnimationName;

	
}

CBehaviorAnimation* CKiryu_KRS_Hit::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Hit* pInstnace = new CKiryu_KRS_Hit();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Hit");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Hit::Free()
{
	__super::Free();
}