#include "GameInstance.h"
#include "Kiryu_KRS_Attack.h"
#include "Player.h"
#include "Monster.h"

CKiryu_KRS_Attack::CKiryu_KRS_Attack()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	
	/* 0 ~ 3 */
	m_AnimationIndices.push_back(344);	//[344]	p_krs_cmb_01[p_krs_cmb_01]
	m_AnimationIndices.push_back(353);	//[353]	p_krs_cmb_02[p_krs_cmb_02]
	m_AnimationIndices.push_back(358);	//[358]	p_krs_cmb_03[p_krs_cmb_03]
	m_AnimationIndices.push_back(362);	//[362]	p_krs_cmb_04[p_krs_cmb_04]

	/* 4 ~ 7 */
	m_AnimationIndices.push_back(351);	//[351]	p_krs_cmb_01_fin[p_krs_cmb_01_fin]
	m_AnimationIndices.push_back(357);	//[357]	p_krs_cmb_02_fin[p_krs_cmb_02_fin]
	m_AnimationIndices.push_back(361);	//[361]	p_krs_cmb_03_fin[p_krs_cmb_03_fin]
	m_AnimationIndices.push_back(363);	//[363]	p_krs_cmb_04_fin[p_krs_cmb_04_fin]
	
	/* 8 ~ 9 */
	m_AnimationIndices.push_back(390);	//[390]	p_krs_sync_cmb_03_fin[p_krs_sync_cmb_03_fin]
	m_AnimationIndices.push_back(364);	//[364]	p_krs_cmb_04_finw[p_krs_cmb_04_finw]

	/* 10 */
	m_AnimationIndices.push_back(330);	//[330]	p_krs_atk_down_trample[p_krs_atk_down_trample]
}

void CKiryu_KRS_Attack::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

	if (nullptr != pTargetObject)
	{
		_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
	}

	if (m_pGameInstance->GetKeyState(DIK_Q) == TAP)
	{
		if (m_iComboCount == 6)
		{
			if (Changeable_Combo_Animation())
				m_iComboCount = 8;
		}
	}
		
}

void CKiryu_KRS_Attack::Change_Animation()
{
	if (0 > m_iComboCount) return;

	m_pPlayer->Change_Animation(m_AnimationIndices[m_iComboCount]);
}

_bool CKiryu_KRS_Attack::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		Reset();
		return true;
	}

	return false;
}

void CKiryu_KRS_Attack::Reset()
{
	m_iComboCount = 0;
}

void CKiryu_KRS_Attack::Combo_Count(_bool isFinAction)
{
	if (isFinAction)
	{
		if (m_iComboCount == 7)
		{
			if (Changeable_Combo_Animation())
				m_iComboCount = 9;
		}

		if (m_iComboCount < 4)
			m_iComboCount += 4;
	}
	else
	{
		if (Changeable_Combo_Animation())
		{
			m_iComboCount++;

			if (m_iComboCount > 3)
				m_iComboCount = 0;
		}
	}

	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();
	if (nullptr != pTargetObject)
	{
		if (static_cast<CMonster*>(pTargetObject)->isDown())
			m_iComboCount = 10;
	}
}		

_bool CKiryu_KRS_Attack::Changeable_Combo_Animation()
{
	_float fInterval = 0.15f;

	if (m_iComboCount > 0)			// 3타째 콤보
		fInterval = 0.3f;

	if (m_iComboCount > 2)			// 마지막 발차기 콤보
		fInterval = 0.4f;

	if (m_iComboCount > 3)			// 그 이상은 피니시 블로우기때문에 중간이 끊지않는다
		fInterval = 1.f;			// 1로 해두면 비율 연산 결과가 1이 넘을일이 없기때문에 아예 안끊김

	if (m_iComboCount == 6)			// p_krs_cmb_04_fin가 끝날 위치를 지정한다
		fInterval = 0.27f;

	if (m_iComboCount == 7)			// p_krs_cmb_04_fin가 끝날 위치를 지정한다
		fInterval = 0.3f;			

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (fInterval < *pModelCom->Get_AnimationCurrentPosition() / *pModelCom->Get_AnimationDuration())
		return true;

	return false;
}

CBehaviorAnimation* CKiryu_KRS_Attack::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Attack* pInstnace = new CKiryu_KRS_Attack();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Attack");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Attack::Free()
{
	__super::Free();
}