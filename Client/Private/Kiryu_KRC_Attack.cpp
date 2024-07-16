#include "GameInstance.h"
#include "Kiryu_KRC_Attack.h"
#include "Player.h"

CKiryu_KRC_Attack::CKiryu_KRC_Attack()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	
	/* 0 ~ 2 */
	m_AnimationIndices.push_back(158);	//[158]	p_krc_cmb_01[p_krc_cmb_01]
	m_AnimationIndices.push_back(160);	//[160]	p_krc_cmb_02[p_krc_cmb_02]
	m_AnimationIndices.push_back(163);	//[163]	p_krc_cmb_03[p_krc_cmb_03]

	m_AnimationIndices.push_back(159);	//[159]	p_krc_cmb_01_fin[p_krc_cmb_01_fin]
	m_AnimationIndices.push_back(161);	//[161]	p_krc_cmb_02_fin[p_krc_cmb_02_fin]
	m_AnimationIndices.push_back(164);	//[164]	p_krc_cmb_03_fin[p_krc_cmb_03_fin]

	/*
		[162]	p_krc_cmb_02_fin_hit[p_krc_cmb_02_fin_hit]
		[165]	p_krc_cmb_03_heavy[p_krc_cmb_03_heavy]
	*/
}

void CKiryu_KRC_Attack::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

	_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);

	//if (m_pGameInstance->GetKeyState(DIK_LSHIFT) == TAP)
	//{
	//	m_isFinished = true;
	//}
}

void CKiryu_KRC_Attack::Change_Animation()
{
	if (0 > m_iComboCount) return;

	m_pPlayer->Change_Animation(m_AnimationIndices[m_iComboCount]);
}

_bool CKiryu_KRC_Attack::Get_AnimationEnd()
{
	if (m_isFinished) return true;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		Reset();
		return true;
	}

	return false;
}

void CKiryu_KRC_Attack::Reset()
{
	m_iComboCount = 0;
}

void CKiryu_KRC_Attack::Combo_Count(_bool isFinAction)
{
	if (isFinAction)
	{
		if (m_iComboCount < 3)
			m_iComboCount += 3;
	}
	else
	{
		if (Changeable_Combo_Animation())
		{
			m_iComboCount++;

			if (m_iComboCount > 5)
				m_iComboCount = 0;
		}
	}
	
}		

_bool CKiryu_KRC_Attack::Changeable_Combo_Animation()
{
	_float fInterval = 0.4f;

	if (m_iComboCount > 1)			// 그 이상은 피니시 블로우기때문에 중간이 끊지않는다
		fInterval = 1.f;			// 1로 해두면 비율 연산 결과가 1이 넘을일이 없기때문에 아예 안끊김		

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (fInterval < *pModelCom->Get_AnimationCurrentPosition() / *pModelCom->Get_AnimationDuration())
		return true;

	return false;
}

CBehaviorAnimation* CKiryu_KRC_Attack::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Attack* pInstnace = new CKiryu_KRC_Attack();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Attack");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Attack::Free()
{
	__super::Free();
}