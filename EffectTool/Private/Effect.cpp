
#include "Effect.h"
const _uint CEffect::iAction[CEffect::ACTION_END] = {
    0x00000001, /* 0000 0001 */
    0x00000002, /* 0000 0010 */
    //    0x00000004, /*0000 0100 */

};


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
        m_bDirInfluence = pDesc->bDir;
        m_vStartPos = pDesc->vStartPos;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vStartPos));
        m_ParticleTag = pDesc->ParticleTag;
        m_fStartTime = pDesc->fStartTime;
    }

    return S_OK;
}

void CEffect::Priority_Tick(const _float& fTimeDelta)
{
}

void CEffect::Tick(const _float& fTimeDelta)
{
    m_fCurTime += fTimeDelta;
    
    if(!m_BufferInstance.isLoop)
    {
        _float fTotalTime = m_fStartTime + m_BufferInstance.vLifeTime.y;
        if (m_fCurTime > fTotalTime)
            m_isDead = true;
    }
}

void CEffect::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CEffect::Render()
{
    return S_OK;
}

HRESULT CEffect::Edit_Action(ACTION iEditAction)
{
    _uint position = (_uint)iEditAction;

    _uint mask = 1 << position;

    m_iAction ^= mask;

    return S_OK;
}

void CEffect::Free()
{
    __super::Free();
}
