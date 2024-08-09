#include "QteManager.h"

CGameObject* CQteManager::Clone(void* pArg)
{
    return nullptr;
}

void CQteManager::Free()
{
}

CQteManager::CQteManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CQteManager::CQteManager(const CQteManager& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CQteManager::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CQteManager::Initialize(void* pArg)
{
    return S_OK;
}

void CQteManager::Priority_Tick(const _float& fTimeDelta)
{
}

void CQteManager::Tick(const _float& fTimeDelta)
{
}

void CQteManager::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CQteManager::Render()
{
    return S_OK;
}

HRESULT CQteManager::Add_Components()
{
    return S_OK;
}

HRESULT CQteManager::Bind_ResourceData()
{
    return S_OK;
}
