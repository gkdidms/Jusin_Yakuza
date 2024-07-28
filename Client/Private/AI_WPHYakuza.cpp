#include "AI_WPHYakuza.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

#include "Monster.h"

CAI_WPHYakuza::CAI_WPHYakuza()
    : CAI_Monster{}
{
}

CAI_WPHYakuza::CAI_WPHYakuza(const CAI_WPHYakuza& rhs)
    : CAI_Monster{ rhs }
{
}

HRESULT CAI_WPHYakuza::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAI_WPHYakuza::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Ready_Tree();

    m_fDelayAttackDuration = m_pGameInstance->Get_Random(6.f, 9.f);
    m_iMonsterType = CAI_Monster::WPH;

    return S_OK;
}

void CAI_WPHYakuza::Tick(const _float& fTimeDelta)
{
    __super::Tick(fTimeDelta);

    this->Execute();
}

CBTNode::NODE_STATE CAI_WPHYakuza::Execute()
{
    return m_pRootNode->Execute();
}

void CAI_WPHYakuza::Ready_Tree()
{
}

CAI_WPHYakuza* CAI_WPHYakuza::Create()
{
    CAI_WPHYakuza* pInstance = new CAI_WPHYakuza();

    if (FAILED(pInstance->Initialize_Prototype()))
        Safe_Release(pInstance);

    return pInstance;
}

CBTNode* CAI_WPHYakuza::Clone(void* pArg)
{
    CAI_WPHYakuza* pInstance = new CAI_WPHYakuza(*this);

    if (FAILED(pInstance->Initialize(pArg)))
        Safe_Release(pInstance);

    return pInstance;
}

void CAI_WPHYakuza::Free()
{
    __super::Free();
}
