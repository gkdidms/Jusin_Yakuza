#include "GameInstance.h"
#include "Kiryu_Adventure_Run.h"
#include "Player.h"
#include "Model.h"

CKiryu_Adventure_Run::CKiryu_Adventure_Run()
	:CBehaviorAnimation{}
{
	// 시작 애니메이션들
	m_AnimationIndex.push_back(319);	//	p_move_run_start_l180 319 / 0
	m_AnimationIndex.push_back(320);	//	p_move_run_start_l90 320 / 1
	m_AnimationIndex.push_back(321);	//	p_move_run_start_r180 321 / 2
	m_AnimationIndex.push_back(322);	//	p_move_run_start_r90 322 / 3

	// 달리는 루프 애니메이션들
	m_AnimationIndex.push_back(314); 	//	p_mov_run_kyoro 314	/ 4
	m_AnimationIndex.push_back(315); 	//	p_mov_run_l 315 / 5
	m_AnimationIndex.push_back(316); 	//	p_mov_run_r 316 / 6

	//p_mov_run_dash 310
	//p_mov_run_dash_tired 311
	//p_mov_run_dash_stop_l 312
	//p_mov_run_dash_stop_s 313

	// 끝날 때 애니메이션
	m_AnimationIndex.push_back(323);	//	p_mov_run_stop 323 / 7

	m_iCurrentIndex = 0;
}

void CKiryu_Adventure_Run::Tick(const _float& fTimeDelta)
{
	switch (m_eAnimState)
	{
	case CKiryu_Adventure_Run::ANIM_START:
	{
		CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

		if (pModelCom->Get_AnimFinished())
			m_eAnimState = ANIM_LOOP;
	}
		break;
	case CKiryu_Adventure_Run::ANIM_LOOP:
		if(m_isStop)
			m_eAnimState = ANIM_END;
		break;
	}
}

void CKiryu_Adventure_Run::Change_Animation()
{
	const _bool* pDirction = m_pPlayer->Get_MoveDirection();

	if (m_eAnimState == ANIM_START)
	{
		//// 정면은 달리기 시작하는 모션이 없다!
		//if (pDirction[CPlayer::F])
		//{
		//	m_eAnimState = ANIM_LOOP;
		//	m_iCurrentIndex = 4;
		//}
		//if (pDirction[CPlayer::B])
		//	m_iCurrentIndex = 0;
		//if (pDirction[CPlayer::L])
		//	m_iCurrentIndex = 1;
		//if (pDirction[CPlayer::R])
		//	m_iCurrentIndex = 3;

		m_iCurrentIndex = 4;

	}
	else if (m_eAnimState == ANIM_LOOP)
	{
		//if ((pDirction[CPlayer::B] || pDirction[CPlayer::F]) && pDirction[CPlayer::L])
		//	m_iCurrentIndex = 5;
		//else if ((pDirction[CPlayer::B] || pDirction[CPlayer::F]) && pDirction[CPlayer::R])
		//	m_iCurrentIndex = 6;
		//else
			m_iCurrentIndex = 4;
	}
	else if (m_eAnimState == ANIM_END)
	{
		m_iCurrentIndex = 7;
	}
		
	m_pPlayer->Change_Animation(m_AnimationIndex[m_iCurrentIndex]);
}

_bool CKiryu_Adventure_Run::Get_AnimationEnd()
{
	if (m_eAnimState != ANIM_END) return false;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		m_eAnimState = { ANIM_START };
		m_isStop = { false };

		return true;
	}

	return false;
}

void CKiryu_Adventure_Run::Stop()
{
	m_eAnimState = ANIM_END;
}


CBehaviorAnimation* CKiryu_Adventure_Run::Create(CPlayer* pPlayer)
{
	CKiryu_Adventure_Run* pInstnace = new CKiryu_Adventure_Run();

	if (nullptr == pInstnace)
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_Adventure_Run");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_Adventure_Run::Free()
{
	__super::Free();
}
