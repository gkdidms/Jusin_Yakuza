#include "GameInstance.h"
#include "Kiryu_KRS_Grab.h"
#include "Player.h"
#include "Camera.h"

CKiryu_KRS_Grab::CKiryu_KRS_Grab()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(647);	// 0 [647]	[p_seize_miss]

	/* lapel */
	m_AnimationIndices.push_back(584);	//1 [584] [p_kru_sync_lapel_st]
	m_AnimationIndices.push_back(578);	//2 [578] [p_kru_sync_lapel_lp]
	m_AnimationIndices.push_back(585);	//3 [585] [p_kru_sync_lapel_walk]
	
	m_AnimationIndices.push_back(580);	//4 [580] [p_kru_sync_lapel_off]			// 잡고있다 놓침
	m_AnimationIndices.push_back(581);	//5 [581] [p_kru_sync_lapel_off_hiza]		// 잡고있다가 놓치면서 배맞음
	m_AnimationIndices.push_back(582);	//6 [582] [p_kru_sync_lapel_press]		// 잡고있다가 얼굴맞으면서 놓치고 넘어짐
	
	m_AnimationIndices.push_back(583);	// 7 [583] [p_kru_sync_lapel_resist]		// 잡고 흔들림

	m_AnimationIndices.push_back(574);	// 8 [574] [p_kru_sync_lapel_atk_punch]
	m_AnimationIndices.push_back(575);	// 9 [575] [p_kru_sync_lapel_cmb_01]
	m_AnimationIndices.push_back(576);	// 10 [576] [p_kru_sync_lapel_cmb_02]
	m_AnimationIndices.push_back(577);	// 11 [577] [p_kru_sync_lapel_cmb_03]

	m_AnimationIndices.push_back(579);	// 12 [579] [p_kru_sync_lapel_nage]

	/* neck */
	m_AnimationIndices.push_back(596);	// 13 [596] [p_kru_sync_neck_st]
	m_AnimationIndices.push_back(590);	// 14 [590] [p_kru_sync_neck_lp]
	m_AnimationIndices.push_back(597);	// 15 [597] [p_kru_sync_neck_walk]

	m_AnimationIndices.push_back(592);	//[592] [p_kru_sync_neck_off]
	m_AnimationIndices.push_back(593);	//[593] [p_kru_sync_neck_off_uraken]	//잡고있다 끌려가면서 놓침
	m_AnimationIndices.push_back(594);	//[594] [p_kru_sync_neck_press]			// 잡고있다가 놓치고 넘어짐
	
	m_AnimationIndices.push_back(595);	//[595] [p_kru_sync_neck_resist]

	m_AnimationIndices.push_back(586);	//[586] [p_kru_sync_neck_atk_kick]
	m_AnimationIndices.push_back(587);	//[587] [p_kru_sync_neck_cmb_01]
	m_AnimationIndices.push_back(588);	//[588] [p_kru_sync_neck_cmb_02]
	m_AnimationIndices.push_back(589);	//[589] [p_kru_sync_neck_cmb_03]
	
	m_AnimationIndices.push_back(591);	//[591] [p_kru_sync_neck_nage]

	/* leg */
	m_AnimationIndices.push_back(700);	//[700]	[p_sync_leg_st_b]
	m_AnimationIndices.push_back(701);	//[701]	[p_sync_leg_st_f]
	m_AnimationIndices.push_back(697);	//[697]	[p_sync_leg_lp]
	m_AnimationIndices.push_back(702);	//[702]	[p_sync_leg_walk]

	m_AnimationIndices.push_back(699);	//[699]	[p_sync_leg_off]

	m_AnimationIndices.push_back(698);	//[698]	[p_sync_leg_nage]

	/* etc */
	m_AnimationIndices.push_back(695);	//[695]	[p_sync_lapel_to_neck]
	
	m_AnimationIndices.push_back(693);	//[693]	[p_sync_head_b]			// 일으켜 세우는거, 누워있는 상대의 머리쪽에서 잡기키 한번 더 누르면 실행된다.
	m_AnimationIndices.push_back(694);	//[694]	[p_sync_head_f]
}

void CKiryu_KRS_Grab::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

	if (nullptr != pTargetObject)
	{
		_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
	}

	if (m_isGrabed)
	{
		if (Changeable_Combo_Animation())
		{
			switch (m_iDirection)
			{
			// lapel (정면)
			case CPlayer::F:
			{
				switch (m_eAnimState)
				{
				case ANIM_START:
				{
					if (Get_AnimationEnd())
						m_iCurrentIndex = 2;

					break;
				}
				case ANIM_LOOP:
					// 여기에 키인풋
					Move_KeyInput(fTimeDelta);

					if (m_isStop)
						m_eAnimState = ANIM_END;

					break;
				case ANIM_END:
					// 놓치는게 end (p_kru_sync_neck_off)
					break;
				default:
					break;
				}

				
				break;
			}
			//neck
			case CPlayer::B:
			{
				m_iCurrentIndex = 14;
				break;
			}
			// lapel (정면)
			case CPlayer::R:
			{
				m_iCurrentIndex = 2;
				break;
			}
			//neck
			case CPlayer::L:
			{
				m_iCurrentIndex = 14;
				break;
			}
			}
		}
	}


	switch (m_eAnimState)
	{
	case CKiryu_KRS_Grab::ANIM_LOOP:
	{

		break;
	}
	case CKiryu_KRS_Grab::ANIM_END:
	{

		break;
	}
	}
}

void CKiryu_KRS_Grab::Change_Animation()
{
	if (0 > m_iCurrentIndex) return;

	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRS_Grab::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		// 잡지못했을 때에는 애니메이션 스테이트 관계없이 종료를 반환한다.
		if (!m_isGrabed)
		{
			Reset();
			return true;
		}

		if (ANIM_START == m_eAnimState)
		{
			m_eAnimState = ANIM_LOOP;
		}
		else if (ANIM_END == m_eAnimState)
		{
			Reset();
			return true;
		}
	}

	return false;
}

void CKiryu_KRS_Grab::Reset()
{
	m_iComboCount = 0;
	m_iDirection = -1;
	m_eAnimState = ANIM_START;
	m_isGrabed = false;
	m_isShaked = false;
	m_iCurrentIndex = 0;
}

void CKiryu_KRS_Grab::Combo_Count(_bool isFinAction)
{
	if (Changeable_Combo_Animation())
	{
		m_iComboCount++;

		if (m_iComboCount > 2)
			m_iComboCount = 0;
	}
}

void CKiryu_KRS_Grab::Stop()
{
	m_isStop = true;
}

// 여기서는 따로 넘겨받는 값 없이 
void CKiryu_KRS_Grab::Setting_Value(void* pValue)
{
	KRS_Grab_DESC* pDesc = static_cast<KRS_Grab_DESC*>(pValue);
	m_isGrabed = pDesc->isGrabed;
	m_iDirection = pDesc->iDirection;

	// 여기서부터 이어서 하면 됨
	// 넘겨받는 값을 가지고 뒤에서 잡을지 앞에서 잡을지 구분하고, 애니메이션 인덱스 설정해주는거 하려고했엇음
}

_bool CKiryu_KRS_Grab::Changeable_Combo_Animation()
{
	_float fInterval = 1.f;

	if (0 == m_iCurrentIndex)
	{
		fInterval = 0.2f;
	}

	if (Checked_Animation_Ratio(fInterval))
	{
		m_isShaked = false;
		return true;
	}

	return false;
}

void CKiryu_KRS_Grab::Shaking()
{
	// 킥콤보 첫타에는 쉐이킹 제외
	if (m_iComboCount > 0)
	{
		if (!m_isShaked && Checked_Animation_Ratio(0.3))
		{
			m_isShaked = true;
			//카메라 쉐이킹
			CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
			pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.2, 0.25);
		}
	}
}

void CKiryu_KRS_Grab::Move_KeyInput(const _float& fTimeDelta)
{
}

CBehaviorAnimation* CKiryu_KRS_Grab::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Grab* pInstnace = new CKiryu_KRS_Grab();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_KickCombo");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Grab::Free()
{
	__super::Free();
}