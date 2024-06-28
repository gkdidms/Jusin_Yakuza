#include "GameInstance.h"
#include "Kiryu_Adventure_Idle.h"
#include "Player.h"

CKiryu_Adventer_Idel::CKiryu_Adventer_Idel()
	:CBehaviorAnimation{}
{
	// 351: p_stand_nml �ִϸ��̼� �ε���
	m_AnimationIndex.push_back(351);
}

void CKiryu_Adventer_Idel::Change_Animation()
{
	// ���̵��� ����� �ϳ��� �д� (���� �߰�����)
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
