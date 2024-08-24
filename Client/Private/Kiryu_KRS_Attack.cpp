#include "GameInstance.h"
#include "Kiryu_KRS_Attack.h"
#include "Player.h"
#include "Monster.h"
#include "Camera.h"

CKiryu_KRS_Attack::CKiryu_KRS_Attack()
	:CBehaviorAnimation{}
{
	// ���� �� ���̵� ����� ��庥ó�� �����ϴ�
	
	/* 0 ~ 3 */
	m_AnimationIndices.push_back(519);	//[519]	p_krs_cmb_01[p_krs_cmb_01]
	m_AnimationIndices.push_back(528);	//[528]	p_krs_cmb_02[p_krs_cmb_02]
	m_AnimationIndices.push_back(533);	//[533]	p_krs_cmb_03[p_krs_cmb_03]
	m_AnimationIndices.push_back(537);	//[537]	p_krs_cmb_04[p_krs_cmb_04]

	/* 4 ~ 7 */
	m_AnimationIndices.push_back(526);	//[526]	p_krs_cmb_01_fin[p_krs_cmb_01_fin]
	m_AnimationIndices.push_back(532);	//[532]	p_krs_cmb_02_fin[p_krs_cmb_02_fin]
	m_AnimationIndices.push_back(536);	//[536]	p_krs_cmb_03_fin[p_krs_cmb_03_fin]
	m_AnimationIndices.push_back(538);	//[538]	p_krs_cmb_04_fin[p_krs_cmb_04_fin]
	
	/* 8 ~ 9 */
	m_AnimationIndices.push_back(573);	//[573]	p_krs_sync_cmb_03_fin[p_krs_sync_cmb_03_fin]
	m_AnimationIndices.push_back(539);	//[539]	p_krs_cmb_04_finw[p_krs_cmb_04_finw]

	/* 10 */
	m_AnimationIndices.push_back(505);	//[505]	p_krs_atk_down_trample[p_krs_atk_down_trample]
}

void CKiryu_KRS_Attack::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

	if (nullptr != pTargetObject)
	{
		if (8 != m_iComboCount)
		{
			_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
			m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
		}

		if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
		{
			if (m_iComboCount == 6)
			{
				// �ٿ�������� �����ͼ� �ٿ� ���¶�� ��Ʈ�׼� ����
				if (static_cast<CMonster*>(pTargetObject)->isDown())
				{
					// ��Ʈ�׼��� �����ų �Լ��� ȣ���ؾ��Ѵ�.
					// �ٵ� ���� AnimCom�� �����ϰ� �ִ� �ִϸ��̼� ������ string���� ó���ؾ��ϴµ�, ������� ó�������� ���� ������̾���
					m_pPlayer->Set_CutSceneAnim(CPlayer::FINISHBLOW); // [8] f209_special_09[f_kiryu] ũ�� ȭ�� ��
					m_iComboCount = 99;
				}
			}
		}
	}

	if (m_pGameInstance->GetKeyState(DIK_Q) == TAP)
	{
		if (m_iComboCount == 6)
		{
			if (static_cast<CMonster*>(pTargetObject)->isDown())
			{
				if (Changeable_Combo_Animation())
				{
					static_cast<CMonster*>(m_pPlayer->Get_TargetObject())->Set_Sync("p_krs_sync_cmb_03_fin");
					m_iComboCount = 8;
				}
			}
		}
	}

	Shaking();

	if (!m_isFreezeEventOn && m_iComboCount == 8 && m_AnimationIndices[m_iComboCount] == static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")))->Get_CurrentAnimationIndex())
	{
		if (Checked_Animation_Ratio(0.38f))
		{
			m_isFreezeEventOn = true;
			m_pPlayer->HitFreeze_On();
		}
	}
}

void CKiryu_KRS_Attack::Change_Animation()
{
	if (0 > m_iComboCount) return;

	if(m_iComboCount != 99)
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
	m_isShaked = false;
	m_isFreezeEventOn = false;
}

void CKiryu_KRS_Attack::Combo_Count(_bool isFinAction)
{
	// �ǴϽ� ����� �� ��� �߿��� �޺�ī��Ʈ �����ʴ´�
	if (m_iComboCount == 99) return;

	// ������ ������ �⺻������ 1��� �������ش�
	m_pPlayer->Set_DamageAmplify(1.f);

	// ������ ������ �� �и� ����
	m_pPlayer->Off_Separation_Hand();

	if (isFinAction)
	{
		// �����ݿ��� ������ 1.5��
		m_pPlayer->Set_DamageAmplify(1.5f);

		if (m_iComboCount == 7)
		{
			if (Changeable_Combo_Animation())
			{
				// ������ ��ų���� ������ 2��
				m_pPlayer->Set_DamageAmplify(2.f);
				m_iComboCount = 9;
			}
		}

		if (Changeable_Combo_Animation())
		{
			if (m_iComboCount < 4)
				m_iComboCount += 4;
		}
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

_bool CKiryu_KRS_Attack::IsFinishBlow()
{
	return (m_iComboCount != 6) && (3 < m_iComboCount) && (10 > m_iComboCount);
}

_bool CKiryu_KRS_Attack::Changeable_Combo_Animation()
{
	_float fInterval = 0.15f;

	if (m_iComboCount > 0)			// 3Ÿ° �޺�
		fInterval = 0.3f;

	if (m_iComboCount > 2)			// ������ ������ �޺�
		fInterval = 0.4f;

	if (m_iComboCount > 3)			// �� �̻��� �ǴϽ� ��ο�⶧���� �߰��� �����ʴ´�
		fInterval = 1.f;			// 1�� �صθ� ���� ���� ����� 1�� �������� ���⶧���� �ƿ� �Ȳ���

	if (m_iComboCount == 6)			// p_krs_cmb_03_fin�� ���� ��ġ�� �����Ѵ�
		fInterval = 0.27f;

	if (m_iComboCount == 7)			// p_krs_cmb_04_fin�� ���� ��ġ�� �����Ѵ�
		fInterval = 0.3f;			

	if (Checked_Animation_Ratio(fInterval))
	{
		m_isShaked = false;
		return true;
	}
	
	return false;
}

void CKiryu_KRS_Attack::Shaking()
{
	if (m_iComboCount > 3)
	{
		if (!m_isShaked && Checked_Animation_Ratio(0.3))
		{
			m_isShaked = true;
			//ī�޶� ����ŷ
			CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
			pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.2, 0.4);
		}
	}
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