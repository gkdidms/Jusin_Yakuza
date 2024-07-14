#include "GameInstance.h"
#include "Kiryu_KRC_Hit.h"
#include "Player.h"

CKiryu_KRC_Hit::CKiryu_KRC_Hit()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	//p_stand_nml 351
	m_AnimationIndices.push_back(351);
}

void CKiryu_KRC_Hit::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRC_Hit::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRC_Hit::Get_AnimationEnd()
{
	//CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	//if (pModelCom->Get_AnimFinished())
	//{
	//	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	//	return true;
	//}

	return false;
}

CBehaviorAnimation* CKiryu_KRC_Hit::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Hit* pInstnace = new CKiryu_KRC_Hit();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Hit");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Hit::Free()
{
	__super::Free();
}