#include "GameInstance.h"
#include "Kiryu_KRS_Walk.h"
#include "Player.h"

CKiryu_KRS_Walk::CKiryu_KRS_Walk()
	:CBehaviorAnimation{}
{
	// ���� �� ���̵� ����� ��庥ó�� �����ϴ�
	//p_stand_nml 351
	m_AnimationIndices.push_back(351);
}

void CKiryu_KRS_Walk::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRS_Walk::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRS_Walk::Get_AnimationEnd()
{
	//CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	//if (pModelCom->Get_AnimFinished())
	//{
	//	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	//	return true;
	//}

	return false;
}

CBehaviorAnimation* CKiryu_KRS_Walk::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Walk* pInstnace = new CKiryu_KRS_Walk();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Walk");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Walk::Free()
{
	__super::Free();
}