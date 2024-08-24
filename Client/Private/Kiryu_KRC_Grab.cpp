#include "GameInstance.h"
#include "Kiryu_KRC_Grab.h"
#include "Player.h"
#include "Camera.h"
#include "Monster.h"

CKiryu_KRC_Grab::CKiryu_KRC_Grab()
	:CBehaviorAnimation{}
{

	/*
	*	[323]	[p_krc_sync_kaihi_nage_b]
		[324]	[p_krc_sync_kaihi_nage_f]
		[325]	[p_krc_sync_lapel_atk_heavy]
		[326]	[p_krc_sync_lapel_atk_punch]
		[327]	[p_krc_sync_lapel_nage]
	*/
	m_AnimationIndices.push_back(647);	// 0 [647]	[p_seize_miss]

	/* lapel */
	m_AnimationIndices.push_back(584);	//1 [584] [p_kru_sync_lapel_st]
	m_AnimationIndices.push_back(578);	//2 [578] [p_kru_sync_lapel_lp]
	m_AnimationIndices.push_back(585);	//3 [585] [p_kru_sync_lapel_walk]
	
	m_AnimationIndices.push_back(580);	//4 [580] [p_kru_sync_lapel_off]			// ����ִ� ��ħ
	m_AnimationIndices.push_back(581);	//5 [581] [p_kru_sync_lapel_off_hiza]		// ����ִٰ� ��ġ�鼭 �����
	m_AnimationIndices.push_back(582);	//6 [582] [p_kru_sync_lapel_press]		// ����ִٰ� �󱼸����鼭 ��ġ�� �Ѿ���
	
	m_AnimationIndices.push_back(583);	// 7 [583] [p_kru_sync_lapel_resist]		// ��� ��鸲

	m_AnimationIndices.push_back(324);	// 8 [324]	[p_krc_sync_kaihi_nage_f]
	m_AnimationIndices.push_back(327);	// 9 [327]	[p_krc_sync_lapel_nage]

	/* etc */
	m_AnimationIndices.push_back(695);	// 31 [695]	[p_sync_lapel_to_neck]
	
	m_AnimationIndices.push_back(693);	// 32 [693]	[p_sync_head_b]			// ������ ����°�, �����ִ� ����� �Ӹ��ʿ��� ���Ű �ѹ� �� ������ ����ȴ�.
	m_AnimationIndices.push_back(694);	// 33 [694]	[p_sync_head_f]
}

void CKiryu_KRC_Grab::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

	if (m_iCurrentIndex != 9)
	{
		if (nullptr != pTargetObject)
		{
			_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
			m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
		}
	}

	if (m_isGrabed)
	{
		switch (m_eAnimState)
		{
		case SEIZE_TRY:
		{
			m_pPlayer->Off_Separation_Hand();
			if (Changeable_Combo_Animation())
			{
				m_eAnimState = ANIM_START;
				m_iCurrentIndex = 1;
			}

			break;
		}
		case ANIM_START:
		{
			m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_GU);
			m_pPlayer->On_Separation_Hand();
			if (Changeable_Combo_Animation())
			{
				m_eAnimState = ANIM_LOOP;
				m_iCurrentIndex = 2;
			}

			break;
		}
		case ANIM_LOOP:
		{
			m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_GU);
			m_pPlayer->On_Separation_Hand();

			CMonster* pTargetMonster = static_cast<CMonster*>(m_pPlayer->Get_TargetObject());
			if (nullptr != pTargetMonster)
				pTargetMonster->Set_Sync("p_kru_sync_lapel_lp");
			// ���⿡ Ű��ǲ
			Move_KeyInput(fTimeDelta);

			break;
		}
		case ANIM_END:
			// ��ġ�°� end (p_kru_sync_neck_off)
			break;
		default:
			break;
		}
	}

	Shaking();
}

void CKiryu_KRC_Grab::Change_Animation()
{
	if (0 > m_iCurrentIndex) return;

	m_pPlayer->Off_Separation_Hand();
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRC_Grab::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		// ���������� ������ �ִϸ��̼� ������Ʈ ������� ���Ḧ ��ȯ�Ѵ�.
		if (!m_isGrabed)
		{
			Reset();
			return true;
		}

		// (ANIM_START == m_eAnimState�� �׷� ���� ����̶�� ��
		if (ANIM_START == m_eAnimState)
		{
			m_eAnimState = ANIM_LOOP;
		}
		// �׷� �õ� ����̰� �׷��� �����ߴٸ� �׷� ���۸������ �ε����� �������ش�.
		if (SEIZE_TRY == m_eAnimState)
		{
			m_eAnimState = ANIM_START;
		}

		if (ANIM_ONCE == m_eAnimState)
		{
			if(m_isStop)
				m_eAnimState = ANIM_END;
			else
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

void CKiryu_KRC_Grab::Reset()
{
	m_iComboCount = 0;
	m_iDirection = -1;
	m_eAnimState = SEIZE_TRY;
	m_isGrabed = false;
	m_isShaked = false;
	m_isStop = false;
	m_iCurrentIndex = 0;
}

void CKiryu_KRC_Grab::Combo_Count(_bool isFinAction)
{
	if (Changeable_Combo_Animation())
	{
		m_iComboCount++;

		if (m_iComboCount > 2)
			m_iComboCount = 0;
	}
}

void CKiryu_KRC_Grab::Stop()
{
	m_isStop = true;
}

// ���⼭�� ���� �Ѱܹ޴� �� ���� 
void CKiryu_KRC_Grab::Setting_Value(void* pValue)
{
	KRC_Grab_DESC* pDesc = static_cast<KRC_Grab_DESC*>(pValue);
	m_isGrabed = pDesc->isGrabed;
	m_iDirection = pDesc->iDirection;

	CMonster* pTargetMonster = static_cast<CMonster*>(m_pPlayer->Get_TargetObject());
	if (nullptr != pTargetMonster)
		pTargetMonster->Set_Sync("p_kru_sync_lapel_st");
}

void CKiryu_KRC_Grab::Event(void* pValue)
{
	_bool* pIsOff = static_cast<_bool*>(pValue);

	if (pIsOff)
		Play_Off();
}

_bool CKiryu_KRC_Grab::Changeable_Combo_Animation()
{
	_float fInterval = 0.8f;

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

void CKiryu_KRC_Grab::Shaking()
{
	if (m_iCurrentIndex == 9)						// ��� �ٴڿ� ������� �ִϸ��̼�
	{
		if (!m_isShaked && Checked_Animation_Ratio(0.4))
		{
			m_isShaked = true;
			//ī�޶� ����ŷ
			CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
			pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.3, 0.4);
		}
	}
}

void CKiryu_KRC_Grab::Play_Off()
{
}

void CKiryu_KRC_Grab::Move_KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
	{
		if (2 < m_pPlayer->Get_CurrentHitLevel())	// ��Ʈ�������� 3�ܱ��� Ǯ�̶��
		{
			m_pPlayer->Set_CutSceneAnim(CPlayer::LAPEL_OIUCHI_NECK);
		}
	}
	if (m_pGameInstance->GetKeyState(DIK_Q) == TAP)
	{
		m_iCurrentIndex = 9;
		m_eAnimState = ANIM_ONCE;
		m_isStop = true;

		CMonster* pTargetMonster = static_cast<CMonster*>(m_pPlayer->Get_TargetObject());
		if (nullptr != pTargetMonster)
			pTargetMonster->Set_Sync("p_krc_sync_lapel_nage");
	}
}

CBehaviorAnimation* CKiryu_KRC_Grab::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Grab* pInstnace = new CKiryu_KRC_Grab();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_KickCombo");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Grab::Free()
{
	__super::Free();
}