#include "GameInstance.h"
#include "Kiryu_KRH_Sway.h"
#include "Player.h"

CKiryu_KRH_Sway::CKiryu_KRH_Sway()
	:CBehaviorAnimation{}
{
	// �����̴� �׳� Ű�Է��� ������ back ������ �⺻��
	// ī�޶� ���� wasd�� �Է¹޾� �������ϴ��� �� �������� �����̸� �ϴ°� �´µ�, 
	// ��µǴ� �ִϸ��̼��� ĳ������ Look�� �������� ���´�.
	m_AnimationIndices.push_back(287);	//[287]	p_krs_sway_f[p_krs_sway_f]
	m_AnimationIndices.push_back(286);	//[286]	p_krs_sway_b[p_krs_sway_b]
	m_AnimationIndices.push_back(288);	//[288]	p_krs_sway_l[p_krs_sway_l]
	m_AnimationIndices.push_back(289);	//[289]	p_krs_sway_r[p_krs_sway_r]


	/*
	* +[196]	p_krh_sway_02_b[p_krh_sway_02_b]
		+[197]	p_krh_sway_02_f[p_krh_sway_02_f]
		+[198]	p_krh_sway_02_l[p_krh_sway_02_l]
		+[199]	p_krh_sway_02_r[p_krh_sway_02_r]
		+[200]	p_krh_sway_03_b[p_krh_sway_03_b]
		+[201]	p_krh_sway_03_f[p_krh_sway_03_f]
		+[202]	p_krh_sway_03_l[p_krh_sway_03_l]
		+[203]	p_krh_sway_03_r[p_krh_sway_03_r]
		+[204]	p_krh_sway_b[p_krh_sway_b]
		+[205]	p_krh_sway_f[p_krh_sway_f]
		+[206]	p_krh_sway_l[p_krh_sway_l]
		+[207]	p_krh_sway_r[p_krh_sway_r]
	
	*/
}

void CKiryu_KRH_Sway::Tick(const _float& fTimeDelta)
{
	const _bool* pDirection = m_pPlayer->Get_MoveDirection();

	if (pDirection[CPlayer::F])
		m_iCurrentIndex = CPlayer::F;
	else if (pDirection[CPlayer::L])
		m_iCurrentIndex = CPlayer::L;
	else if (pDirection[CPlayer::R])
		m_iCurrentIndex = CPlayer::R;
	else
		m_iCurrentIndex = CPlayer::B;
}

void CKiryu_KRH_Sway::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRH_Sway::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		m_pPlayer->Reset_MoveDirection();
		return true;
	}
	return false;
}

CBehaviorAnimation* CKiryu_KRH_Sway::Create(CPlayer* pPlayer)
{
	CKiryu_KRH_Sway* pInstnace = new CKiryu_KRH_Sway();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Sway");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRH_Sway::Free()
{
	__super::Free();
}