#include "GameInstance.h"
#include "Kiryu_KRH_Down.h"
#include "Player.h"

CKiryu_KRH_Down::CKiryu_KRH_Down()
	:CBehaviorAnimation{}
{
	// 루프 모션 0 ~ 1
	m_AnimationIndices.push_back(32); //[32]	c_dwn_b[c_dwn_b]
	m_AnimationIndices.push_back(45); //[45]	c_dwn_f[c_dwn_f]

	//일어나는 모션 2 ~ 3
	m_AnimationIndices.push_back(46); //[46]	c_standup_dnb_fast[c_standup_dnb_fast]
	m_AnimationIndices.push_back(47); //[47]	c_standup_dnf_fast[c_standup_dnf_fast]

	// 밟힐때 모션 (팔딱거리는거) 4 ~ 5
	m_AnimationIndices.push_back(10); //[10]	c_dam_dnb_trample
	m_AnimationIndices.push_back(13); //[13]	c_dam_dnf_trample

	// 밟힐때 모션 (팔딱거리는거) 6 ~ 9
	m_AnimationIndices.push_back(8); //[8]	c_dam_dnb_l		//Back->Front
	m_AnimationIndices.push_back(9); //[9]	c_dam_dnb_r
	m_AnimationIndices.push_back(11); //[11]	c_dam_dnf_l	// Front->Back
	m_AnimationIndices.push_back(12); //[12]	c_dam_dnf_r
}

void CKiryu_KRH_Down::Tick(const _float& fTimeDelta)
{
	if (!m_isStop && (m_pGameInstance->GetKeyState(DIK_W) == HOLD || m_pGameInstance->GetKeyState(DIK_S) == HOLD
		|| m_pGameInstance->GetKeyState(DIK_A) == HOLD || m_pGameInstance->GetKeyState(DIK_D) == HOLD))
	{
		Stop();
	}

	switch (m_eAnimState)
	{
	case CKiryu_KRH_Down::ANIM_LOOP:
		if (m_isStop)
		{
			m_iCurrentIndex += 2;
			m_eAnimState = ANIM_END;
		}
		break;
	}
}

void CKiryu_KRH_Down::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRH_Down::Get_AnimationEnd()
{
	if (m_eAnimState != ANIM_END) return false;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		if (m_eAnimState == ANIM_ONCE)
		{
			if (m_isFront)
				m_iCurrentIndex = 1;
			else
				m_iCurrentIndex = 0;

			m_eAnimState = ANIM_LOOP;
		}
		else
		{
			Reset();
		}
		return true;
	}

	return false;
}

void CKiryu_KRH_Down::Stop()
{
	m_isStop = true;
}

void CKiryu_KRH_Down::Setting_Value(void* pValue)
{
	KRH_DOWN_DESC* pDownState = static_cast<KRH_DOWN_DESC*>(pValue);

	//iDownState값이 -1이 아니라면 넘어지고 유지되는 것에 대한 처리가 필요
	if (pDownState->iDownState != -1)
	{
		m_eAnimState == ANIM_LOOP;

		// 0 B(뒤), 1 F(앞)
		if (pDownState->iDownState == 0)
		{
			m_isFront = false; //뒤
			m_iCurrentIndex = 0;
		}
		else if (pDownState->iDownState == 1)
		{
			m_isFront = true; //앞
			m_iCurrentIndex = 1;
		}
	}
	// iDownState값이 -1이고 iType나 iDirection가 -1이 아니라면 넘어진 상태에서 히트당했을 때 처리
	else if (pDownState->iDirection != -1)
	{
		m_eAnimState == ANIM_ONCE;

		_bool isTrample = { false };
		if (pDownState->strAnimationName == "e_kta_atk_down")	// 밟기
		{
			isTrample = true;
		}
		//else if (pDownState->strAnimationName == "e_kuz_atk_down")	// 차기

		//true: 밟기, false: 차기
		if (isTrample)
		{
			// 앞
			if (m_isFront)
			{
				m_iCurrentIndex = 5;
			}
			else if (!m_isFront)
			{
				m_iCurrentIndex = 4;
			}
		}
		else
		{
			// 상대방이 내 기준 앞이나 오른쪽에서 나를 찼다!
			// 그럼 왼쪽으로 구르자
			if (pDownState->iDirection == CPlayer::F || pDownState->iDirection == CPlayer::R)
			{
				//그리고 나는 드러누워있는 상태
				if (m_isFront)
				{
					m_iCurrentIndex = 8;
				}
				else if (!m_isFront)
				{
					m_iCurrentIndex = 6;
				}
			}
			else if (!m_isFront)
			{
				//그리고 나는 드러누워있는 상태
				if (m_isFront)
				{
					m_iCurrentIndex = 9;
				}
				else if (!m_isFront)
				{
					m_iCurrentIndex = 7;
				}
			}

			// 굴러갈때는 앞뒤를 반대로 저장해야한다.
			m_isFront = !m_isFront;
		}
	}
}

_bool CKiryu_KRH_Down::Stopping()
{
	if (ANIM_END == m_eAnimState && !Get_AnimationEnd()) return true;

	return false;
}

void CKiryu_KRH_Down::Reset()
{
	m_iCurrentIndex = 0;
	m_eAnimState = { ANIM_LOOP };
	m_isStop = false;
}

CBehaviorAnimation* CKiryu_KRH_Down::Create(CPlayer* pPlayer)
{
	CKiryu_KRH_Down* pInstnace = new CKiryu_KRH_Down();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Down");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRH_Down::Free()
{
	__super::Free();
}