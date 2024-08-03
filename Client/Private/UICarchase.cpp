#include "UICarchase.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"	
#include"GameInstance.h"
CUICarchase::CUICarchase()
    :CUIScene{}
{
}

CUICarchase::CUICarchase(const CUICarchase& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUICarchase::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;
    return S_OK;
}

HRESULT CUICarchase::Tick(const _float& fTimeDelta)
{
    __super::Tick(fTimeDelta);

    for (auto& iter : m_EventUI)
        iter->Tick(fTimeDelta);


    return S_OK;
}

HRESULT CUICarchase::Late_Tick(const _float& fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    for (auto& iter : m_EventUI)
        iter->Late_Tick(fTimeDelta);

    if (!m_isAnimFin)
        m_isAnimFin = true;

    return S_OK;
}

void CUICarchase::Action()
{
}

void CUICarchase::OverAction()
{
}

HRESULT CUICarchase::Add_TargetingRound()
{
    
    return S_OK;
}

CUICarchase* CUICarchase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CUICarchase* pInstance = new CUICarchase();
    if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
    {
        MSG_BOX("Make Failed :CUICarchase");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CUICarchase::Free()
{
    __super::Free();
}
