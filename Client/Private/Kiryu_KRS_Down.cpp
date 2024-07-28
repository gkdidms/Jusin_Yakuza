#include "GameInstance.h"
#include "Kiryu_KRS_Down.h"
#include "Player.h"

CKiryu_KRS_Down::CKiryu_KRS_Down()
	:CBehaviorAnimation{}
{
	// ���� ��� 0 ~ 1
	m_AnimationIndices.push_back(74); //[74]	c_dwn_b[c_dwn_b]
	m_AnimationIndices.push_back(115); //[115]	c_dwn_f[c_dwn_f]

	//�Ͼ�� ��� 2 ~ 3
	m_AnimationIndices.push_back(216); //[216]	c_standup_dnb_fast[c_standup_dnb_fast]
	m_AnimationIndices.push_back(218); //[218]	c_standup_dnf_fast[c_standup_dnf_fast]

	// ������ ��� (�ȵ��Ÿ��°�) 4 ~ 5
	m_AnimationIndices.push_back(24); //[24]	c_dam_dnb_trample
	m_AnimationIndices.push_back(27); //[27]	c_dam_dnf_trample

	// ������ ��� (�ȵ��Ÿ��°�) 6 ~ 9
	m_AnimationIndices.push_back(22); //[22]	c_dam_dnb_l		//Back->Front
	m_AnimationIndices.push_back(23); //[23]	c_dam_dnb_r
	m_AnimationIndices.push_back(25); //[25]	c_dam_dnf_l	// Front->Back
	m_AnimationIndices.push_back(26); //[26]	c_dam_dnf_r
}

void CKiryu_KRS_Down::Tick(const _float& fTimeDelta)
{
	// ���� ���¿��� Ű �Է��� ������ �Ͼ�� ��� ����
	if (!m_isStop && (m_pGameInstance->GetKeyState(DIK_W) == HOLD || m_pGameInstance->GetKeyState(DIK_S) == HOLD
		|| m_pGameInstance->GetKeyState(DIK_A) == HOLD || m_pGameInstance->GetKeyState(DIK_D) == HOLD))
	{
		Stop();
	}

	switch (m_eAnimState)
	{
	case CKiryu_KRS_Down::ANIM_LOOP:
		if (m_isStop)
		{
			m_iCurrentIndex += 2;
			m_eAnimState = ANIM_END;
		}
		break;
	}
}

void CKiryu_KRS_Down::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRS_Down::Get_AnimationEnd()
{
	if (m_eAnimState == ANIM_LOOP) return false;

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

			return false;
		}
		else
		{
			Reset();
		}
		return true;
	}

	return false;
}

void CKiryu_KRS_Down::Stop()
{
	m_isStop = true;
}

void CKiryu_KRS_Down::Setting_Value(void* pValue)
{
	KRS_DOWN_DESC* pDownState = static_cast<KRS_DOWN_DESC*>(pValue);

	//iDownState���� -1�� �ƴ϶�� �Ѿ����� �����Ǵ� �Ϳ� ���� ó���� �ʿ�
	if (pDownState->iDownState != -1)
	{
		m_eAnimState = ANIM_LOOP;

		// 0 B(��), 1 F(��)
		if (pDownState->iDownState == 0)
		{
			m_isFront = false; //��
			m_iCurrentIndex = 0;
		}
		else if(pDownState->iDownState == 1)
		{
			m_isFront = true; //��
			m_iCurrentIndex = 1;
		}
	}
	// iDownState���� -1�̰� iType�� iDirection�� -1�� �ƴ϶�� �Ѿ��� ���¿��� ��Ʈ������ �� ó��
	else if (pDownState->iDirection != -1)
	{
		m_eAnimState = ANIM_ONCE;

		_bool isTrample = { false };
		if (m_pGameInstance->Extract_String(pDownState->strAnimationName, '[', ']') == "e_kta_atk_down")	// ���
		{
			isTrample = true;	
		}
		//else if (pDownState->strAnimationName == "e_kuz_atk_down")	// ����

		//true: ���, false: ����
		if (isTrample)
		{
			// ��
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
			// ������ �� ���� ���̳� �����ʿ��� ���� á��!
			// �׷� �������� ������
			if (pDownState->iDirection == CPlayer::F || pDownState->iDirection == CPlayer::R)
			{
				//�׸��� ���� �巯�����ִ� ����
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
				//�׸��� ���� �巯�����ִ� ����
				if (m_isFront)
				{
					m_iCurrentIndex = 9;
				}
				else if (!m_isFront)
				{
					m_iCurrentIndex = 7;
				}
			}

			// ���������� �յڸ� �ݴ�� �����ؾ��Ѵ�.
			m_isFront = !m_isFront;
		}
	}
}

_bool CKiryu_KRS_Down::Stopping()
{
	if (ANIM_END == m_eAnimState && !Get_AnimationEnd()) return true;

	return false;
}

void CKiryu_KRS_Down::Reset()
{
	m_iCurrentIndex = 0;
	m_eAnimState = { ANIM_LOOP };
	m_isStop = false;
}

CBehaviorAnimation* CKiryu_KRS_Down::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Down* pInstnace = new CKiryu_KRS_Down();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Run");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Down::Free()
{
	__super::Free();
}