#include "GameInstance.h"
#include "Kiryu_KRS_FlyKick.h"
#include "Player.h"

CKiryu_KRS_FlyKick::CKiryu_KRS_FlyKick()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(511);		//+[511]	p_krs_atk_run_heavy[p_krs_atk_run_heavy]
}

void CKiryu_KRS_FlyKick::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

	if (nullptr != pTargetObject)
	{
		_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
	}
}

void CKiryu_KRS_FlyKick::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRS_FlyKick::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		return true;
	}

	return false;
}

CBehaviorAnimation* CKiryu_KRS_FlyKick::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_FlyKick* pInstnace = new CKiryu_KRS_FlyKick();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_FlyKick");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_FlyKick::Free()
{
	__super::Free();
}