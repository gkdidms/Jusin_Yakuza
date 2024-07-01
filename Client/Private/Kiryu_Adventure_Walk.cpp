#include "GameInstance.h"
#include "Kiryu_Adventure_Walk.h"
#include "Player.h"

CKiryu_Adventure_Walk::CKiryu_Adventure_Walk()
	:CBehaviorAnimation{}
{
	// p_mov_jog 308
	//	p_mov_walk_fast 330
	// p_mov_walk_s 331 / 천천히 걷기 시작할 때 쓴다(패드로 하는 게임이다보니 키입력의 강도가 있어서 있는듯)
	//	p_mov_walk_st 332
	//	p_mov_walk_en 328
	//	p_mov_walk_en_mir 329
	m_AnimationIndex.push_back(308);
	m_AnimationIndex.push_back(330);
	m_AnimationIndex.push_back(331);
	m_AnimationIndex.push_back(332);
	m_AnimationIndex.push_back(328);
	m_AnimationIndex.push_back(329);
}

void CKiryu_Adventure_Walk::Tick(const _float& fTimeDelta)
{
	
}

void CKiryu_Adventure_Walk::Change_Animation()
{
	const _bool* pDirction = m_pPlayer->Get_MoveDirection();

	if(pDirction[CPlayer::F])
		m_pPlayer->Change_Animation(m_AnimationIndex[0]);
}

CBehaviorAnimation* CKiryu_Adventure_Walk::Create(CPlayer* pPlayer)
{
	CKiryu_Adventure_Walk* pInstnace = new CKiryu_Adventure_Walk();

	if (nullptr == pInstnace)
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_Adventure_Walk");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_Adventure_Walk::Free()
{
	__super::Free();
}
