#include "GameInstance.h"
#include "Kiryu_Adventure_Idle.h"
#include "Player.h"

CKiryu_Adventer_Idel::CKiryu_Adventer_Idel()
	:CBehaviorAnimation{}
{
	// 351: p_stand_nml 애니메이션 인덱스
	m_AnimationIndex.push_back(351);
}

void CKiryu_Adventer_Idel::Change_Animation()
{
	// 아이들은 모션을 하나만 둔다 (추후 추가예정)
	m_pPlayer->Change_Animation(m_AnimationIndex[0]);
}

CBehaviorAnimation* CKiryu_Adventer_Idel::Create(CPlayer* pPlayer)
{
	CKiryu_Adventer_Idel* pInstnace = new CKiryu_Adventer_Idel();

	if (nullptr == pInstnace)
	{
		Safe_Release(pInstnace);
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_Adventer_Idel::Free()
{
	__super::Free();
}
