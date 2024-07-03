#include "GameInstance.h"
#include "Kiryu_KRS_Gaurd.h"
#include "Player.h"

CKiryu_KRS_Gaurd::CKiryu_KRS_Gaurd()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	//p_stand_nml 351
	m_AnimationIndices.push_back(351);
}

void CKiryu_KRS_Gaurd::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRS_Gaurd::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRS_Gaurd::Get_AnimationEnd()
{
	//CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	//if (pModelCom->Get_AnimFinished())
	//{
	//	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	//	return true;
	//}

	return false;
}

CBehaviorAnimation* CKiryu_KRS_Gaurd::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Gaurd* pInstnace = new CKiryu_KRS_Gaurd();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_Adventure_Idle");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Gaurd::Free()
{
	__super::Free();
}