#include "UICarchase.h"

#include"GameInstance.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"	

#include "CarChase_Monster.h"

CUICarchase::CUICarchase()
    :CUIScene{}
{
}

CUICarchase::CUICarchase(const CUICarchase& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUICarchase::Update_TargetMatrix(_uint iIndex, _matrix TargetMatrix, _float iHP)
{
    CARCCHASE_UI_DESC* pTarget = Find_TargetUI(iIndex);
    if (nullptr == pTarget)
        return E_FAIL;

    _vector vTargetPos = TargetMatrix.r[3];
    pTarget->TargetingUI->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vTargetPos);

    vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) - 60.f);
    pTarget->HPBarBackUI->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vTargetPos);
    pTarget->HPBarUI->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vTargetPos);
    pTarget->HPBarUI->Get_TransformCom()->Set_Scale(iHP, 6.f, 0.f);

    return S_OK;
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

    m_EventUI[AIM]->Tick(fTimeDelta);

    Move_Aim(fTimeDelta);

    for (auto& pTarget : m_Targets)
    {
        pTarget.second.HPBarBackUI->Tick(fTimeDelta);
        pTarget.second.HPBarUI->Tick(fTimeDelta);
        pTarget.second.TargetingUI->Tick(fTimeDelta);
    }

    return S_OK;
}

HRESULT CUICarchase::Late_Tick(const _float& fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_EventUI[AIM]->Late_Tick(fTimeDelta);

    for (auto& pTarget : m_Targets)
    {
        pTarget.second.HPBarBackUI->Late_Tick(fTimeDelta);
        pTarget.second.HPBarUI->Late_Tick(fTimeDelta);
        pTarget.second.TargetingUI->Late_Tick(fTimeDelta);
    }

    if (!m_isAnimFin)
        m_isAnimFin = true;

    Coll_Aim();

    return S_OK;
}

void CUICarchase::Action()
{
}

void CUICarchase::OverAction()
{
}

HRESULT CUICarchase::Add_TargetingRound(_uint iIndex, class CCarChase_Monster* pMonster)
{
    if (nullptr != Find_TargetUI(iIndex))
        return E_FAIL;

    CARCCHASE_UI_DESC Desc{};

    CUI_Object* pTargeting = dynamic_cast<CUI_Object*>(m_EventUI[TARGETING]->Clone(nullptr));
    if (nullptr == pTargeting)
        return E_FAIL;

    CUI_Object* pHpBarBack = dynamic_cast<CUI_Object*>(m_EventUI[HPBAR_BACK]->Clone(nullptr));
    if (nullptr == pHpBarBack)
        return E_FAIL;

    CUI_Object* pHpBar = dynamic_cast<CUI_Object*>(m_EventUI[HPBAR]->Clone(nullptr));
    if (nullptr == pHpBar)
        return E_FAIL;

    Desc.TargetingUI = pTargeting;
    Desc.HPBarBackUI = pHpBarBack;
    Desc.HPBarUI = pHpBar;
    Desc.pMonsterAddr = pMonster;

    m_Targets.emplace(iIndex, Desc);
    Safe_AddRef(pMonster);
    
    return S_OK;
}

void CUICarchase::Move_Aim(const _float& fTimeDelta)
{
    _vector vPos = m_EventUI[AIM]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

    if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
    {
        _float fY = XMVectorGetY(vPos);
        fY += fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetY(vPos, fY);
    }
    if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
    {
        _float fX = XMVectorGetX(vPos);
        fX += fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetX(vPos, fX);
    }
    if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
    {
        _float fX = XMVectorGetX(vPos);
        fX -= fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetX(vPos, fX);
    }
    if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
    {
        _float fY = XMVectorGetY(vPos);
        fY -= fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetY(vPos, fY);
    }

    m_EventUI[AIM]->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
}

CUICarchase::CARCCHASE_UI_DESC* CUICarchase::Find_TargetUI(_uint iIndex)
{
    auto pTarget = m_Targets.find(iIndex);
    
    if (pTarget == m_Targets.end())
        return nullptr;

    return &(pTarget->second);
}

//충돌나면?
void CUICarchase::Coll_Aim()
{
    if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
    {
        _float4 vAimPos;
        XMStoreFloat4(&vAimPos, m_EventUI[AIM]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
        _float3 vAimScale = m_EventUI[AIM]->Get_TransformCom()->Get_Scaled();
        //에임이 정확하게 들어와 있어야 함.
        for (auto& pTarget : m_Targets)
        {
            _float4 vTargetPos;
            XMStoreFloat4(&vTargetPos, pTarget.second.TargetingUI->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

            _float3 vTargetScale;
            vTargetScale = pTarget.second.TargetingUI->Get_TransformCom()->Get_Scaled();

            _float fAimMinX = vAimPos.x - vAimScale.x * 0.5f;
            _float fAimMaxX = vAimPos.x + vAimScale.x * 0.5f;

            _float fAimMinY = vAimPos.y - vAimScale.y * 0.5f;
            _float fAimMaxY = vAimPos.y + vAimScale.y * 0.5f;

            _float fTargetMinX = vTargetPos.x - vTargetScale.x * 0.5f;
            _float fTargetMaxX = vTargetPos.x + vTargetScale.x * 0.5f;

            _float fTargetMinY = vTargetPos.y - vTargetScale.y * 0.5f;
            _float fTargetMaxY = vTargetPos.y + vTargetScale.y * 0.5f;

            if (fAimMinX > fTargetMinX && fAimMaxX < fTargetMaxX
                && fAimMinY > fTargetMinY && fAimMaxY < fTargetMaxY)
            {
                //충돌
                pTarget.second.pMonsterAddr->Set_Coll();
                pTarget.second.TargetingUI->Show_UI();
            }
        }
    }
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

    for (auto& pTarget : m_Targets)
    {
        Safe_Release(pTarget.second.HPBarBackUI);
        Safe_Release(pTarget.second.HPBarUI);
        Safe_Release(pTarget.second.pMonsterAddr);
        Safe_Release(pTarget.second.TargetingUI);
    }
    m_Targets.clear();
    
}
