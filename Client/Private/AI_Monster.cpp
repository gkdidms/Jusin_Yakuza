#include "AI_Monster.h"

#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

CAI_Monster::CAI_Monster()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

CAI_Monster::CAI_Monster(const CAI_Monster& rhs)
	: m_pGameInstance { rhs.m_pGameInstance}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAI_Monster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Monster::Initialize(void* pArg)
{
	AI_MONSTER_DESC* pDesc = static_cast<AI_MONSTER_DESC*>(pArg);
	m_pState = pDesc->pState;
	m_pAnimCom = pDesc->pAnim;
	m_pThis = pDesc->pThis;

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	return S_OK;
}

void CAI_Monster::Tick(const _float& fTimeDelta)
{
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

	m_pThis->Get_TransformCom()->LookAt(vPlayerPos);
}

CBTNode::NODE_STATE CAI_Monster::Check_Death()
{
	if (*m_pState != CMonster::MONSTER_DEATH)
		return CBTNode::FAIL;

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::Check_Sway()
{
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Sway()
{
	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Chcek_Sync()
{
	if (!m_isSync)
		return CBTNode::SUCCESS;

	if (m_pAnimCom->Get_AnimFinished())
		return CBTNode::SUCCESS;

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::Sync_Neck()
{
	// �÷��̾ ����� ��� ����� �Ҷ�
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	// ������ ������ ��� ��Ʋ ��Ÿ���� ��� �����ϴ�.
	if (pPlayer->Get_BattleStyle() == CPlayer::KRH)
		return CBTNode::FAIL;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Monster::Check_Hit()
{
	//if (m_iSkill != SKILL_HIT)
	//	return CBTNode::SUCCESS;

	// ��Ʈ����� �ϰ� �ִ°�?


	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::HitAndGuard()
{
	//Hit üũ�ϰ� ���带 �� ������, Hit�� ������?
	//�浹�Ұ��ΰ�?
	return CBTNode::FAIL;
}

//KRS: ���Ѵ�, KRH: ����, KRC: �ı���
CBTNode::NODE_STATE CAI_Monster::Normal_Hit()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	if (pPlayer->Get_BattleStyle() == CPlayer::KRS)
	{
		//
	}
	else if (pPlayer->Get_BattleStyle() == CPlayer::KRH)
	{

	}
	else if (pPlayer->Get_BattleStyle() == CPlayer::KRC)
	{

	}

	return CBTNode::SUCCESS;
}

//KRS: ���Ѵ�, KRH: ����, KRC: �ı���
CBTNode::NODE_STATE CAI_Monster::Strong_Hit()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	if (pPlayer->Get_BattleStyle() == CPlayer::KRS)
	{
		//
	}
	else if (pPlayer->Get_BattleStyle() == CPlayer::KRH)
	{
		//
	}
	else if (pPlayer->Get_BattleStyle() == CPlayer::KRC)
	{

	}

	return CBTNode::SUCCESS();
}

CBTNode::NODE_STATE CAI_Monster::Guard()
{
	//����?

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Check_Angry()
{
	// �г���� ��ȯ �б�
	if (!m_isAngry)
	{
		_uint iRandom = m_pGameInstance->Get_Random(0, 100);

		//if (iRandom < 30.f)
		//{
		//	// �г� ���·� �̵�
		//	return CBTNode::SUCCESS;
		//}
		//���� ü�� �����϶�, �ǰ� �� �г� ������� �ƴ����� üũ�Ͽ� ���� �Լ����� �г� ���� ��ȯ�Ѵ�.

		return CBTNode::FAIL;
	}

	//ȭ�� ���ִ� ���¿��� �ִϸ��̼��� �������ΰ�?
	if (*m_pState == CMonster::MONSTER_ANGRY_START && m_pAnimCom->Get_AnimFinished())
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
		if (*m_pState == CMonster::MONSTER_ANGRY_CHOP && m_pAnimCom->Get_AnimFinished())
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
		if (*m_pState == CMonster::MONSTER_ANGRY_KICK && m_pAnimCom->Get_AnimFinished())
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

CBTNode::NODE_STATE CAI_Monster::Check_Break()
{
	if (m_isBreak == false) return CBTNode::SUCCESS;

	LookAtPlayer();

	return CBTNode::RUNNING;
}

CBTNode::NODE_STATE CAI_Monster::ShiftAndIdle()
{
	static _uint iCount = 0;

	iCount++;
	if (iCount == 5)
		m_iSkill = SKILL_SHIFT;
	else 
		m_iSkill = SKILL_IDLE;
	
	if (iCount <= 10)
		iCount = 0;

	m_fBreakDuration = m_pGameInstance->Get_Random(2.f, 4.f);
	
	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Monster::Shift()
{
	if (m_iSkill != SKILL_SHIFT)
		return CBTNode::FAIL;

	_uint iIndex = m_pGameInstance->Get_Random(0, 3);
	if (iIndex == 0)
		*m_pState = CMonster::MONSTER_SHIFT_F;
	else if (iIndex == 1)
		*m_pState = CMonster::MONSTER_SHIFT_B;
	else if (iIndex == 2)
		*m_pState = CMonster::MONSTER_SHIFT_L;
	else if (iIndex == 3)
		*m_pState = CMonster::MONSTER_SHIFT_R;

	m_isBreak = true;

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
}
