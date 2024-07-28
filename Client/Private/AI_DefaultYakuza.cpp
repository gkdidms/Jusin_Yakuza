#include "AI_DefaultYakuza.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

#include "Monster.h"

CAI_DefaultYakuza::CAI_DefaultYakuza()
	: CAI_Monster{}
{
}

CAI_DefaultYakuza::CAI_DefaultYakuza(const CAI_DefaultYakuza& rhs)
	: CAI_Monster{ rhs }
{
}

HRESULT CAI_DefaultYakuza::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_DefaultYakuza::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	m_fDelayAttackDuration = m_pGameInstance->Get_Random(6.f, 9.f);
	m_iMonsterType = CAI_Monster::WPH;

	return S_OK;
}

void CAI_DefaultYakuza::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_DefaultYakuza::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_DefaultYakuza::Ready_Tree()
{
}

CAI_DefaultYakuza* CAI_DefaultYakuza::Create()
{
	CAI_DefaultYakuza* pInstance = new CAI_DefaultYakuza();
	
	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_DefaultYakuza::Clone(void* pArg)
{
	CAI_DefaultYakuza* pInstance = new CAI_DefaultYakuza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_DefaultYakuza::Free()
{
	__super::Free();
}
