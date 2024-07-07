#include "MapCollider.h"

#include "GameInstance.h"

CMapCollider::CMapCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CMapCollider::CMapCollider(const CMapCollider& rhs)
    : CGameObject(rhs)
{
}

HRESULT CMapCollider::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMapCollider::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    if (FAILED(Add_Components(pArg)))
        return E_FAIL;

    m_WorldMatrix = XMMatrixIdentity();

    return S_OK;
}

void CMapCollider::Priority_Tick(const _float& fTimeDelta)
{
}

void CMapCollider::Tick(const _float& fTimeDelta)
{
    for (auto& iter : m_vCollider)
    {
        iter->Tick(m_WorldMatrix);
    }


}

void CMapCollider::Late_Tick(const _float& fTimeDelta)
{
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CMapCollider::Render()
{
//#ifdef _DEBUG
//    for (auto& iter : m_vCollider)
//    {
//        m_pGameInstance->Add_DebugComponent(iter);
//    }
//#endif

    return S_OK;
}

HRESULT CMapCollider::Add_Components(void* pArg)
{
    COLLIDER_IO* colliderDescIO = (COLLIDER_IO*)pArg;

    for (int i = 0; i < colliderDescIO->iColliderNum; i++)
    {
        COLLIDER_DESC       collderIOdesc = colliderDescIO->pColliderDesc[i];
        // Ω√¿€
        CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

        ColliderDesc.eType = CCollider::COLLIDER_OBB;
        ColliderDesc.vExtents = collderIOdesc.vExtents;
        ColliderDesc.vCenter = collderIOdesc.vCenter;
        ColliderDesc.vRotation = collderIOdesc.vQuaternion;

        m_vCollider.push_back(dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(LEVEL_TEST, TEXT("Prototype_Component_Collider"), &ColliderDesc)));
    }



    return S_OK;
}

HRESULT CMapCollider::Bind_ShaderResources()
{
    return S_OK;
}

CMapCollider* CMapCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapCollider* pInstance = new CMapCollider(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CMapCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapCollider::Clone(void* pArg)
{
    CMapCollider* pInstance = new CMapCollider(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CMapCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapCollider::Free()
{
    __super::Free();

    for (auto& iter : m_vCollider)
    {
        Safe_Release(iter);
    }

    m_vCollider.clear();
}
