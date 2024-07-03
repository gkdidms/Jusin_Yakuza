#include "GameInstance.h"
#include "Kiryu_KRS_Attack.h"
#include "Player.h"

CKiryu_KRS_Attack::CKiryu_KRS_Attack()
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
	
	// 8
	m_AnimationIndices.push_back(226);	//[226]	p_krs_atk_kick_f[p_krs_atk_kick_f]

	//m_AnimationIndices.push_back(238);	//[238]	p_krs_cmb_01_b[p_krs_cmb_01_b]
	//m_AnimationIndices.push_back(239);	//[239]	p_krs_cmb_01_b_fin[p_krs_cmb_01_b_fin]
	//m_AnimationIndices.push_back(240);	//[240]	p_krs_cmb_01_b_fin_nml[p_krs_cmb_01_b_fin_nml]
	//m_AnimationIndices.push_back(241);	//[241]	p_krs_cmb_01_b_nml[p_krs_cmb_01_b_nml]
	//m_AnimationIndices.push_back(242);	//[242]	p_krs_cmb_01_bf[p_krs_cmb_01_bf]
	//m_AnimationIndices.push_back(243);	//[243]	p_krs_cmb_01_bf_nml[p_krs_cmb_01_bf_nml]
	//m_AnimationIndices.push_back(245);	//[245]	p_krs_cmb_01_run[p_krs_cmb_01_run]

	///* cmb2 9 ~ 13 */
	//m_AnimationIndices.push_back(247);	//[247]	p_krs_cmb_02_b[p_krs_cmb_02_b]
	//m_AnimationIndices.push_back(248);	//[248]	p_krs_cmb_02_b_fin[p_krs_cmb_02_b_fin]
	//m_AnimationIndices.push_back(249);	//[249]	p_krs_cmb_02_bf[p_krs_cmb_02_bf]
	

	///* cmb3 14 ~ 17 */
	//m_AnimationIndices.push_back(252);	//[252]	p_krs_cmb_03_b[p_krs_cmb_03_b]
	//m_AnimationIndices.push_back(253);	//[253]	p_krs_cmb_03_b_fin[p_krs_cmb_03_b_fin]
	

	///* cmb4 18 ~ 20 */
	//m_AnimationIndices.push_back(257);	//[257]	p_krs_cmb_04_finw[p_krs_cmb_04_finw]

	///* 다운공격 21 */
	//m_AnimationIndices.push_back(0);	//p_atk_down_trample 0 (다운된 상태의 적 기본공격)
}

void CKiryu_KRS_Attack::Tick(const _float& fTimeDelta)
{

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
	m_iComboCount = -1;
}

void CKiryu_KRS_Attack::Combo_Count(_bool isFinAction)
{
	if (isFinAction)
	{
		// 콤보 도중 우클릭이면 피니시 모션을 출력하는게 맞지만,
		// 그냥 처음부터 우클릭을 한 경우는 발차기 애니메이션을 진행해야함
		if (m_iComboCount == -1)
			m_iComboCount = 8;
		// 인덱스 3번까지는 기본콤보고, +4하게되면 블로우 피니시 기술을 실행한다.
		else if (m_iComboCount < 4)
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

_bool CKiryu_KRS_Attack::Changeable_Combo_Animation()
{
	_float fInterval = 0.2f;

	if (m_iComboCount > 1)			// 3타째 콤보
		fInterval = 0.4f;

	if (m_iComboCount > 2)			// 마지막 발차기 콤보
		fInterval = 0.6f;

	if (m_iComboCount > 3)			// 그 이상은 피니시 블로우기때문에 중간이 끊지않는다
		fInterval = 1.f;			// 1로 해두면 비율 연산 결과가 1이 넘을일이 없기때문에 아예 안끊김

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
		MSG_BOX("Faild To Created : Kiryu_Adventure_Idle");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Attack::Free()
{
	__super::Free();
}