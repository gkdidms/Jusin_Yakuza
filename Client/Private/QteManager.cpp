#include "QteManager.h"
#include "GameInstance.h"

Client::CQteManager::CQteManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

Client::CQteManager::CQteManager(const CQteManager& rhs)
    : CGameObject{ rhs }
{
}

HRESULT Client::CQteManager::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Client::CQteManager::Initialize(void* pArg)
{
    if (FAILED(Ready_SlowKeyFrame()))
        return E_FAIL;

    return S_OK;
}

void Client::CQteManager::Priority_Tick(const _float& fTimeDelta)
{
}

void Client::CQteManager::Tick(const _float& fTimeDelta)
{
    Check_QTE_Section();
    Slowing();
}

void Client::CQteManager::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT Client::CQteManager::Render()
{
    return S_OK;
}

void Client::CQteManager::Set_Animation(CAnim* pAnim, string strAnimName)
{
    m_pPlayerAnimCom = pAnim;
    m_strAnimationName = strAnimName;
    m_iSuccess = 0;
}

HRESULT Client::CQteManager::Ready_SlowKeyFrame()
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

void Client::CQteManager::ResetVariables()
{
    m_fSlowSpeed = QTE_TIME_SPEED_MAX;
    m_fOutTime = 0.f;
    m_fPassTime = 0.f;
    m_strAnimationName = "";
    m_isSlowing = false;

    m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 1);
    m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1);
}

void Client::CQteManager::Slowing()
{
    if (!m_isSlowing) return;

    m_fSlowSpeed -= m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

    if (m_fSlowSpeed <= QTE_TIME_SPEED)
    {
        m_fSlowSpeed = QTE_TIME_SPEED;
    }

    m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), m_fSlowSpeed);
    m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fSlowSpeed);

    auto it = m_QTEs.find(m_strAnimationName);
    if (it != m_QTEs.end()) {
        _uint animIndex = it->second.iAnimIndex;

        // 해당 애니메이션의 포지션을 가져와서 얼마나 경과되었는지 계산한다.
        // Get_AnimPosition 값이 iStartKeyFrameIndex 이후라는 확신이 있는 상황에서 Slowing를 실행시켜야한다.
        _double AnimPos = *m_pPlayerAnimCom->Get_AnimPosition(animIndex);

        _double ConvertAnimPos = AnimPos - it->second.iKeyFrames;
        _double PassRatio = ConvertAnimPos / it->second.iKeyFrames;

        if (Check_QTE())
        {
            m_iSuccess = 1;
            ResetVariables();
        }

        if (1.0 <= PassRatio)
        {
            m_iSuccess = 2;
            ResetVariables();
        }
    }
}

_bool Client::CQteManager::Check_QTE()
{
    return (m_pGameInstance->GetKeyState(DIK_Q) == TAP);
}

void Client::CQteManager::Check_QTE_Section()
{
    if (nullptr == m_pPlayerAnimCom) return;
    
    auto it = m_QTEs.find(m_strAnimationName);
    if (it != m_QTEs.end()) 
    {
        _uint animIndex = it->second.iAnimIndex;

        if (!m_isSlowing && it->second.iStartKeyFrameIndex < *m_pPlayerAnimCom->Get_AnimPosition(animIndex))
        {
            m_isSlowing = true;
        }
        if (m_isSlowing && it->second.iEndKeyFrameIndex < *m_pPlayerAnimCom->Get_AnimPosition(animIndex))
        {
            m_isSlowing = false;
            ResetVariables();
        }
    }

}

Client::CQteManager* Client::CQteManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CQteManager* pInstance = new CQteManager(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CQteManager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* Client::CQteManager::Clone(void* pArg)
{
    CQteManager* pInstance = new CQteManager(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CQteManager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Client::CQteManager::Free()
{
    __super::Free();
}
