#include "GameInstance.h"
#include "Kiryu_KRS_KickCombo.h"
#include "Player.h"

CKiryu_KRS_KickCombo::CKiryu_KRS_KickCombo()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(226);	//[226]	p_krs_atk_kick_f[p_krs_atk_kick_f]
	m_AnimationIndices.push_back(227);	//[227]	p_krs_atk_kick_f_02[p_krs_atk_kick_f_02]
	m_AnimationIndices.push_back(228);	//[228]	p_krs_atk_kick_f_03[p_krs_atk_kick_f_03]
}

void CKiryu_KRS_KickCombo::Tick(const _float& fTimeDelta)
{

}

void CKiryu_KRS_KickCombo::Change_Animation()
{
	if (0 > m_iComboCount) return;

	m_pPlayer->Change_Animation(m_AnimationIndices[m_iComboCount]);
}

_bool CKiryu_KRS_KickCombo::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		Reset();
		return true;
	}

	return false;
}

void CKiryu_KRS_KickCombo::Reset()
{
	m_iComboCount = 0;
}

void CKiryu_KRS_KickCombo::Combo_Count(_bool isFinAction)
{
	if (Changeable_Combo_Animation())
	{
		m_iComboCount++;

		if (m_iComboCount > 2)
			m_iComboCount = 0;
	}
}

_bool CKiryu_KRS_KickCombo::Changeable_Combo_Animation()
{
	_float fInterval = 0.18f;

	if (m_iComboCount > 0)			// 3타째 콤보
		fInterval = 0.4f;

	if (m_iComboCount > 1)			// 그 이상은 피니시 블로우기때문에 중간이 끊지않는다
		fInterval = 1.f;			// 1로 해두면 비율 연산 결과가 1이 넘을일이 없기때문에 아예 안끊김

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (fInterval < *pModelCom->Get_AnimationCurrentPosition() / *pModelCom->Get_AnimationDuration())
		return true;

	return false;
}

CBehaviorAnimation* CKiryu_KRS_KickCombo::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_KickCombo* pInstnace = new CKiryu_KRS_KickCombo();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_KickCombo");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_KickCombo::Free()
{
	__super::Free();
}