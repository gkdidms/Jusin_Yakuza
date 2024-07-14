#include "GameInstance.h"
#include "Kiryu_KRH_Hit.h"
#include "Player.h"

CKiryu_KRH_Hit::CKiryu_KRH_Hit()
	:CBehaviorAnimation{}
{
	// 몸통 맞을 때 0 ~ 5
	m_AnimationIndices.push_back(0);		//[0] c_dam_body_lv01_b[c_dam_body_lv01_b]	0
	m_AnimationIndices.push_back(1);		//[1] c_dam_body_lv01_f[c_dam_body_lv01_f]	1
	m_AnimationIndices.push_back(2);		//[2] c_dam_body_lv02_b[c_dam_body_lv02_b]	2
	m_AnimationIndices.push_back(3);		//[3] c_dam_body_lv02_f[c_dam_body_lv02_f]	3
	m_AnimationIndices.push_back(4);		//[4] c_dam_body_lv02_l[c_dam_body_lv02_l]	4
	m_AnimationIndices.push_back(5);		//[5] c_dam_body_lv02_r[c_dam_body_lv02_r]	5

	// 머리 맞을 때 6 ~ 15
	m_AnimationIndices.push_back(16);		//[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
	m_AnimationIndices.push_back(17);		//[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
	m_AnimationIndices.push_back(18);		//[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
	m_AnimationIndices.push_back(19);		//[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
	m_AnimationIndices.push_back(20);		//[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]	10
	m_AnimationIndices.push_back(21);		//[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]	11	
	m_AnimationIndices.push_back(22);		//[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]	12
	m_AnimationIndices.push_back(23);		//[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]	13

	// 어퍼컷맞을때
	m_AnimationIndices.push_back(30);		//[30]	c_dam_push_head_b[c_dam_push_head_b]	14
	m_AnimationIndices.push_back(31);		//[31] c_dam_push_head_f[c_dam_push_head_f]		15

	// 다리맞을때 16 ~ 21
	m_AnimationIndices.push_back(24);		//[24]	c_dam_leg_lv01_b[c_dam_leg_lv01_b]		16
	m_AnimationIndices.push_back(25);		//[25]	c_dam_leg_lv01_f[c_dam_leg_lv01_f]		17
	m_AnimationIndices.push_back(26);		//[26]	c_dam_leg_lv01_l[c_dam_leg_lv01_l]		18
	m_AnimationIndices.push_back(27);		//[27]	c_dam_leg_lv01_r[c_dam_leg_lv01_r]		19
	m_AnimationIndices.push_back(28);		//[28]	c_dam_leg_lv02_l[c_dam_leg_lv02_l]		20
	m_AnimationIndices.push_back(29);		//[29]	c_dam_leg_lv02_r[c_dam_leg_lv02_r]		21

	// 넘어지는거 22 ~ 25
	m_AnimationIndices.push_back(33);		//[33]	c_dwn_body_b[c_dwn_body_b]				22
	m_AnimationIndices.push_back(34);		//[34]	c_dwn_body_f[c_dwn_body_f]				23
	m_AnimationIndices.push_back(35);		//[35]	c_dwn_body_l[c_dwn_body_l]				24
	m_AnimationIndices.push_back(36);		//[36]	c_dwn_body_r[c_dwn_body_r]				25

	// 가드깨지면서 넘어지는거 26 ~ 29
	m_AnimationIndices.push_back(37);		//[37]	c_dwn_break_guard_b[c_dwn_break_guard_b]	26
	m_AnimationIndices.push_back(38);		//[38]	c_dwn_break_guard_f[c_dwn_break_guard_f]	27
	m_AnimationIndices.push_back(39);		//[39]	c_dwn_break_guard_l[c_dwn_break_guard_l]	28
	m_AnimationIndices.push_back(40);		//[40]	c_dwn_break_guard_r[c_dwn_break_guard_r]	29

	// 엎어지는거 30 ~ 33
	m_AnimationIndices.push_back(41);		//[41]	c_dwn_direct_b[c_dwn_direct_b]			30
	m_AnimationIndices.push_back(42);		//[42]	c_dwn_direct_f[c_dwn_direct_f]			31
	m_AnimationIndices.push_back(43);		//[43]	c_dwn_direct_l[c_dwn_direct_l]			32
	m_AnimationIndices.push_back(44);		//[44]	c_dwn_direct_r[c_dwn_direct_r]			33

	// 다운 루프 34 ~ 35
	m_AnimationIndices.push_back(32);		//[32]	c_dwn_b[c_dwn_b]						34
	m_AnimationIndices.push_back(45);		//[45]	c_dwn_f[c_dwn_f]						35

	// 누운상태에서 차여서 굴러갈때 36 ~ 39
	m_AnimationIndices.push_back(8);		//[8]	c_dam_dnb_l[c_dam_dnb_l]				36
	m_AnimationIndices.push_back(9);		//[9]	c_dam_dnb_r[c_dam_dnb_r]				37
	m_AnimationIndices.push_back(10);		//[11]	c_dam_dnf_l[c_dam_dnf_l]				38
	m_AnimationIndices.push_back(11);		//[12]	c_dam_dnf_r[c_dam_dnf_r]				39

	// 누운 상태에서 밟힐 때 40 ~ 41
	m_AnimationIndices.push_back(10);		//[10]	c_dam_dnb_trample[c_dam_dnb_trample]	40
	m_AnimationIndices.push_back(13);		//[13]	c_dam_dnf_trample[c_dam_dnf_trample]	41

	// 일어나는 모션 42 ~ 43
	m_AnimationIndices.push_back(46);		//[46]	c_standup_dnb_fast[c_standup_dnb_fast]	42
	m_AnimationIndices.push_back(47);		//[47]	c_standup_dnf_fast[c_standup_dnf_fast]	43

	/*
		// 쓸지 안쓸지 모르겠음
		[6]	c_dam_break_attack[c_dam_break_attack]
		[7]	c_dam_break_guard[c_dam_break_guard]
		[14]	c_dam_fire_b[c_dam_fire_b]
		[15]	c_dam_fire_f[c_dam_fire_f]
	*/
}

void CKiryu_KRH_Hit::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRH_Hit::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRH_Hit::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		return true;
	}

	return false;
}

void CKiryu_KRH_Hit::Reset()
{
	m_iCurrentIndex = 0;
}

void CKiryu_KRH_Hit::Setting_Value(void* pValue)
{
	KRH_Hit_DESC* pDesc = static_cast<KRH_Hit_DESC*>(pValue);

	string strAnimName = m_pGameInstance->Extract_String(pDesc->strAnimationName, '[', ']');


	if ("p_krh_cmb_01" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}

	}
	else if ("p_krh_cmb_02" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("p_krh_cmb_03" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("p_krh_cmb_04" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("p_krh_cmb_05" == strAnimName)
	{
		/*
		[30]	c_dam_push_head_b[c_dam_push_head_b]	14
		[31] c_dam_push_head_f[c_dam_push_head_f]		15
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 14;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 14;
			break;
		}
		}
	}
	else if ("e_knk_atk_chop" == strAnimName)
	{
		// [30]	c_dam_push_head_b[c_dam_push_head_b]	14;
		m_iCurrentIndex = 14;
	}
	else if ("e_knk_atk_kick" == strAnimName)
	{
		// 러쉬 타입에서 이 스킬을 맞으면 넘어지게한다.
	}
	else if ("e_wpa_cmb_01" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_wpa_cmb_02" == strAnimName)
	{
		/*
		[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]	10
		[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]	11
		[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]	12
		[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]	13
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 11;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 10;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 12;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 13;
			break;
		}
		}
	}
	else if ("e_kta_atk_rariatto" == strAnimName)
	{
		/*
		[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]	10
		[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]	11
		[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]	12
		[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]	13
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 11;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 10;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 12;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 13;
			break;
		}
		}
	}
	else if ("e_kta_atk_gurad_run" == strAnimName)
	{
		// 넘어진다
	}
	else if ("e_kta_cmb_a_01" == strAnimName)
	{
		/*
		* 	[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]	10
			[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]	11
			[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]	12
			[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]	13
		*/

		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 11;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 10;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 12;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 13;
			break;
		}
		}
	}
	else if ("e_kta_cmb_a_02" == strAnimName)
	{
		/*
			[4] c_dam_body_lv02_l[c_dam_body_lv02_l]	4
			[5] c_dam_body_lv02_r[c_dam_body_lv02_r]	5
		*/

		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 5;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 5;
			break;
		}
		}
	}
	else if ("e_kta_cmb_a_03" == strAnimName)
	{
		/*
			[4] c_dam_body_lv02_l[c_dam_body_lv02_l]	4
			[5] c_dam_body_lv02_r[c_dam_body_lv02_r]	5
		*/

		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 5;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 5;
			break;
		}
		}
	}
	else if ("e_kuz_atk_down" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 7;
			break;
		}
		}
	}
	else if ("e_kuz_atk_jab" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_a_01" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_a_02" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_a_03" == strAnimName)
	{
		/*
		[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]	10
		[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]	11
		[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]	12
		[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]	13
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 11;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 10;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 12;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 13;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_b_01" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_b_02" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_b_03" == strAnimName)
	{
		/*
		[30]	c_dam_push_head_b[c_dam_push_head_b]	14
		[31] c_dam_push_head_f[c_dam_push_head_f]		15
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 14;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 14;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_headbutt_01" == strAnimName)
	{
		/*
		[30]	c_dam_push_head_b[c_dam_push_head_b]	14
		[31] c_dam_push_head_f[c_dam_push_head_f]		15
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 14;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 14;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_headbutt_02" == strAnimName)
	{
		/*
[30]	c_dam_push_head_b[c_dam_push_head_b]	14
[31] c_dam_push_head_f[c_dam_push_head_f]		15
*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 14;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 14;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_renda_01" == strAnimName)
	{
		/*
			[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
			[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
			[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
			[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_renda_02_l" == strAnimName)
	{
		/*
			[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
			[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
			[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
			[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_renda_02_l_fin" == strAnimName)
	{
		/*
			//[4] c_dam_body_lv02_l[c_dam_body_lv02_l]	4
			//[5] c_dam_body_lv02_r[c_dam_body_lv02_r]	5
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 5;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 5;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 4;
			break;
		}
		}
	}
	else if ("e_kuz_atk_heavy" == strAnimName)
	{
		// 하단(발로 밟고) 상단 상단(상체를 돌리면서 팔꿈치 휘두름) 때림
	}
	else if ("e_kuz_atk_hiji_2ren" == strAnimName)
	{
		/*
		[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]	10
		[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]	11
		[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]	12
		[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]	13
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 11;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 10;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 12;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 13;
			break;
		}
		}
	}


}

CBehaviorAnimation* CKiryu_KRH_Hit::Create(CPlayer* pPlayer)
{
	CKiryu_KRH_Hit* pInstnace = new CKiryu_KRH_Hit();

	if (nullptr == pInstnace)
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Hit");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRH_Hit::Free()
{
	__super::Free();
}