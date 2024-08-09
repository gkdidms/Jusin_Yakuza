#include "QteManager.h"
#include "GameInstance.h"

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
    if (FAILED(Ready_SlowKeyFrame()))
        return E_FAIL;

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

HRESULT CQteManager::Ready_SlowKeyFrame()
{
    /* 요네다 관련 */
    QTE_DESC Desc{ };
    Desc.iAnimIndex = 3;
    Desc.iStartKeyFrameIndex = 10; 
    Desc.iEndKeyFrameIndex = 22;
    Desc.iKeyFrames = Desc.iEndKeyFrameIndex - Desc.iStartKeyFrameIndex;
    m_QTEs.emplace("a60300", Desc);

    Desc.iAnimIndex = 5;
    Desc.iStartKeyFrameIndex = 61;
    Desc.iEndKeyFrameIndex = 75;
    Desc.iKeyFrames = Desc.iEndKeyFrameIndex - Desc.iStartKeyFrameIndex;
    m_QTEs.emplace("a60330", Desc);

    Desc.iAnimIndex = 6;
    Desc.iStartKeyFrameIndex = 120;
    Desc.iEndKeyFrameIndex = 140;
    Desc.iKeyFrames = Desc.iEndKeyFrameIndex - Desc.iStartKeyFrameIndex;
    m_QTEs.emplace("a60350", Desc);

    return S_OK;
}

void CQteManager::Slowing(CAnim* pAnimCom)
{
    _uint iAnimIndex = (*m_QTEs.find(m_strAnimationName)).second.iAnimIndex;

    vector<_uint> Indices = *pAnimCom->Get_CurrentKeyFrameIndices(iAnimIndex);

    _double AnimDuration = *pAnimCom->Get_AnimDuration(iAnimIndex);
    _double AnimPosition = *pAnimCom->Get_AnimPosition(iAnimIndex);

    //if (QTE_TIME <= m_fOutTimer)
    //{
    //    m_fSlowSpeed = 0.f;
    //    m_fOutTimer = 0.f;
    //    return;
    //}
    //
    //_float fTimeDelta = m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

    //m_fSlowSpeed += fTimeDelta;
    //m_fOutTimer += fTimeDelta;
    //_float fTimeSpeed = 1 - m_fSlowSpeed;

    //m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), fTimeSpeed);
    //m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), fTimeSpeed);
}
