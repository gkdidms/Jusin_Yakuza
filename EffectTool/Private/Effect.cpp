
#include "Effect.h"


CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CBlendObject{pDevice,pContext}
{
}

CEffect::CEffect(const CEffect& rhs)
    :CBlendObject{rhs}
{
}

HRESULT CEffect::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    if (nullptr != pArg)
    {
        EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);
        m_eType = pDesc->eType;
        m_BufferInstance = pDesc->BufferInstance;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vStartPos));
    }

    return S_OK;
}

void CEffect::Priority_Tick(const _float& fTimeDelta)
{
}

void CEffect::Tick(const _float& fTimeDelta)
{
}

void CEffect::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CEffect::Render()
{
    return S_OK;
}

void CEffect::Free()
{
    __super::Free();
}
