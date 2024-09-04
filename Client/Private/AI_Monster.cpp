#include "AI_Monster.h"

#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

#include "Effect.h"

CAI_Monster::CAI_Monster()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CAI_Monster::CAI_Monster(const CAI_Monster& rhs)
	: m_pGameInstance{ rhs.m_pGameInstance }
{
	Safe_AddRef(m_pGameInstance);
}

void CAI_Monster::Reset_AI()
{
	m_isStart = false;
	m_isAdventer = false;
	m_isAttack = false;
	m_isAngry = false;
	m_isSync = false;
	m_isBreak = false;
	m_isGuard = false;
	m_isSway = false;
	m_isPlayerDownAtk = false;
	m_fAttackDelayTime = 0.f;
	m_fBreakTime = 0.f;
	m_fGuardTime = 0.f;
	m_fSyncTime = 0.f;
	m_fCmbCount = 0.f;
	m_fGuardAtkAcc = 0.f;
}

HRESULT CAI_Monster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Monster::Initialize(void* pArg)
{
	AI_MONSTER_DESC* pDesc = static_cast<AI_MONSTER_DESC*>(pArg);
	m_pState = pDesc->pState;
	m_pCurrentAnimType = pDesc->pCurrentAnimType;
	memcpy(m_pAnimCom, pDesc->pAnim, sizeof(CAnim*) * 2);
	//Safe_AddRef(m_pAnimCom);

	m_pThis = pDesc->pThis;
	//Safe_AddRef(m_pThis);

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
	//Safe_AddRef(m_pPlayer);

	m_fSwayDistance = _float2(2.f, 2.2f);

	return S_OK;
}

void CAI_Monster::Tick(const _float& fTimeDelta)
{
	if (m_isAttack == false)
		m_fAttackDelayTime += fTimeDelta;

	if (m_isBreak)
	{
		m_fBreakTime += fTimeDelta;

		if (m_fBreakDuration <= m_fBreakTime)
		{
			m_isBreak = false;
			m_fBreakTime = 0.f;
		}
	}

	if (m_isGuard)
		m_fGuardTime += fTimeDelta;

	if (m_isSync)
		m_fSyncTime += fTimeDelta;
}

void CAI_Monster::Ready_Tree()
{
}

//�÷��̾���� �Ÿ�
_float CAI_Monster::DistanceFromPlayer()
{
	_vector vPlayerPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vThisPos = m_pThis->m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDistance = XMVector3Length(vPlayerPos - vThisPos).m128_f32[0]; // �Ÿ� ����	

	return fDistance;
}

_bool CAI_Monster::isBehine()
{
	//���͸� �������� �÷��̾ �ڿ� �����ϴ��� Ȯ���ϱ�.
	_vector vThisLook = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	_vector vThisPos = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	_vector vToPlayer = XMVector3Normalize(vPlayerPos - vThisPos);

	_float fDot = XMVector3Dot(vThisLook, vToPlayer).m128_f32[0];
	_float fAngle = acos(fDot);

	//�÷��̾��� �þ߰��� 180���� �ΰ� ����Ѵ�.
	//�ڿ� �ִ��� ������ üũ.
	return  fAngle > (XMConvertToRadians(180.f) * 0.5f);
}

void CAI_Monster::LookAtPlayer()
{
	_vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	m_pThis->Get_TransformCom()->LookAt_For_LandObject(vPlayerPos);
}

_bool CAI_Monster::Find_PlayerCurrentAnimationName(string strAnimName)
{
	string strPlayerAnimName = m_pPlayer->Get_CurrentAnimationName();
	if (string_view(strPlayerAnimName) == string_view(strAnimName))
		return true;

	return false;
}

//����
_uint CAI_Monster::Check_KRH(_uint iPlayerLv, _bool isBehine, _bool isAnimChange)
{
	_uint iDir = PLAYER_ATK_DIR_END;

	if (Find_PlayerCurrentAnimationName("p_krh_atk_down")) // �ٿ�Ǿ�����
	{
		if (isAnimChange)
		{
			*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DNF_BOUND : CMonster::MONSTER_DWN_DNF_BOUND_G;

			m_pThis->Set_Down(true);

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [22].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4812 [5].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}


	if (Find_PlayerCurrentAnimationName("p_krh_atk_heavy_f")) // ������ ?
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [15].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F_SP : CMonster::MONSTER_DWN_BODY_F_SP;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B_SP : CMonster::MONSTER_DWN_BODY_B_SP;

				m_pThis->Set_Down(true);

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [16].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4717 [1].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	//if (Find_PlayerCurrentAnimationName("p_krh_atk_punch_b"))  //��ġ
	//	*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_PlayerCurrentAnimationName("p_krh_atk_run_heavy")) // ������
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [15].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B;

				m_pThis->Set_Down(true);

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [17].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4717 [2].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_atk_sp")) //������ 
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [14].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F_SP;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B_SP;

				m_pThis->Set_Down(true);

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [20].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4718 [2].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_atk_standup_kick_dnb")) // �ٿ��
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [18].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F_SP;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B_SP;

				m_pThis->Set_Down(true);

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [21].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4717 [2].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}


		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_atk_standup_kick_dnf"))
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [18].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F_SP;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B_SP;

				m_pThis->Set_Down(true);

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [17].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4718 [2].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_cmb_01"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_F : CMonster::MONSTER_DAM_HEAD_LV02_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [17].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}


			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4701 [6].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_cmb_02"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_L : CMonster::MONSTER_DAM_HEAD_LV02_L;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;
			

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [18].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4701 [7].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = L;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_cmb_03"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_R : CMonster::MONSTER_DAM_HEAD_LV02_R;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [21].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4701 [7].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = R;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_cmb_04"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [18].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4701 [6].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_cmb_05"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_F : CMonster::MONSTER_DAM_HEAD_LV02_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [16].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4701 [7].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_cmb_06"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_D : CMonster::MONSTER_DAM_BODY_LV02_D;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [16].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4701 [6].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_cmb_07"))
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [15].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);

			}
			else
			{
				m_pThis->Set_Down(true);
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DIRECT_B : CMonster::MONSTER_DWN_DIRECT_B;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DIRECT_F : CMonster::MONSTER_DWN_DIRECT_F;

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [19].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4702 [4].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_cmb_08"))
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [17].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				m_pThis->Set_Down(true);
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F_SP;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B_SP;

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [21].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4702 [3].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);

		}

		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krh_cmb_01_fin")
		|| Find_PlayerCurrentAnimationName("p_krh_cmb_02_fin")
		|| Find_PlayerCurrentAnimationName("p_krh_cmb_03_fin")
		|| Find_PlayerCurrentAnimationName("p_krh_cmb_05_fin")
		|| Find_PlayerCurrentAnimationName("p_krh_cmb_05_fin")
		|| Find_PlayerCurrentAnimationName("p_krh_cmb_06_fin")
		|| Find_PlayerCurrentAnimationName("p_krh_cmb_07_fin")
		|| Find_PlayerCurrentAnimationName("p_krh_cmb_08_fin"))
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [15].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				m_pThis->Set_Down(true);
				if (!isBehine)
					*m_pState = CMonster::MONSTER_DWN_BODY_F_SP;
				else
					*m_pState = CMonster::MONSTER_DWN_BODY_B_SP;
				
				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [22].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4702 [4].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	return iDir;
}

_uint CAI_Monster::Check_KRS(_uint iPlayerLv, _bool isBehine, _bool isAnimChange)
{
	_uint iDir = PLAYER_ATK_DIR_END;
	//ATK_Counter �Ѿ����� ����
	if (Find_PlayerCurrentAnimationName("p_krs_atk_down_trample"))
	{
		if (isAnimChange)
		{
			*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_DNF_BOUND : CMonster::MONSTER_DWN_DNF_BOUND_G;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [22].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}


			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("471c [1].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}
			

		iDir = F;
	}

	//ATK_Kick_f
	if (Find_PlayerCurrentAnimationName("p_krs_atk_kick_f"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_L : CMonster::MONSTER_DAM_HEAD_LV01_L;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV01_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [14].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4718 [3].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	//ATK_Kick_f_2
	if (Find_PlayerCurrentAnimationName("p_krs_atk_kick_f_02"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_F : CMonster::MONSTER_DAM_HEAD_LV02_F;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;
			
			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [15].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}


			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4718 [2].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	//ATK_Kick_f_3
	if (Find_PlayerCurrentAnimationName("p_krs_atk_kick_f_03"))
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [16].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B;

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [22].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4701 [7].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	//ATK_Run_Heavy  // ������ ��������
	if (Find_PlayerCurrentAnimationName("p_krs_atk_run_heavy"))
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [17].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_EXPLODE_F : CMonster::MONSTER_DWN_EXPLODE_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_EXPLODE_B : CMonster::MONSTER_DWN_EXPLODE_B;

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [22].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4713 [1].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	//TAK_Tame_LOOP
	//if (Find_PlayerCurrentAnimationName("p_krs_atk_tame_en"))
	//	*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_PlayerCurrentAnimationName("p_krs_cmb_01") || Find_PlayerCurrentAnimationName("p_wpc_cmb_01"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_L : CMonster::MONSTER_DAM_HEAD_LV02_L;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [14].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}


			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4701 [6].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = L;
	}

	if (Find_PlayerCurrentAnimationName("p_krs_cmb_02") || Find_PlayerCurrentAnimationName("p_wpc_cmb_02"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_R : CMonster::MONSTER_DAM_HEAD_LV02_R;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [15].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4701 [6].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = R;
	}

	if (Find_PlayerCurrentAnimationName("p_krs_cmb_03") )
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_L : CMonster::MONSTER_DAM_HEAD_LV02_L;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [14].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}


			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4717 [2].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = L;
	}

	if (Find_PlayerCurrentAnimationName("p_krs_cmb_04"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_R : CMonster::MONSTER_DAM_HEAD_LV02_R;
			else
				*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_HEAD_LV01_B : CMonster::MONSTER_DAM_HEAD_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [15].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}


			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4718 [2].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = R;
	}

	if (Find_PlayerCurrentAnimationName("p_krs_cmb_01_fin")
		|| Find_PlayerCurrentAnimationName("p_krs_cmb_02_fin")
		|| Find_PlayerCurrentAnimationName("p_krs_cmb_03_fin")
		|| Find_PlayerCurrentAnimationName("p_krs_cmb_04_fin")
		|| Find_PlayerCurrentAnimationName("p_krs_sync_cmb_03_fin")
		|| Find_PlayerCurrentAnimationName("p_wpc_cmb_03"))
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [16].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F : CMonster::MONSTER_DWN_BODY_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B : CMonster::MONSTER_DWN_BODY_B;

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [20].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4702 [4].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krs_cmb_04_finw"))
	{
		//������
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_F : CMonster::MONSTER_DAM_BODY_LV02_F;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DAM_BODY_LV01_B : CMonster::MONSTER_DAM_BODY_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [17].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_F_SP : CMonster::MONSTER_DWN_BODY_F_SP;
				else
					*m_pState = iPlayerLv == 0 ? CMonster::MONSTER_DWN_BODY_B_SP : CMonster::MONSTER_DWN_BODY_B_SP;

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [21].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4702 [3].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	if
		(*m_pState == CMonster::MONSTER_DWN_BODY_F ||
			*m_pState == CMonster::MONSTER_DWN_BODY_B ||
			*m_pState == CMonster::MONSTER_DWN_BODY_F_SP ||
			*m_pState == CMonster::MONSTER_DWN_BODY_B_SP ||
			*m_pState == CMonster::MONSTER_DWN_EXPLODE_F || 
			*m_pState == CMonster::MONSTER_DWN_EXPLODE_B )
	{
		m_pThis->Set_Down(true);
	}


	return iDir;
}

_uint CAI_Monster::Check_KRC(_uint iPlayerLv, _bool isBehine, _bool isAnimChange)
{
	_uint iDir = PLAYER_ATK_DIR_END;

	if (Find_PlayerCurrentAnimationName("p_krc_atk_down_trample"))
	{
		if (isAnimChange)
		{
			*m_pState = CMonster::MONSTER_DWN_DNF_BOUND_G;
			m_pThis->Set_Down(true);

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [22].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4718 [2].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}
			
		iDir = F;
	}

	//���� �� �ݰ�
	if (Find_PlayerCurrentAnimationName("p_krc_atk_guard_heavy"))
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_F;
				else
					*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [18].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = CMonster::MONSTER_DWN_DIRECT_F_BOUND_G;
				else
					*m_pState = CMonster::MONSTER_DWN_DIRECT_B_BOUND_G;

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [20].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

				m_pThis->Set_Down(true);
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4717 [2].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krc_atk_guard_tame"))
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_F;
				else
					*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [19].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = CMonster::MONSTER_DWN_DIRECT_F_BOUND_G;
				else
					*m_pState = CMonster::MONSTER_DWN_DIRECT_B_BOUND_G;

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [21].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

				m_pThis->Set_Down(true);
			}
			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4718 [3].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}
		iDir = F;
	}

	if (Find_PlayerCurrentAnimationName("p_krc_atk_tackle_f"))
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_F;
				else
					*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [19].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = CMonster::MONSTER_DWN_DIRECT_F_BOUND_G;
				else
					*m_pState = CMonster::MONSTER_DWN_DIRECT_B_BOUND_G;

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [22].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

				m_pThis->Set_Down(true);
			}
			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4717 [1].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	//if (Find_PlayerCurrentAnimationName("p_krc_atk_tame_tackle_en"))
	//	*m_pState = iPlayerLv == 1 ? CMonster::MONSTER_ATK_DOWN : CMonster::MONSTER_ATK_DOWN;

	if (Find_PlayerCurrentAnimationName("p_krc_cmb_01")
		|| Find_PlayerCurrentAnimationName("p_krc_wpc_atk_pickup_l")
		|| Find_PlayerCurrentAnimationName("p_krc_wpc_atk_pickup_r")
		|| Find_PlayerCurrentAnimationName("p_krc_wpc_cmb_01"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_L;
			else
				*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [19].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}


			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4714 [8].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = L;
	}

	if (Find_PlayerCurrentAnimationName("p_krc_cmb_02")
		|| Find_PlayerCurrentAnimationName("p_krc_wpc_cmb_01_pickup_l")
		|| Find_PlayerCurrentAnimationName("p_krc_wpc_cmb_01_pickup_r")
		|| Find_PlayerCurrentAnimationName("p_krc_wpc_cmb_02"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_R;
			else
				*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [18].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}


			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4714 [8].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = R;
	}

	if (Find_PlayerCurrentAnimationName("p_krc_cmb_03")
		|| Find_PlayerCurrentAnimationName("p_krc_wpc_cmb_02_pickup_l")
		|| Find_PlayerCurrentAnimationName("p_krc_wpc_cmb_02_pickup_r"))
	{
		if (isAnimChange)
		{
			if (!isBehine)
				*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_B;

			if (m_iMonsterType != CMonster::KUZE)
			{
				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [19].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}


			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("4717 [1].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = B;
	}

	if (Find_PlayerCurrentAnimationName("p_krc_cmb_01_fin")
		|| Find_PlayerCurrentAnimationName("p_krc_cmb_02_fin")
		|| Find_PlayerCurrentAnimationName("p_krc_cmb_03_fin")
		|| Find_PlayerCurrentAnimationName("p_krc_wpc_cmb_03_pickup_l")
		|| Find_PlayerCurrentAnimationName("p_krc_wpc_cmb_03_pickup_r")
		|| Find_PlayerCurrentAnimationName("p_krc_wpc_cmb_03"))
	{
		if (isAnimChange)
		{
			if (m_iMonsterType == CMonster::SHAKDDOWN)
			{
				if (!isBehine)
					*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_F;
				else
					*m_pState = CMonster::MONSTER_DAM_HEAD_LV02_B;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [18].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
			}
			else
			{
				if (!isBehine)
					*m_pState = CMonster::MONSTER_DWN_BODY_F_SP;
				else
					*m_pState = CMonster::MONSTER_DWN_BODY_B_SP;

				if (m_iMonsterType != CMonster::KUZE)
				{
					m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
					m_pGameInstance->PlaySound_W(TEXT("46a0 [21].wav"), SOUND_ENEMY_VOICE, m_fSoundVoice);
				}

				m_pThis->Set_Down(true);
			}

			m_pGameInstance->StopSound(SOUND_ENEMY_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("47ce [5].wav"), SOUND_ENEMY_EFFECT, m_fSoundEffect);
		}

		iDir = F;
	}

	return iDir;
}

_bool CAI_Monster::Check_StandUp()
{
	//���� �ٶ�
	_uint iDir = Get_DownDir();
	if (iDir == DIR_F)
	{
		*m_pState = *m_pState == CMonster::MONSTER_H23070_000_4 ? CMonster::MONSTER_DWN_STANDUP_HEAD_R_EN : CMonster::MONSTER_STANDUP_DNF_FAST;
		return false;
	}
	else if (iDir == DIR_B)
	{
		*m_pState = CMonster::MONSTER_STANDUP_DNB_FAST;
		return false;
	}
	else
	{
		m_pThis->Set_Down(false);
		return false;
	}

	//standUp ��
	if (*m_pState == CMonster::MONSTER_STANDUP_DNF_FAST
		|| *m_pState == CMonster::MONSTER_STANDUP_DNB_FAST)
		m_pThis->Set_Down(false);

	return true;
}

_uint CAI_Monster::Get_DownDir()
{
	if (*m_pState == CMonster::MONSTER_DWN_EXPLODE_F
		|| *m_pState == CMonster::MONSTER_DWN_BODY_F
		|| *m_pState == CMonster::MONSTER_DWN_BODY_F_SP
		|| *m_pState == CMonster::MONSTER_DWN_DIRECT_F_BOUND_G
		|| *m_pState == CMonster::MONSTER_DWN_DIRECT_F
		|| *m_pState == CMonster::MONSTER_DWN_DNF_BOUND
		|| *m_pState == CMonster::MONSTER_DWN_DNF_BOUND_G
		|| *m_pState == CMonster::MONSTER_KRC_SYNC1_GUARD_COUNTER_F
		|| *m_pState == CMonster::MONSTER_KRC_SYNC1_KAIHI_NAGE_B
		|| *m_pState == CMonster::MONSTER_KRC_SYNC1_LAPEL_ATK_HEAVY
		|| *m_pState == CMonster::MONSTER_KRC_SYNC1_LAPEL_ATK_PUNCH
		|| *m_pState == CMonster::MONSTER_KRC_SYNC1_LAPEL_NAGE
		|| *m_pState == CMonster::MONSTER_KRC_SYNC1_NECK_ATK_HEAVY
		|| *m_pState == CMonster::MONSTER_KRC_SYNC1_NECK_ATK_PUNCH
		|| *m_pState == CMonster::MONSTER_KRS_SYNC1_CMB_03_FIN
		|| *m_pState == CMonster::MONSTER_KRU_SYNC1_LAPEL_CMB_03
		|| *m_pState == CMonster::MONSTER_KRU_SYNC1_LAPEL_NAGE
		|| *m_pState == CMonster::MONSTER_H23320_000_4
		|| *m_pState == CMonster::MONSTER_H20021_000_2
		|| *m_pState == CMonster::MONSTER_H10111_000_1
		|| *m_pState == CMonster::MONSTER_H23060_000_4
		|| *m_pState == CMonster::MONSTER_H1500_000_1
		|| *m_pState == CMonster::MONSTER_H3261_000_4
		|| *m_pState == CMonster::MONSTER_H23020_000_4
		|| *m_pState == CMonster::MONSTER_H23070_000_4
		|| *m_pState == CMonster::MONSTER_H23250_000_2
		|| *m_pState == CMonster::MONSTER_A60300_000_2
		|| *m_pState == CMonster::MONSTER_A60330_000_2
		|| *m_pState == CMonster::MONSTER_A60350_000_2
		|| *m_pState == CMonster::MONSTER_KRU_SYNC1_LAPEL_ATK_PUNCH
		|| *m_pState == CMonster::MONSTER_KRU_SYNC1_NECK_ATK_KICK)
		return DIR_F;

	if (*m_pState == CMonster::MONSTER_DWN_DIRECT_B
		|| *m_pState == CMonster::MONSTER_DWN_DIRECT_B_BOUND_G
		|| *m_pState == CMonster::MONSTER_DWN_BODY_B
		|| *m_pState == CMonster::MONSTER_DWN_BODY_B_SP
		|| *m_pState == CMonster::MONSTER_DWN_DNB_BOUND_G
		|| *m_pState == CMonster::MONSTER_H23010_000_4
		|| *m_pState == CMonster::MONSTER_H1511_000_4
		|| *m_pState == CMonster::MONSTER_KRC_SYNC1_KAIHI_NAGE_F
		|| *m_pState == CMonster::MONSTER_KRC_SYNC1_NECK_ATK_PUNCH
		|| *m_pState == CMonster::MONSTER_KRC_SYNC1_NECK_NAGE
		|| *m_pState == CMonster::MONSTER_KRS_SYNC1_CMB_03_FIN_B
		|| *m_pState == CMonster::MONSTER_SYNC1_LEG_ATK_KICK
		|| *m_pState == CMonster::MONSTER_SYNC1_LEG_NAGE
		|| *m_pState == CMonster::MONSTER_H1540_000_1
		|| *m_pState == CMonster::MONSTER_KRU_SYNC1_NECK_NAGE)
		return DIR_B;

	return DIR_END;
}

CBTNode::NODE_STATE CAI_Monster::Check_Start()
{
	if (m_isStart)
		return CBTNode::SUCCESS;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Start()
{
	if (*m_pState == CMonster::MONSTER_BTLST && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
	{
		m_isStart = false;
		return CBTNode::SUCCESS;
	}
		
	*m_pState = CMonster::MONSTER_BTLST;

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::Chcek_Sync()
{
	if (!m_isSync)
	{
		m_isSycnSound = false;
		m_fSyncTime = 0.f;
		return CBTNode::FAIL;
	}

	//QTE���� sync�� �ű�
	if (*m_pState == CMonster::MONSTER_H23250_000_2
		|| *m_pState == CMonster::MONSTER_A60300_000_2
		|| *m_pState == CMonster::MONSTER_A60330_000_2
		|| *m_pState == CMonster::MONSTER_A60350_000_2)
	{
		if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			if (m_pThis->isQTEResult() == 1)
			{
				//����
				m_pThis->Set_Down(true);
			}
			else if (m_pThis->isQTEResult() == 2)
			{
				*m_pCurrentAnimType = CLandObject::DEFAULT;
				Reset_State();
			}

			m_isQTE = true;
			m_isSync = false;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (*m_pState == CMonster::MONSTER_KRU_SYNC1_LAPEL_NAGE)
	{
		if (!m_isSycnSound)
		{
			if (*(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 20.0)
			{
				m_isSycnSound = true;

				m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
				m_pGameInstance->PlaySound_W(TEXT("46a0 [22].wav"), SOUND_ENEMY_VOICE, 0.8f);
			}
		}

	}
		
	if (*m_pState == CMonster::MONSTER_SYNC1_LEG_LP ||
		*m_pState == CMonster::MONSTER_SYNC1_LEG_ST_B ||
		*m_pState == CMonster::MONSTER_SYNC1_LEG_ST_F ||
		*m_pState == CMonster::MONSTER_SYNC1_LEG_WALK)
	{
		if (m_fSyncDuration <= m_fSyncTime)
		{
			//Leg�� ���� ó��
			m_fSyncTime = 0.f;
			*m_pState = CMonster::MONSTER_SYNC1_LEG_OFF;
			return CBTNode::SUCCESS;
		}
	}

	if (*m_pState == CMonster::MONSTER_KRU_SYNC1_NECK_LP ||
		*m_pState == CMonster::MONSTER_KRU_SYNC1_NECK_RESIST ||
		*m_pState == CMonster::MONSTER_KRU_SYNC1_NECK_ST ||
		*m_pState == CMonster::MONSTER_KRU_SYNC1_NECK_WALK)
	{
		if (m_fSyncDuration <= m_fSyncTime)
		{
			//KRU_SYNC1_NECK�� ���� ó��
			m_fSyncTime = 0.f;
			*m_pState = CMonster::MONSTER_KRU_SYNC1_NECK_OFF;
			return CBTNode::SUCCESS;
		}
	}

	if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
	{
		Reset_State();
		m_isSync = false;
		return CBTNode::SUCCESS;
	}

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::Check_Down()
{
	if (m_iSkill == SKILL_DEAD)
		return CBTNode::RUNNING;

	if (m_pThis->isObjectDead() || m_pThis->isDown())
	{
		Reset_State();
		return CBTNode::SUCCESS;
	}

	*m_pCurrentAnimType = CMonster::DEFAULT;
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::StandUpAndDead()
{
	if (m_pThis->isObjectDead())
		return CBTNode::SUCCESS;

	if (m_pThis->isDown())
	{
		Reset_State();

		//�ٿ�Ǿ��ִ� �ִϸ��̼� �����ΰ�?
		if (*m_pState == CMonster::MONSTER_H23250_000_2
			|| *m_pState == CMonster::MONSTER_A60300_000_2
			|| *m_pState == CMonster::MONSTER_A60330_000_2
			|| *m_pState == CMonster::MONSTER_A60350_000_2
			|| *m_pState == CMonster::MONSTER_H23010_000_4
			|| *m_pState == CMonster::MONSTER_H23020_000_4)
			return CBTNode::SUCCESS;

		if (!m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
			return CBTNode::RUNNING;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::StandUp()
{
	*m_pCurrentAnimType = CMonster::DEFAULT;

	if (m_pThis->isObjectDead())
		return CBTNode::FAIL;

	if (Check_StandUp() == true)
		return CBTNode::SUCCESS;

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::Dead()
{
	*m_pCurrentAnimType = CMonster::DEFAULT;

	_uint iDir = { PLAYER_ATK_DIR_END };
	_uint iPlayerLevel = m_pPlayer->Get_CurrentHitLevel();
	_bool isBehine = this->isBehine();

	m_iSkill = SKILL_DEAD;

	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRH)
		iDir = Check_KRH(iPlayerLevel, isBehine);
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRS)
		iDir = Check_KRS(iPlayerLevel, isBehine);
	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRC)
		iDir = Check_KRC(iPlayerLevel, isBehine);

	if (iDir == PLAYER_ATK_DIR_END)
		return CBTNode::FAIL;

	if (!m_pThis->isDown())
	{
		if (iDir == F)
			*m_pState = CMonster::MONSTER_DED_F_2;
		else if (iDir == B)
			*m_pState = CMonster::MONSTER_DED_B_2;
		else if (iDir == L)
			*m_pState = CMonster::MONSTER_DED_L;
		else if (iDir == R)
			*m_pState = CMonster::MONSTER_DED_R;

		m_pGameInstance->PlaySound_W(TEXT("46a0 [24].wav"), SOUND_ENEMY_DEAD, 0.7f);
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Check_PlayerDown()
{
	if (m_isPlayerDownAtk && !m_pPlayer->isDown())
	{
		m_isPlayerDownAtk = false;

		if (m_iSkill == SKILL_DOWN)
			m_isAttack = false;


		return CBTNode::FAIL;
	}
	else if (m_pPlayer->isDown())
	{
		// �÷��̾ �ٿ�Ǿ��ִٸ� 
		if (DistanceFromPlayer() > 2.f || m_isPlayerDownAtk)
			return CBTNode::FAIL;

		//�÷��̾ �ٿ�Ǿ������� �ֿ켱������ ������ �Ѵ�.	
		if (m_iSkill != SKILL_DOWN)
			Reset_State();

		m_iSkill = SKILL_DOWN;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::ATK_Down()
{
	if (m_iSkill == SKILL_DOWN && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_ATK_DOWN && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;
			m_isPlayerDownAtk = true;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	m_isAttack = true;
	*m_pState = CMonster::MONSTER_ATK_DOWN;

	return CBTNode::SUCCESS;
}

//�÷��̾ �������϶� ���� �� �ֵ��� 
CBTNode::NODE_STATE CAI_Monster::Check_Sway()
{
	m_pThis->Set_Down(false);

	if (m_isGuard || m_iSkill == SKILL_HIT || m_isAttack || m_isPlayerDownAtk || m_iSkill == SKILL_QTE)
	{
		m_isSway = false;
		return CBTNode::FAIL;	
	}

	if (m_isSway)
	{
		if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isSway = false;
			return CBTNode::FAIL;
		}

		return CBTNode::RUNNING;
	}

	//�÷��̾ ���� �������ΰ�? && �÷��̾�� �浹���� �ʾҴ°�?
	if (m_pPlayer->isAttack() && !m_pThis->isColl())
	{
		//�÷��̾���� �Ÿ��� ������� �ִ� ���¿��߸� ��
		if (DistanceFromPlayer() >= m_fSwayDistance.x && DistanceFromPlayer() < m_fSwayDistance.y)
		{
			Reset_State();
			return CBTNode::SUCCESS;
		}
	}

	m_isSway = false;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Sway()
{
	_uint iPlayerAtkDir = PLAYER_ATK_DIR_END;
	_uint iPlayerLv = m_pPlayer->Get_CurrentHitLevel();

	//�÷��̾ ������ �ڿ� ������ �����̸� ���� �ʴ´�.
	if (isBehine())
		return CBTNode::FAIL;

	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRS)
		iPlayerAtkDir = Check_KRS(iPlayerLv, false);
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRH)
		iPlayerAtkDir = Check_KRH(iPlayerLv, false);
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRC)
		iPlayerAtkDir = Check_KRC(iPlayerLv, false);

	//����ó��
	if (iPlayerAtkDir == PLAYER_ATK_DIR_END)
		return CBTNode::FAIL;

	Reset_State();

	//������ ��� �ֱ�
	//F
	if (iPlayerAtkDir == F)
		*m_pState = CMonster::MONSTER_SWAY_B;
	//B
	if (iPlayerAtkDir == B)
		*m_pState = CMonster::MONSTER_SWAY_F;
	//L
	if (iPlayerAtkDir == L)
		*m_pState = CMonster::MONSTER_SWAY_R;
	//R
	if (iPlayerAtkDir == R)
		*m_pState = CMonster::MONSTER_SWAY_L;

	m_isSway = true;
	m_iSkill = SKILL_SWAY;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Check_Hit()
{

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::HitAndGuard()
{
	if (m_isGuard)
	{
		if (m_pThis->isColl())
		{
			m_pGameInstance->PlaySound_W(TEXT("000b [3].wav"), SOUND_ENEMY_HIT, m_fSoundEffect);
			//���� ���¿��� �浹�� �ߴٸ� ������ ����
			CEffect::EFFECT_DESC EffectDesc;

			EffectDesc.pWorldMatrix = m_pThis->Get_TransformCom()->Get_WorldFloat4x4();

			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_GuardBlink"), TEXT("Layer_Particle"), &EffectDesc);
			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_GuardParticle"), TEXT("Layer_Particle"), &EffectDesc);
			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_GuardSmoke"), TEXT("Layer_Particle"), &EffectDesc);
			//m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part0"), TEXT("Layer_Particle"), &EffectDesc);
			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Guard1_Distortion0"), TEXT("Layer_Particle"), &EffectDesc);

			m_fGuardAtkAcc += m_pThis->Get_HitDamage();

		}

		return CBTNode::SUCCESS;
	}

	if (m_pThis->isColl())
	{
		//�浹�Ǹ� �÷��̾� �������� �ƴ��� üũ�� Ǯ��
		Reset_State();

		//�������� ó���ϱ� (3 Ȯ���� ����)
		_uint iRandom = m_pGameInstance->Get_Random(0, 100);

		if (iRandom == 95 || iRandom == 40 || iRandom == 67)
			m_iSkill = SKILL_GUARD;
		else
			m_iSkill = SKILL_HIT;

		return CBTNode::SUCCESS;
	}
	else
	{
		//�浹���� ���� ���¿��� ��Ʈ ����� ������?
		if (m_iSkill == SKILL_HIT && !m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
			return CBTNode::RUNNING;
	}

	return CBTNode::FAIL;
}

//KRS: ���Ѵ�, KRH: ����, KRC: �ı���
CBTNode::NODE_STATE CAI_Monster::Hit()
{
	_uint iLevel = m_pPlayer->Get_CurrentHitLevel();

	_bool isBehine = this->isBehine();
	if (m_pPlayer->Get_BattleStyle() == CPlayer::KRS)
	{
		Check_KRS(iLevel, isBehine);
	}
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRH)
	{
		Check_KRH(iLevel, isBehine);
	}
	else if (m_pPlayer->Get_BattleStyle() == CPlayer::KRC)
	{
		Check_KRC(iLevel, isBehine);
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Guard()
{
	if (m_iSkill != SKILL_GUARD)
		return CBTNode::FAIL;

	if (m_isGuard)
	{
		if (*m_pState == CMonster::MONSTER_GURAD_START && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			//���� ���ӽð� ��
			*m_pState = CMonster::MONSTER_GURAD_LOOP;
		}

		//���� �� �������� ���� ������ �Ѱ� �ȴٸ�
		if (m_fGuardBroken <= m_fGuardAtkAcc)
		{
			*m_pState = CMonster::MONSTER_GURAD_FLOAT;
		}

		//���� �ð��� �����ٸ�
		if (*m_pState == CMonster::MONSTER_GURAD_LOOP && m_fGuardDuration <= m_fGuardTime)
		{
			*m_pState = CMonster::MONSTER_GURAD_END;
		}

		//���� ���� ������
		if ((*m_pState == CMonster::MONSTER_GURAD_END
			|| *m_pState == CMonster::MONSTER_GURAD_FLOAT)
			&& m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isGuard = false;
			m_fGuardTime = 0.f;
			m_fGuardAtkAcc = 0.f;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	//ù ���� ��
	if (m_isGuard == false && m_iSkill == SKILL_GUARD)
	{
		m_isGuard = true;
		m_iSkill = SKILL_GUARD;
		*m_pState = CMonster::MONSTER_GURAD_START;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Check_Angry()
{
	// �г���� ��ȯ �б�
	if (!m_isAngry)
	{
		//�÷��̾�� �浹������ �������� 30.f �̶��?
		if (m_pThis->Get_HitDamage() < 30.f)
		{
			// �г� ���·� �̵�
			return CBTNode::SUCCESS;
		}
		//���� ������ �̻��� �޾����� �г���

		return CBTNode::FAIL;
	}

	//ȭ�� ���ִ� ���¿��� �ִϸ��̼��� �������ΰ�?
	if (*m_pState == CMonster::MONSTER_ANGRY_START && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		return CBTNode::FAIL;
	else if (*m_pState == CMonster::MONSTER_ANGRY_START) return CBTNode::RUNNING;

	//�̹� ȭ�� �� �ִٸ� �ٷ� ���� ������� �̵�
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Angry()
{
	m_isAngry = true;
	*m_pState = CMonster::MONSTER_ANGRY_START;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::ATK_Angry_Punch()
{
	if (m_iSkill == SKILL_ANGRY_CHOP && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_ANGRY_CHOP && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_ANGRY_CHOP)
	{
		*m_pState = CMonster::MONSTER_ANGRY_CHOP;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::ATK_Angry_Kick()
{
	if (m_iSkill == SKILL_ANGRY_KICK && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_ANGRY_KICK && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_ANGRY_KICK)
	{
		*m_pState = CMonster::MONSTER_ANGRY_KICK;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Check_Attack()
{
	//�÷��̾ �ٿ���¸� �������� ����
	if (m_pPlayer->isDown())
		return CBTNode::FAIL;

	if (!m_isAttack)
	{
		if (m_fDelayAttackDuration > m_fAttackDelayTime)
			return CBTNode::FAIL;

		m_fAttackDelayTime = 0.f;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Check_Break()
{
	LookAtPlayer();

	if (m_isBreak == false) return CBTNode::SUCCESS;

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::ShiftAndIdle()
{
	static _uint iCount = rand() % 7 + 2;

	if (DistanceFromPlayer() > 3.f && DistanceFromPlayer() <= 6.f)
	{
		m_iSkill = SKILL_SHIFT;
	}
	else if (DistanceFromPlayer() > 6.f)
	{
		m_iSkill = SKILL_IDLE;
	}
	iCount++;

	if (iCount == 12)
		m_iSkill = SKILL_SHIFT;
	else
		m_iSkill = SKILL_IDLE;

	if (iCount >= 20)
		iCount = 0;

	m_fBreakDuration = m_pGameInstance->Get_Random(4.f, 7.f);

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Shift()
{
	if (m_iSkill != SKILL_SHIFT)
		return CBTNode::FAIL;

	if (DistanceFromPlayer() > 2.f)
	{
		*m_pState = CMonster::MONSTER_SHIFT_F;
	}
	else {
		_uint iIndex = m_pGameInstance->Get_Random(0, 3);

		if (iIndex == 0)
			*m_pState = CMonster::MONSTER_SHIFT_B;
		else if (iIndex == 1)
			*m_pState = CMonster::MONSTER_SHIFT_L;
		else if (iIndex == 2)
			*m_pState = CMonster::MONSTER_SHIFT_R;
		else // ����ó��
			*m_pState = CMonster::MONSTER_SHIFT_L;

		m_isBreak = true;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Idle()
{
	*m_pState = CMonster::MONSTER_IDLE;
	m_isBreak = true;

	return CBTNode::SUCCESS;
}

void CAI_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pRootNode);

	//if (m_isClone)
	//{
	//	Safe_Release(m_pThis);
	//	Safe_Release(m_pAnimCom);
	//	Safe_Release(m_pPlayer);
	//}
}