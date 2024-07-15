#include "GameInstance.h"
#include "Kiryu_KRS_Down.h"
#include "Player.h"

CKiryu_KRS_Down::CKiryu_KRS_Down()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(32); //[32]	c_dwn_b[c_dwn_b]
	m_AnimationIndices.push_back(45); //[45]	c_dwn_f[c_dwn_f]

	m_AnimationIndices.push_back(46); //[46]	c_standup_dnb_fast[c_standup_dnb_fast]
	m_AnimationIndices.push_back(47); //[47]	c_standup_dnf_fast[c_standup_dnf_fast]

	//// 다운 루프 34 ~ 35
	///*34*/m_AnimationIndices.push_back(32); m_AnimationNames.push_back("c_dwn_b");
	///*35*/m_AnimationIndices.push_back(45); m_AnimationNames.push_back("c_dwn_f");

	//// 누운상태에서 차여서 굴러갈때 36 ~ 39
	///*36*/m_AnimationIndices.push_back(8); m_AnimationNames.push_back("c_dam_dnb_l");
	///*37*/m_AnimationIndices.push_back(9); m_AnimationNames.push_back("c_dam_dnb_r");
	///*38*/m_AnimationIndices.push_back(10); m_AnimationNames.push_back("c_dam_dnf_l");
	///*39*/m_AnimationIndices.push_back(11); m_AnimationNames.push_back("c_dam_dnf_r");

	//// 누운 상태에서 밟힐 때 40 ~ 41
	///*40*/m_AnimationIndices.push_back(10); m_AnimationNames.push_back("c_dam_dnb_trample");
	///*41*/m_AnimationIndices.push_back(13); m_AnimationNames.push_back("c_dam_dnf_trample");

	//// 일어나는 모션 42 ~ 43
	///*42*/m_AnimationIndices.push_back(46); m_AnimationNames.push_back("c_standup_dnb_fast");
	///*43*/m_AnimationIndices.push_back(47); m_AnimationNames.push_back("c_standup_dnf_fast");

}

void CKiryu_KRS_Down::Tick(const _float& fTimeDelta)
{
	if (!m_isStop && (m_pGameInstance->GetKeyState(DIK_W) == HOLD || m_pGameInstance->GetKeyState(DIK_S) == HOLD
		|| m_pGameInstance->GetKeyState(DIK_A) == HOLD || m_pGameInstance->GetKeyState(DIK_D) == HOLD))
	{
		Stop();
	}

	switch (m_eAnimState)
	{
	case CKiryu_KRS_Down::ANIM_LOOP:
		if (m_isStop)
		{
			m_iCurrentIndex += 2;
			m_eAnimState = ANIM_END;
		}
		break;
	}
}

void CKiryu_KRS_Down::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRS_Down::Get_AnimationEnd()
{
	if (m_eAnimState != ANIM_END) return false;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		Reset();
		return true;
	}

	return false;
}

void CKiryu_KRS_Down::Stop()
{
	m_isStop = true;
}

void CKiryu_KRS_Down::Setting_Value(void* pValue)
{
	_bool* pDownState = static_cast<_bool*>(pValue);

	// 넘겨받은 값이 false면 뒤, true면 앞
	if (*pDownState)
		m_iCurrentIndex = 1;
	else
		m_iCurrentIndex = 0;
}

_bool CKiryu_KRS_Down::Stopping()
{
	if (ANIM_END == m_eAnimState && !Get_AnimationEnd()) return true;

	return false;
}

void CKiryu_KRS_Down::Reset()
{
	m_iCurrentIndex = 0;
	m_eAnimState = { ANIM_LOOP };
	m_isStop = false;
}

CBehaviorAnimation* CKiryu_KRS_Down::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Down* pInstnace = new CKiryu_KRS_Down();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Run");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Down::Free()
{
	__super::Free();
}