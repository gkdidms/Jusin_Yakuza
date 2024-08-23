#include "QteManager.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Yoneda.h"
#include "Player.h"

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
    Cancle_KeyFrame();
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
    m_strPlayingAnimName = m_strAnimationName;
    m_iSuccess = 0;
}

HRESULT Client::CQteManager::Ready_SlowKeyFrame()
{
    /* 요네다 관련 */
    QTE_DESC Desc{ };
    Desc.iAnimIndex = 3;
    Desc.iCamAnimIndex = 24;
    Desc.iStartKeyFrameIndex = 10; 
    Desc.iEndKeyFrameIndex = 22;
    Desc.iKeyFrames = Desc.iEndKeyFrameIndex - Desc.iStartKeyFrameIndex;
    Desc.iSuccesStartFrame = 23;
    Desc.iSuccesEndFrame= 181;
    Desc.iFailedStartFrame = 182;
    Desc.iFailedEndFrame = 291;

    m_QTEs.emplace("a60300", Desc);

    Desc.iAnimIndex = 5;
    Desc.iCamAnimIndex = 26;
    Desc.iStartKeyFrameIndex = 61;
    Desc.iEndKeyFrameIndex = 75;
    Desc.iKeyFrames = Desc.iEndKeyFrameIndex - Desc.iStartKeyFrameIndex;
    Desc.iSuccesStartFrame = 76;
    Desc.iSuccesEndFrame = 244;
    Desc.iFailedStartFrame = 245;
    Desc.iFailedEndFrame = 628;

    m_QTEs.emplace("a60330", Desc);

    Desc.iAnimIndex = 6;
    Desc.iCamAnimIndex = 27;
    Desc.iStartKeyFrameIndex = 120;
    Desc.iEndKeyFrameIndex = 140;
    Desc.iKeyFrames = Desc.iEndKeyFrameIndex - Desc.iStartKeyFrameIndex;
    Desc.iSuccesStartFrame = 286;
    Desc.iSuccesEndFrame = 350;
    Desc.iFailedStartFrame = 141;
    Desc.iFailedEndFrame = 285;

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

void Client::CQteManager::Skip_KeyFrame()
{
    auto AnimList = m_pPlayerAnimCom->Get_Animations();

    auto iter = m_QTEs.find(m_strPlayingAnimName);

    if (iter == m_QTEs.end()) return;
    QTE_DESC Desc = iter->second;

    switch (m_iSuccess)
    {
    case 1:         //성공
    {
        // 키류 모델 애니메이션의 키프레임을 옮겨준다.
        AnimList[Desc.iAnimIndex]->Set_CurrentPosition(_double(Desc.iSuccesStartFrame));

        // 키류 컷신 카메라 애니메이션의 키프레임을 옮겨준다.
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
        CModel* pPlayerCamModel = dynamic_cast<CModel*>(pPlayer->Get_Component(TEXT("Com_Model_Cam")));
        auto CamModelAnimList = pPlayerCamModel->Get_Animations();
        CamModelAnimList[Desc.iCamAnimIndex]->Set_CurrentPosition(Desc.iSuccesStartFrame);

        // 요네다의 애니메이션 키프레임을 옮겨준다.
        CYoneda* pYoneda = dynamic_cast<CYoneda*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Yoneda"), 0));
        CAnimation* YonedaAnimation = dynamic_cast<CAnim*>(pYoneda->Get_Component(TEXT("Com_SyncAnim")))->Get_CurrentAnimation();
        YonedaAnimation->Set_CurrentPosition(_double(Desc.iSuccesStartFrame));
        break;
    }
    case 2:         //실패
    {
        AnimList[Desc.iAnimIndex]->Set_CurrentPosition(_double(Desc.iFailedStartFrame));

        // 키류 컷신 카메라 애니메이션의 키프레임을 옮겨준다.
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
        CModel* pPlayerCamModel = dynamic_cast<CModel*>(pPlayer->Get_Component(TEXT("Com_Model_Cam")));
        auto CamModelAnimList = pPlayerCamModel->Get_Animations();
        CamModelAnimList[Desc.iCamAnimIndex]->Set_CurrentPosition(Desc.iFailedStartFrame);

        CYoneda* pYoneda = dynamic_cast<CYoneda*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Yoneda"), 0));
        CAnimation* YonedaAnimation = dynamic_cast<CAnim*>(pYoneda->Get_Component(TEXT("Com_SyncAnim")))->Get_CurrentAnimation();
        YonedaAnimation->Set_CurrentPosition(_double(Desc.iFailedStartFrame));
        break;
    }
    }
}

void CQteManager::Cancle_KeyFrame()
{
    if ("" == m_strPlayingAnimName) return;

    auto iter = m_QTEs.find(m_strPlayingAnimName);

    if (iter == m_QTEs.end()) return;
    QTE_DESC m_eCurrentQTE = iter->second;

    // 성공 키프레임과 실패 키프레임을 비교해서, 성공 키프레임이 앞쪽이라면 true, 실패 키프레임이 앞쪽이라면 false
    _bool isFront = m_eCurrentQTE.iSuccesStartFrame < m_eCurrentQTE.iFailedStartFrame;
    auto AnimList = m_pPlayerAnimCom->Get_Animations();

    switch (m_iSuccess)
    {
    case 1:
    {
        if (m_eCurrentQTE.iSuccesEndFrame <= *AnimList[m_eCurrentQTE.iAnimIndex]->Get_CurrentPosition())
        {
            m_strPlayingAnimName = "";
                 
            // 이 때 컷신 종료해줘야한다.
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
            pPlayer->Reset_CutSceneEvent();

            CYoneda* pYoneda = dynamic_cast<CYoneda*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Yoneda"), 0));
            pYoneda->Off_Sync();
        }
         
        break;
    }
    case 2:
{
        if (m_eCurrentQTE.iFailedEndFrame <= *AnimList[m_eCurrentQTE.iAnimIndex]->Get_CurrentPosition())
        {
            m_strPlayingAnimName = "";

            // 이 때 컷신 종료해줘야한다.
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
            pPlayer->Reset_CutSceneEvent();

            CYoneda* pYoneda = dynamic_cast<CYoneda*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Yoneda"), 0));
            pYoneda->Off_Sync();
        }

        break;
    }
    }


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
            Skip_KeyFrame();
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
            m_iSuccess = 2;
            m_isSlowing = false;
            ResetVariables();

            Skip_KeyFrame();
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

    Safe_Release(m_pPlayerAnimCom);
}
