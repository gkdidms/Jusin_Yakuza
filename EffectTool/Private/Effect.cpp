
#include "Effect.h"
#include "GameInstance.h"

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
        m_vStartPos = pDesc->vStartPos;
        m_ParticleTag = pDesc->ParticleTag;
        m_fStartTime = pDesc->fStartTime;
        m_vStartColor = pDesc->vStartColor;
        m_vEndColor = pDesc->vEndColor;
        m_iShaderPass = pDesc->iShaderPass;

       
    }

    return S_OK;
}

void CEffect::Priority_Tick(const _float& fTimeDelta)
{
}

void CEffect::Tick(const _float& fTimeDelta)
{
    switch (m_eType)
    {
    case Client::CEffect::TYPE_POINT:
        break;
    case Client::CEffect::TYPE_TRAIL:
    {
        if (m_pGameInstance->GetKeyState(DIK_LEFT) == HOLD)
            m_pTransformCom->Go_Left(fTimeDelta);
        if (m_pGameInstance->GetKeyState(DIK_RIGHT) == HOLD)
            m_pTransformCom->Go_Right(fTimeDelta);
        if (m_pGameInstance->GetKeyState(DIK_UP) == HOLD)
            m_pTransformCom->Go_Straight(fTimeDelta);
        if (m_pGameInstance->GetKeyState(DIK_DOWN) == HOLD)
            m_pTransformCom->Go_Backward(fTimeDelta);

    }
        break;
    case Client::CEffect::TYPE_END:
        break;
    default:
        break;
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

void* CEffect::Get_Instance()
{
    return nullptr;
}

void CEffect::Free()
{
    __super::Free();
}
