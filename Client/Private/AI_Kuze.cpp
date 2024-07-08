#include "AI_Kuze.h"

#include "GameInstance.h"

CAI_Kuze::CAI_Kuze()
	: CAI_Monster{}
{
}

CAI_Kuze::CAI_Kuze(const CAI_Kuze& rhs)
	: CAI_Monster{ rhs }
{
}

HRESULT CAI_Kuze::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Kuze::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	return S_OK;
}

void CAI_Kuze::Tick(const _float& fTimeDelta)
{
	this->Execute();
}

CBTNode::NODE_STATE CAI_Kuze::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Kuze::Ready_Tree()
{
}

CAI_Kuze* CAI_Kuze::Create()
{
	CAI_Kuze* pInstance = new CAI_Kuze();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Kuze::Clone(void* pArg)
{
	CAI_Kuze* pInstance = new CAI_Kuze(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Kuze::Free()
{
	__super::Free();
}
