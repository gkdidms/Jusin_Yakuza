#include "GameInstance.h"
#include "Kiryu_KRH_Attack.h"
#include "Player.h"

CKiryu_KRH_Attack::CKiryu_KRH_Attack()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	
	/* 0 ~ 7 */
	m_AnimationIndices.push_back(263);	//[263]	p_krh_cmb_01[p_krh_cmb_01]
	m_AnimationIndices.push_back(270);	//[270]	p_krh_cmb_02[p_krh_cmb_02]
	m_AnimationIndices.push_back(273);	//[273]	p_krh_cmb_03[p_krh_cmb_03]
	m_AnimationIndices.push_back(276);	//[276]	p_krh_cmb_04[p_krh_cmb_04]
	m_AnimationIndices.push_back(279);	//[279]	p_krh_cmb_05[p_krh_cmb_05]
	m_AnimationIndices.push_back(282);	//[282]	p_krh_cmb_06[p_krh_cmb_06]
	m_AnimationIndices.push_back(285);	//[285]	p_krh_cmb_07[p_krh_cmb_07]
	m_AnimationIndices.push_back(288);	//[288]	p_krh_cmb_08[p_krh_cmb_08]

	/* 8 ~ 15 */
	m_AnimationIndices.push_back(264);	//[264]	p_krh_cmb_01_fin[p_krh_cmb_01_fin]
	m_AnimationIndices.push_back(271);	//[271]	p_krh_cmb_02_fin[p_krh_cmb_02_fin]
	m_AnimationIndices.push_back(274);	//[274]	p_krh_cmb_03_fin[p_krh_cmb_03_fin]
	m_AnimationIndices.push_back(277);	//[277]	p_krh_cmb_04_fin[p_krh_cmb_04_fin]
	m_AnimationIndices.push_back(280);	//[280]	p_krh_cmb_05_fin[p_krh_cmb_05_fin]
	m_AnimationIndices.push_back(283);	//[283]	p_krh_cmb_06_fin[p_krh_cmb_06_fin]
	m_AnimationIndices.push_back(286);	//[286]	p_krh_cmb_07_fin[p_krh_cmb_07_fin]
	m_AnimationIndices.push_back(289);	//[289]	p_krh_cmb_08_fin[p_krh_cmb_08_fin]

	/* 16 ~ 23 */
	m_AnimationIndices.push_back(269);	//[269]	p_krh_cmb_01_tame[p_krh_cmb_01_tame]
	m_AnimationIndices.push_back(272);	//[272]	p_krh_cmb_02_tame[p_krh_cmb_02_tame]
	m_AnimationIndices.push_back(275);	//[275]	p_krh_cmb_03_tame[p_krh_cmb_03_tame]
	m_AnimationIndices.push_back(278);	//[278]	p_krh_cmb_04_tame[p_krh_cmb_04_tame]
	m_AnimationIndices.push_back(281);	//[281]	p_krh_cmb_05_tame[p_krh_cmb_05_tame]
	m_AnimationIndices.push_back(284);	//[284]	p_krh_cmb_06_tame[p_krh_cmb_06_tame]
	m_AnimationIndices.push_back(287);	//[287]	p_krh_cmb_07_tame[p_krh_cmb_07_tame]
	m_AnimationIndices.push_back(290);	//[290]	p_krh_cmb_08_tame[p_krh_cmb_08_tame]

	/*
		[265]	p_krh_cmb_01_sway_b[p_krh_cmb_01_sway_b]
		[266]	p_krh_cmb_01_sway_f[p_krh_cmb_01_sway_f]
		[267]	p_krh_cmb_01_sway_l[p_krh_cmb_01_sway_l]
		[268]	p_krh_cmb_01_sway_r[p_krh_cmb_01_sway_r]
	*/
}

void CKiryu_KRH_Attack::Tick(const _float& fTimeDelta)
{
		
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
		//if (m_iComboCount == 7)
		//{
		//	if (Changeable_Combo_Animation())
		//		m_iComboCount = 9;
		//}

		if (m_iComboCount < 8)
			m_iComboCount += 8;
	}
	else
	{
		if (Changeable_Combo_Animation())
		{
			m_iComboCount++;

			if (m_iComboCount > 7)
				m_iComboCount = 0;
		}
	}
	
}		

_bool CKiryu_KRH_Attack::Changeable_Combo_Animation()
{
	_float fInterval = 0.2f;

	//if (m_iComboCount > 0)			// 3타째 콤보
	//	fInterval = 0.3f;

	//if (m_iComboCount > 2)			// 마지막 발차기 콤보
	//	fInterval = 0.4f;

	//if (m_iComboCount > 3)			// 그 이상은 피니시 블로우기때문에 중간이 끊지않는다
	//	fInterval = 1.f;			// 1로 해두면 비율 연산 결과가 1이 넘을일이 없기때문에 아예 안끊김

	if (m_iComboCount == 5)			// 6타 발차기
		fInterval = 0.27f;

	if (m_iComboCount == 6)			// 7타 발차기 (스킬 찍어야 쓸 수 있음)
		fInterval = 0.27f;

	if (m_iComboCount == 7)			// 8타 발차기 (스킬 찍어야 쓸 수 있음)
		fInterval = 0.5f;			

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