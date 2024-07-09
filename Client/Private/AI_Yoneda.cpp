#include "AI_Yoneda.h"

CAI_Yoneda::CAI_Yoneda()
	: CAI_Monster{}
{
}

CAI_Yoneda::CAI_Yoneda(const CAI_Yoneda& rhs)
	: CAI_Monster{ rhs }
{
}

HRESULT CAI_Yoneda::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Yoneda::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	return S_OK;
}

void CAI_Yoneda::Tick(const _float& fTimeDelta)
{
}

CBTNode::NODE_STATE CAI_Yoneda::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Yoneda::Ready_Tree()
{
}

CAI_Yoneda* CAI_Yoneda::Create()
{
	CAI_Yoneda* pInstance = new CAI_Yoneda();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Yoneda::Clone(void* pArg)
{
	CAI_Yoneda* pInstance = new CAI_Yoneda(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Yoneda::Free()
{
	__super::Free();
}
