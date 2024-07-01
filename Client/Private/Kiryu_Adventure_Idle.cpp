#include "GameInstance.h"
#include "Kiryu_Adventure_Idle.h"
#include "Player.h"

CKiryu_Adventure_Idle::CKiryu_Adventure_Idle()
	:CBehaviorAnimation{}
{
	//p_stand_nml 351
	//p_stand_idle_lookaround 350
	m_AnimationIndex.push_back(350);
	m_AnimationIndex.push_back(351);
}

void CKiryu_Adventure_Idle::Tick(const _float& fTimeDelta)
{
}

void CKiryu_Adventure_Idle::Change_Animation()
{
	// 아이들은 모션을 하나만 둔다 (추후 추가예정)
	m_pPlayer->Change_Animation(m_AnimationIndex[0]);
}

CBehaviorAnimation* CKiryu_Adventure_Idle::Create(CPlayer* pPlayer)
{
	CKiryu_Adventure_Idle* pInstnace = new CKiryu_Adventure_Idle();

	if (nullptr == pInstnace)
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_Adventure_Idle");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_Adventure_Idle::Free()
{
	__super::Free();
}
