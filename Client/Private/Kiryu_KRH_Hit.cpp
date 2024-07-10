#include "GameInstance.h"
#include "Kiryu_KRH_Hit.h"
#include "Player.h"

CKiryu_KRH_Hit::CKiryu_KRH_Hit()
	:CBehaviorAnimation{}
{
	// ���� �� ���̵� ����� ��庥ó�� �����ϴ�
	//p_stand_nml 351
	m_AnimationIndices.push_back(351);
}

void CKiryu_KRH_Hit::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRH_Hit::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRH_Hit::Get_AnimationEnd()
{
	//CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	//if (pModelCom->Get_AnimFinished())
	//{
	//	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	//	return true;
	//}

	return false;
}

CBehaviorAnimation* CKiryu_KRH_Hit::Create(CPlayer* pPlayer)
{
	CKiryu_KRH_Hit* pInstnace = new CKiryu_KRH_Hit();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Hit");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRH_Hit::Free()
{
	__super::Free();
}