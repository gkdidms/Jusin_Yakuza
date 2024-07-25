#include "AI_Adventure.h"

#include "GameInstance.h"

#include "Player.h"
#include "Adventure.h"

CAI_Adventure::CAI_Adventure()
	: CBTNode{},
	m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CAI_Adventure::CAI_Adventure(const CAI_Adventure& rhs)
	: CBTNode{ rhs },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAI_Adventure::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Adventure::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	AI_ADVENTURE_DESC* pDesc = static_cast<AI_ADVENTURE_DESC*>(pArg);
	m_pAnimCom = pDesc->pAnimCom;
	m_pThis = pDesc->pThis;
	m_pState = pDesc->pState;
	m_pAStartCom = pDesc->pAStartCom;
	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	return S_OK;
}

void CAI_Adventure::Tick(const _float& fTimeDelta)
{
}

_bool CAI_Adventure::isRouteMoveFinish()
{
	list<CCell*>& FunnelList = m_pAStartCom->Get_FunnelList();

	if (FunnelList.empty())
		return true;

	//웨이포인트가 아닌 선분의 중점을 기준으로 다가간다.
	_vector vCellCheckPoint = (FunnelList.front()->Get_Point(CCell::POINT_B) + FunnelList.front()->Get_Point(CCell::POINT_C)) * 0.5f;
	_vector	vDir = vCellCheckPoint - m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	_float fDist = XMVectorGetX(XMVector3Length(vDir));

	m_pThis->Get_TransformCom()->LookAt_For_LandObject(vCellCheckPoint);

	//거의 근접하게 다가갈경우 제거한다.
	if (0.1f >= fDist)
	{
		Safe_Release(FunnelList.front());
		FunnelList.pop_front();
	}

	if (FunnelList.size() <= 0)
		return true;

	return false;
}

CBTNode::NODE_STATE CAI_Adventure::Check_Coll()
{
	if (m_pThis->isColl())
	{

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Adventure::Coll()
{

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Adventure::Check_Walk()
{
	if (m_isWayPointFinish)
	{
		if (m_Routes.empty())
			m_Routes = dynamic_cast<CNavigation*>(m_pThis->Get_Component(TEXT("Com_Navigation")))->Get_RouteIndexs(0);

		m_pThis->Start_Root(m_Routes.front());
		m_Routes.erase(m_Routes.begin());
	}

	m_isWayPointFinish = isRouteMoveFinish();

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Adventure::Walk()
{
	if (m_iSkill == SKILL_WALK)
	{
		if (*m_pState == CAdventure::ADVENTURE_WALK_S && m_pAnimCom->Get_AnimFinished())
			*m_pState = CAdventure::ADVENTURE_WALK;

		return CBTNode::RUNNING;
	}

	m_iSkill = SKILL_WALK;
	*m_pState = CAdventure::ADVENTURE_WALK_S;

	return CBTNode::SUCCESS;
}

void CAI_Adventure::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRootNode);
}
