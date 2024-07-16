#include "GameInstance.h"
#include "Kiryu_KRS_Walk.h"
#include "Player.h"

CKiryu_KRS_Walk::CKiryu_KRS_Walk()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(373);			//[373]	p_krs_shift_b[p_krs_shift_b]
	m_AnimationIndices.push_back(374);			//[374]	p_krs_shift_bl[p_krs_shift_bl]
	m_AnimationIndices.push_back(375);			//[375]	p_krs_shift_br[p_krs_shift_br]
	m_AnimationIndices.push_back(376);			//[376]	p_krs_shift_f[p_krs_shift_f]
	m_AnimationIndices.push_back(377);			//[377]	p_krs_shift_fl[p_krs_shift_fl]
	m_AnimationIndices.push_back(378);			//[378]	p_krs_shift_fr[p_krs_shift_fr]
	m_AnimationIndices.push_back(379);			//[379]	p_krs_shift_l[p_krs_shift_l]
	m_AnimationIndices.push_back(380);			//[380] p_krs_shift_r[p_krs_shift_r]

}

void CKiryu_KRS_Walk::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

	if (nullptr != pTargetObject)
	{
		_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
	}

	// F, B, L, R
	_bool isKeyInput[4];
	for (size_t i = 0; i < 4; i++)
		isKeyInput[i] = false;

	if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
	{
		isKeyInput[CPlayer::F] = true;
	}
	if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
	{
		isKeyInput[CPlayer::B] = true;
	}
	if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
	{
		isKeyInput[CPlayer::L] = true;
	}
	if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
	{
		isKeyInput[CPlayer::R] = true;
	}
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