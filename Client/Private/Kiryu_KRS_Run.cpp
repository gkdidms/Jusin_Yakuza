#include "GameInstance.h"
#include "Kiryu_KRS_Run.h"
#include "Player.h"

CKiryu_KRS_Run::CKiryu_KRS_Run()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(180); //+[180]	p_krc_mov_run_btl[p_krc_mov_run_btl]
	m_AnimationIndices.push_back(181); // +[181]	p_krc_mov_run_stop_btl[p_krc_mov_run_stop_btl]
}

void CKiryu_KRS_Run::Tick(const _float& fTimeDelta)
{
	switch (m_eAnimState)
	{
	case CKiryu_KRS_Run::ANIM_LOOP:
		if (m_isStop)
			m_eAnimState = ANIM_END;
		break;
	}
}

void CKiryu_KRS_Run::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_eAnimState]);
}

_bool CKiryu_KRS_Run::Get_AnimationEnd()
{
	if (m_eAnimState != ANIM_END) return false;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		m_eAnimState = { ANIM_LOOP};
		m_isStop = false;

		return true;
	}

	return false;
}

void CKiryu_KRS_Run::Stop()
{
	m_isStop = true;
}

CBehaviorAnimation* CKiryu_KRS_Run::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Run* pInstnace = new CKiryu_KRS_Run();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Run");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Run::Free()
{
	__super::Free();
}