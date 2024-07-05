#include "GameInstance.h"
#include "Kiryu_Adventure_Walk.h"
#include "Player.h"

CKiryu_Adventure_Walk::CKiryu_Adventure_Walk()
	:CBehaviorAnimation{}
{
	//	p_mov_walk_st 332
	//	p_mov_walk_fast 330
	//	p_mov_walk_en 328
	//	p_mov_walk_en_mir 329
	m_AnimationIndices.push_back(330);
	m_AnimationIndices.push_back(332);
	m_AnimationIndices.push_back(328);
	m_AnimationIndices.push_back(329);
}

void CKiryu_Adventure_Walk::Tick(const _float& fTimeDelta)
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

void CKiryu_Adventure_Walk::Change_Animation()
{
	const _bool* pDirction = m_pPlayer->Get_MoveDirection();

	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_Adventure_Walk::Get_AnimationEnd()
{
	if (m_eAnimState != ANIM_END) return false;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		m_iCurrentIndex = 0;
		m_eAnimState = ANIM_LOOP;
		return true;
	}

	return false;
}

void CKiryu_Adventure_Walk::Stop()
{
	if (m_eAnimState == ANIM_END) return; 
	m_eAnimState = ANIM_END;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (0.5 < (*pModelCom->Get_AnimationCurrentPosition() / *pModelCom->Get_AnimationDuration()))
		m_iCurrentIndex = 2;
	else
		m_iCurrentIndex = 3;
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
