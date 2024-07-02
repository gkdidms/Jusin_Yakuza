#include "GameInstance.h"
#include "Kiryu_KRS_Attack.h"
#include "Player.h"

CKiryu_KRS_Attack::CKiryu_KRS_Attack()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	//p_atk_down_trample 351 (검사때 보여드릴려고 일부러 0번만 넣음)
	m_AnimationIndex.push_back(0);
}

void CKiryu_KRS_Attack::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRS_Attack::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndex[0]);
}

_bool CKiryu_KRS_Attack::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		return true;
	}

	return false;
}

CBehaviorAnimation* CKiryu_KRS_Attack::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Attack* pInstnace = new CKiryu_KRS_Attack();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_Adventure_Idle");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Attack::Free()
{
	__super::Free();
}