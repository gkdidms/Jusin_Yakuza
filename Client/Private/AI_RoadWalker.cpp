#include "AI_RoadWalker.h"

#include "GameInstance.h"

#include "Player.h"
#include "Adventure.h"

CAI_RoadWalker::CAI_RoadWalker()
	: CBTNode{},
	m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CAI_RoadWalker::CAI_RoadWalker(const CAI_RoadWalker& rhs)
	: CBTNode{ rhs },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAI_RoadWalker::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_RoadWalker::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	AI_ADVENTURE_DESC* pDesc = static_cast<AI_ADVENTURE_DESC*>(pArg);
	m_pAnimCom = pDesc->pAnimCom;
	m_pThis = pDesc->pThis;
	m_pState = pDesc->pState;
	m_pAStartCom = pDesc->pAStartCom;
	m_pNavigationCom = pDesc->pNavigation;
	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	return S_OK;
}

void CAI_RoadWalker::Tick(const _float& fTimeDelta)
{
	if (m_isWalk)
	{
		// waypoint ����ޱ�
		// m_isTurn, m_iDir, m_isBack : ���⼭ ����
		_vector vDir = m_pNavigationCom->Compute_WayPointDir_Adv(m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_POSITION),
			fTimeDelta, &m_isTurn, &m_iDir, &m_isBack);
		m_pThis->Get_TransformCom()->LookAt_For_LandObject(vDir, true);

		m_vNextDir = vDir;

		if (m_isTurn || m_isBack)
			m_fTurnTimer = 0;
	}

	if (m_isTurn || m_isBack)
		m_fTurnTimer += fTimeDelta;
}

CBTNode::NODE_STATE CAI_RoadWalker::Check_Coll()
{
	// �浹���� �ƴ��� -> �����
	// m_pThis : RoadWalker�� this(ai �A��)
	// this : �� ���̸� ������ �ִ� ���� ������ �ּ�. �ൿ�ؾ��ϴ���ü. AI_ROADWALKER �������ִ¾ֶ�� �����
	// this : �ൿ �ؾ��ϴ���ü. ��ü -> player ���� ��
	if (m_pThis->isColl())
	{
		// �̰� �������ٶ�
		// success : sequence ���� ���� �Ѿ
		// sequence �ȿ� 2���� ������ ���� �̰� �����ϸ� ������ ȣ��. ���� ��ü�� ȣ��(action)
		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_RoadWalker::Coll()
{

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RoadWalker::Check_Walk()
{
	if (m_isBack || m_isTurn)
	{
		if (m_iSkill == SKILL_BACK || m_iSkill == SKILL_TURN)
			return CBTNode::SUCCESS;

		if (*m_pState == CAdventure::ADVENTURE_WALK_EN && m_pAnimCom->Get_AnimFinished())
			return CBTNode::SUCCESS;

		*m_pState = CAdventure::ADVENTURE_WALK_EN;
		m_isWalk = false;

		return CBTNode::RUNNING;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RoadWalker::Back()
{
	if (!m_isBack) return CBTNode::FAIL;

	if (m_iSkill == SKILL_BACK && *m_pState == CAdventure::ADVENTURE_TURN)
	{
		//if (m_pAnimCom->Get_AnimFinished())
		if (m_fTurnTimer > 1.5f)
		{
			m_isBack = false;
			m_isTurn = false;
			m_isWalk = true;
			m_pThis->Get_TransformCom()->LookAt_For_LandObject(m_vNextDir, true);
		}

		return CBTNode::SUCCESS;
	}

	m_iSkill = SKILL_BACK;
	*m_pState = CAdventure::ADVENTURE_TURN;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RoadWalker::Turn()
{
	if (!m_isTurn) return CBTNode::FAIL;


	if (m_iSkill == SKILL_TURN && (*m_pState == CAdventure::ADVENTURE_TURN90_L || *m_pState == CAdventure::ADVENTURE_TURN90_R))
	{
		//if (m_pAnimCom->Get_AnimFinished())
		if(m_fTurnTimer > 1.5f)
		{
			m_isTurn = false;
			m_isWalk = true;
			m_isBack = false;
			m_pThis->Get_TransformCom()->LookAt_For_LandObject(m_vNextDir, true);
			m_fTurnTimer = 0;
		}

		return CBTNode::SUCCESS;
	}
	
	m_iSkill = SKILL_TURN;

	//�� �ϴ� ���� ���ϱ�
	if (m_iDir == DIR_R)
		*m_pState = CAdventure::ADVENTURE_TURN90_R;
	else if (m_iDir == DIR_L)
		*m_pState = CAdventure::ADVENTURE_TURN90_L;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RoadWalker::Walk()
{
	if (m_iSkill == SKILL_WALK)
	{
		if (*m_pState == CAdventure::ADVENTURE_WALK_ST && m_pAnimCom->Get_AnimFinished())
			*m_pState = CAdventure::ADVENTURE_WALK;

		return CBTNode::RUNNING;
	}

	m_iSkill = SKILL_WALK;
	*m_pState = CAdventure::ADVENTURE_WALK_ST;

	return CBTNode::SUCCESS;
}

void CAI_RoadWalker::Rotate_Dir(XMVECTOR& currentDirection, XMVECTOR targetDirection, bool& bEnd)
{
	//currentDirection = XMVector3Normalize(currentDirection);
	//targetDirection = XMVector3Normalize(targetDirection);

	//// �� ���� ������ ���� ���
	//float cosAngle = XMVectorGetX(XMVector3Dot(currentDirection, targetDirection));
	//float angle = acosf(cosAngle);

	//// ���� ������ �����̸� ȸ�� ����
	//if (angle < XMConvertToRadians(1.0f)) {
	//	currentDirection = targetDirection;
	//	bEnd = true; // �̹� ��ǥ ����� ��ġ
	//}

	//// �ִ� ȸ�� ������ 1���� ����
	//float rotationAngle = XMConvertToRadians(1.0f);

	//// ȸ�� ���� ��� (�� ������ ����)
	//XMVECTOR rotationAxis = XMVector3Normalize(XMVector3Cross(currentDirection, targetDirection));

	//// ȸ�� ��� ����
	//XMMATRIX rotationMatrix = XMMatrixRotationAxis(rotationAxis, rotationAngle);

	//// ���� ���⿡ ȸ�� ����� �����Ͽ� ���ο� ���� ���� ���
	//currentDirection = XMVector3TransformNormal(currentDirection, rotationMatrix);

	//// ������ ��ǥ ����� ��ġ���� ������ FALSE ��ȯ
	//bEnd = false;
}

void CAI_RoadWalker::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRootNode);
}
