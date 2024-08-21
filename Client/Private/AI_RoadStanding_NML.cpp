#include "AI_RoadStanding_NML.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

#include "Adventure.h"
#include "RoadStanding_NML.h"

CAI_RoadStanding_NML::CAI_RoadStanding_NML()
	: CAI_RoadStanding{}
{
}

CAI_RoadStanding_NML::CAI_RoadStanding_NML(const CAI_RoadStanding_NML& rhs)
	: CAI_RoadStanding{ rhs }
{
}

HRESULT CAI_RoadStanding_NML::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_RoadStanding_NML::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Root();

	*m_pState = m_pGameInstance->Get_Random((int)CAdventure::ADVENTURE_STAND_ST, (int)CAdventure::ADVENTURE_STAND);

	return S_OK;
}

void CAI_RoadStanding_NML::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//this->Execute();


	if (DistanceFromPlayer() < 30.f)
	{
		m_isGround = true;
		Execute_Anim();
	}
	else
	{
		if (true == m_isGround && m_pAnimCom->Get_AnimFinished())
		{
			*m_pState = CAdventure::ADVENTURE_STAND_ST;
			m_isGround = false;
		}
		
	}
}

CBTNode::NODE_STATE CAI_RoadStanding_NML::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_RoadStanding_NML::Ready_Root()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Coll
	CSequance* pCollSeq = CSequance::Create();
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadStanding_NML::Check_Coll, this)));
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadStanding_NML::Coll, this)));
#pragma endregion

#pragma region Walk
	CSequance* pStandSeq = CSequance::Create();
	pStandSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadStanding_NML::Check_Stand, this)));

	CSelector* pStandSelector = CSelector::Create();
	pStandSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadStanding_NML::Ground_The_Player, this)));
	pStandSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadStanding_NML::Stand, this)));
	pStandSeq->Add_Children(pStandSelector);
#pragma endregion

	pRoot->Add_Children(pCollSeq);
	pRoot->Add_Children(pStandSeq);

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_RoadStanding_NML::Stand()
{
	if (*m_pState == CAdventure::ADVENTURE_STAND)
		return CBTNode::SUCCESS;
	 
	if (*m_pState == CAdventure::ADVENTURE_STAND_ST && m_pAnimCom->Get_AnimFinished())
	{
		*m_pState = CAdventure::ADVENTURE_STAND;
	}

	*m_pState = CAdventure::ADVENTURE_STAND_ST;

	return CBTNode::SUCCESS;
}

void CAI_RoadStanding_NML::Execute_Anim()
{
	if (*m_pMotionType < (int)MOTION_TYPE::MOTION_TALK_A || *m_pMotionType >(int)MOTION_TYPE::MOTION_LISTEN)
	{
		_uint		iMotion = m_pGameInstance->Get_Random((int)MOTION_TYPE::MOTION_TALK_A, (int)MOTION_TYPE::MOTION_LISTEN);
		*m_pMotionType = iMotion;
	}


	if (m_pAnimCom->Get_AnimFinished())
	{
		if (CAdventure::ADVENTURE_STAND == *m_pState)
		{
			*m_pState = CAdventure::ADVENTURE_STAND_ST;
		}
		else
		{
			*m_pState = CAdventure::ADVENTURE_STAND;
		}

		_uint		iMotion = m_pGameInstance->Get_Random((int)MOTION_TYPE::MOTION_TALK_A, (int)MOTION_TYPE::MOTION_LISTEN);
		*m_pMotionType = iMotion;
	}

}

CAI_RoadStanding_NML* CAI_RoadStanding_NML::Create()
{
	CAI_RoadStanding_NML* pInstance = new CAI_RoadStanding_NML();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_RoadStanding_NML::Clone(void* pArg)
{
	CAI_RoadStanding_NML* pInstance = new CAI_RoadStanding_NML(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_RoadStanding_NML::Free()
{
	__super::Free();
}
