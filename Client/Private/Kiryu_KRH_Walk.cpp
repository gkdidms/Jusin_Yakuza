#include "GameInstance.h"
#include "Kiryu_KRH_Walk.h"
#include "Player.h"

CKiryu_KRH_Walk::CKiryu_KRH_Walk()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	//p_stand_nml 351
	m_AnimationIndices.push_back(351);
}

void CKiryu_KRH_Walk::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRH_Walk::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRH_Walk::Get_AnimationEnd()
{
	//CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	//if (pModelCom->Get_AnimFinished())
	//{
	//	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	//	return true;
	//}

	return false;
}

CBehaviorAnimation* CKiryu_KRH_Walk::Create(CPlayer* pPlayer)
{
	CKiryu_KRH_Walk* pInstnace = new CKiryu_KRH_Walk();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Walk");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRH_Walk::Free()
{
	__super::Free();
}