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
		// waypoint 방향받기
		// m_isTurn, m_iDir, m_isBack : 여기서 받음
		_vector vDir = m_pNavigationCom->Compute_WayPointDir_Adv(m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 
			fTimeDelta, &m_isTurn, &m_iDir, &m_isBack);
		m_pThis->Get_TransformCom()->LookAt_For_LandObject(vDir, true);
	}
}

CBTNode::NODE_STATE CAI_RoadWalker::Check_Coll()
{
	// 충돌된지 아닌지 -> 어깨빵
	// m_pThis : RoadWalker가 this(ai 뺸거)
	// this : 이 아이를 가지고 있는 애의 포인터 주소. 행동해야하는주체. AI_ROADWALKER 가지고있는애라고 보면됨
	// this : 행동 해야하는주체. 객체 -> player 같은 애
	if (m_pThis->isColl())
	{
		// 이게 성공해줄때
		// success : sequence 다음 노드로 넘어감
		// sequence 안에 2개를 가지고 있음 이게 성공하면 다음을 호출. 다음 객체를 호출(action)
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
		if (m_pAnimCom->Get_AnimFinished())
		{
			m_isBack = false;
			m_isTurn = false;
			m_isWalk = true;
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
		if (m_pAnimCom->Get_AnimFinished())
		{
			m_isTurn = false;
			m_isWalk = true;
		}

		return CBTNode::SUCCESS;
	}
	
	m_iSkill = SKILL_TURN;

	//턴 하는 방향 구하기
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

void CAI_RoadWalker::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRootNode);
}
