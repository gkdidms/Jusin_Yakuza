#include "GameInstance.h"
#include "Kiryu_KRH_Attack.h"
#include "Player.h"

CKiryu_KRH_Attack::CKiryu_KRH_Attack()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	
	/* 0 ~ 3 */
	m_AnimationIndices.push_back(237);	//[237]	p_krs_cmb_01[p_krs_cmb_01]
	m_AnimationIndices.push_back(246);	//[246]	p_krs_cmb_02[p_krs_cmb_02]
	m_AnimationIndices.push_back(251);	//[251]	p_krs_cmb_03[p_krs_cmb_03]
	m_AnimationIndices.push_back(255);	//[255]	p_krs_cmb_04[p_krs_cmb_04]

	/* 4 ~ 7 */
	m_AnimationIndices.push_back(244);	//[244]	p_krs_cmb_01_fin[p_krs_cmb_01_fin]
	m_AnimationIndices.push_back(250);	//[250]	p_krs_cmb_02_fin[p_krs_cmb_02_fin]
	m_AnimationIndices.push_back(254);	//[254]	p_krs_cmb_03_fin[p_krs_cmb_03_fin]
	m_AnimationIndices.push_back(256);	//[256]	p_krs_cmb_04_fin[p_krs_cmb_04_fin]

	m_AnimationIndices.push_back(292);	//[292]	p_krs_sync_cmb_03_fin[p_krs_sync_cmb_03_fin]
	m_AnimationIndices.push_back(257);	//[257]	p_krs_cmb_04_finw[p_krs_cmb_04_finw]
}

void CKiryu_KRH_Attack::Tick(const _float& fTimeDelta)
{

	if (m_pGameInstance->GetKeyState(DIK_Q) == TAP)
	{
		if (m_iComboCount == 6)
		{
			if (Changeable_Combo_Animation())
				m_iComboCount = 8;
		}
	}
		
}

void CKiryu_KRH_Attack::Change_Animation()
{
	if (0 > m_iComboCount) return;

	m_pPlayer->Change_Animation(m_AnimationIndices[m_iComboCount]);
}

_bool CKiryu_KRH_Attack::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		Reset();
		return true;
	}

	return false;
}

void CKiryu_KRH_Attack::Reset()
{
	m_iComboCount = 0;
}

void CKiryu_KRH_Attack::Combo_Count(_bool isFinAction)
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
	
}		

_bool CKiryu_KRH_Attack::Changeable_Combo_Animation()
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
		fInterval = 0.35f;			

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (fInterval < *pModelCom->Get_AnimationCurrentPosition() / *pModelCom->Get_AnimationDuration())
		return true;

	return false;
}

CBehaviorAnimation* CKiryu_KRH_Attack::Create(CPlayer* pPlayer)
{
	CKiryu_KRH_Attack* pInstnace = new CKiryu_KRH_Attack();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Attack");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRH_Attack::Free()
{
	__super::Free();
}