#include "Animation.h"

#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
    : m_Duration{ rhs.m_Duration },
    m_TickPerSecond { rhs.m_TickPerSecond },
    m_iNumChannels { rhs.m_iNumChannels },
    m_Channels { rhs.m_Channels },
    m_iCurrentPosition { rhs.m_iCurrentPosition },
    m_IsFinished { rhs.m_IsFinished },
    m_CurrentKeyFrameIndex { rhs.m_CurrentKeyFrameIndex }
{
    strcpy_s(m_szName, rhs.m_szName);
    for (auto& pChannel : m_Channels)
        Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const char* pName, _double Duration, _double TickPerSecond, _uint iNumChannels, vector<class CChannel*> Channels)
{
    strcpy_s(m_szName, pName);

    m_Duration = Duration;
    m_TickPerSecond = TickPerSecond;
    m_iNumChannels = iNumChannels;

    m_CurrentKeyFrameIndex.resize(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; ++i)
    {
        m_Channels.emplace_back(Channels[i]);
    }

    return S_OK;
}

void CAnimation::Update_TransformationMatrix(const _float& fTimeDelta, const vector<CBone*> Bones, _bool isLoop)
{
    m_iCurrentPosition += m_TickPerSecond * fTimeDelta;

    if (m_iCurrentPosition >= m_Duration)
    {
        //�ִϸ��̼� ��
        m_iCurrentPosition = 0.0;
        ZeroMemory(&m_CurrentKeyFrameIndex.front(), sizeof(_uint) * m_iNumChannels);

        if (!isLoop) m_IsFinished = true;
        else m_IsLoopFinished = true;
    }

    if (!m_IsFinished)
    {
        for (size_t i = 0; i < m_iNumChannels; ++i)
        {
            m_Channels[i]->Update_TransformationMatrix(m_iCurrentPosition, Bones, &m_CurrentKeyFrameIndex[i]);
        }
    }
}

void CAnimation::Linear_TransformationMatrix(const _float& fTimeDelta, const vector<CBone*> Bones, _bool isLoop)
{
    double LinearDuration = isLoop ? 8 : 5;
    m_iCurrentPosition += m_TickPerSecond * fTimeDelta;

    if (m_iCurrentPosition >= LinearDuration)
    {
        //�ִϸ��̼� ��
        m_iCurrentPosition = 0.0;
        m_IsFirst = false;
    }

    if (m_IsFirst)
    {
        for (size_t i = 0; i < m_iNumChannels; ++i)
        {
            m_Channels[i]->First_TransformationMatrix(m_iCurrentPosition, Bones, LinearDuration);
        }
    }
}

void CAnimation::Reset()
{
    m_iCurrentPosition = 0.0;
    m_IsFinished = false;
    m_IsLoopFinished = false;
    m_IsFirst = true;
    ZeroMemory(&m_CurrentKeyFrameIndex.front(), sizeof(_uint) * m_iNumChannels);
}

CAnimation* CAnimation::Clone()
{
    return new CAnimation(*this);
}

CAnimation* CAnimation::Create(const char* pName, _double Duration, _double TickPerSecond, _uint iNumChannels, vector<class CChannel*> Channels)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(pName, Duration, TickPerSecond, iNumChannels, Channels)))
    {
        MSG_BOX("Failed To Created : CAnimation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAnimation::Free()
{
    for (auto& iter : m_Channels)
        Safe_Release(iter);

    m_Channels.clear();
}
