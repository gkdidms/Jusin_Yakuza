#include "GameInstance.h"
#include "Kiryu_KRC_Guard.h"
#include "Kiryu_KRC_Hit.h"
#include "Player.h"

CKiryu_KRC_Guard::CKiryu_KRC_Guard()
	:CBehaviorAnimation{}
{
	/*
	*	// 피격
		[166]	p_krc_guard_body_b[p_krc_guard_body_b]
		[167]	p_krc_guard_body_f[p_krc_guard_body_f]
		[169]	p_krc_guard_head_l[p_krc_guard_head_l]
		[170]	p_krc_guard_head_r[p_krc_guard_head_r]

		// 빠른 가드 (스킬임)
		[172]	p_krc_guard_run_en[p_krc_guard_run_en]
		[173]	p_krc_guard_run_lp[p_krc_guard_run_lp]
		[174]	p_krc_guard_run_st[p_krc_guard_run_st]

		// 기본 모션
		[168]	p_krc_guard_en[p_krc_guard_en]
		[171]	p_krc_guard_lp[p_krc_guard_lp]
		[175]	p_krc_guard_st[p_krc_guard_st]
	*/

	/* 0 ~ 3 */
	m_AnimationIndices.push_back(175); // [175]	p_krc_guard_st[p_krc_guard_st]
	m_AnimationIndices.push_back(171); // [171]	p_krc_guard_lp[p_krc_guard_lp]
	m_AnimationIndices.push_back(168); // [168]	p_krc_guard_en[p_krc_guard_en]

	/* 4 ~ 7 */
	m_AnimationIndices.push_back(166);	//[166]	p_krc_guard_body_b[p_krc_guard_body_b]
	m_AnimationIndices.push_back(167);	//[167]	p_krc_guard_body_f[p_krc_guard_body_f]
	m_AnimationIndices.push_back(169);	//[169]	p_krc_guard_head_l[p_krc_guard_head_l]
	m_AnimationIndices.push_back(170);	//[170]	p_krc_guard_head_r[p_krc_guard_head_r]
}

void CKiryu_KRC_Guard::Tick(const _float& fTimeDelta)
{
	switch (m_eAnimState)
	{
	case CKiryu_KRC_Guard::ANIM_START:
		if (Get_AnimationEnd())
		{
			m_eAnimState = ANIM_LOOP;
		}
		break;
	case CKiryu_KRC_Guard::ANIM_LOOP:
		if (m_isStop)
			m_eAnimState = ANIM_END;
		break;
	}

}

void CKiryu_KRC_Guard::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_eAnimState + m_iCurrentIndex]);
}

_bool CKiryu_KRC_Guard::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		//m_eAnimState = { ANIM_START };
		//m_isStop = false;

		return true;
	}

	return false;
}

void CKiryu_KRC_Guard::Stop()
{
	m_isStop = true;
}

// 유일하게 Stop했는지 용도로 사용할 것이다.
_bool CKiryu_KRC_Guard::Stopping()
{
	if (ANIM_END == m_eAnimState && Get_AnimationEnd()) return true;

	return false;
}

void CKiryu_KRC_Guard::Reset()
{
	m_eAnimState = ANIM_START;
	m_iCurrentIndex = 0;
	m_isStop = false;
}

void CKiryu_KRC_Guard::Setting_Value(void* pValue)
{
	CKiryu_KRC_Hit::KRC_Hit_DESC* pDesc = static_cast<CKiryu_KRC_Hit::KRC_Hit_DESC*>(pValue);

	// m_eAnimState 에 합해서 쓸려고 그냥 2 뺀 값으로 세팅해줌 
	switch (pDesc->iDirection)
	{
	case CPlayer::F:
	{
		m_iCurrentIndex = 3;
		break;
	}
	case CPlayer::B:
	{
		m_iCurrentIndex = 2;
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

CBehaviorAnimation* CKiryu_KRC_Guard::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Guard* pInstnace = new CKiryu_KRC_Guard();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Gaurd");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Guard::Free()
{
	__super::Free();
}