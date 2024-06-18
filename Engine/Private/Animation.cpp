#include "Animation.h"

#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_Duration{ rhs.m_Duration }
	, m_TickPerSecond{ rhs.m_TickPerSecond }
	, m_CurrentPosition{ rhs.m_CurrentPosition }
	, m_isFinished{ rhs.m_isFinished }
	, m_iNumChannels{ rhs.m_iNumChannels }
	, m_CurrentKeyFrameIndices{ rhs.m_CurrentKeyFrameIndices }
	, m_Channels{ rhs.m_Channels }
	, m_CurrentChangePosition{rhs.m_CurrentChangePosition }
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAnimation, const vector<CBone*>& Bones)
{
	strcpy_s(m_szName, pAnimation->mName.data);

	m_Duration = pAnimation->mDuration;
	m_TickPerSecond = pAnimation->mTicksPerSecond;

	m_iNumChannels = pAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);
	
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAnimation->mChannels[i], Bones);

		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}


	return S_OK;
}

HRESULT CAnimation::Initialize(const BAiAnimation* pAnimation, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAnimation->mName);

	m_Duration = pAnimation->mDuration;
	m_TickPerSecond = pAnimation->mTicksPerSecond;

	m_iNumChannels = pAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAnimation->mChannels[i], Bones);

		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}


	return S_OK;
}

void CAnimation::Update_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop)
{
	m_CurrentPosition += m_TickPerSecond * fTimeDelta;

	if (m_CurrentPosition >= m_Duration)
	{
		m_CurrentPosition = 0.0;

		if(false == isLoop)
			m_isFinished = true;
	}

	if (false == m_isFinished)
	{
		_uint		iChannelIndex = { 0 };

		for (auto& pChannel : m_Channels)
		{
			pChannel->Update_TransformationMatrix(m_CurrentPosition, Bones, &m_CurrentKeyFrameIndices[iChannelIndex++]);
		}
	}
}

// ���� �Է¹��� �ִϸ��̼ǿ��� �����ų ���̴�
void CAnimation::Update_Change_Animation(_float fTimeDelta, const vector<class CBone*>& Bones, CAnimation* pPrevAnimation, _double ChangeInterval)
{
	if (ChangeInterval <= m_CurrentChangePosition)
		m_isChanged = true;

	m_CurrentChangePosition += m_TickPerSecond * fTimeDelta;

	_uint		iChannelIndex = { 0 };

	for(auto& pDstChannel : m_Channels)
	{
		//pSrcChannel�� ������ �����ϰ� �ִ� �ִϸ��̼��̴�
		for (auto& pSrcChannel : pPrevAnimation->m_Channels)
		{
			// ���� ������ �ִϸ��̼ǰ� ������ �����ϰ� �ִ� �ִϸ��̼��� �� �ε����� ���� ä���� �ִٸ�
			// ���������� �ʿ��� ��Ȳ�̴�.
			if (pDstChannel->Get_BoneIndex() == pSrcChannel->Get_BoneIndex())
			{
				pDstChannel->Update_TransformationMatrix(m_CurrentChangePosition, Bones, pSrcChannel, pPrevAnimation->m_CurrentKeyFrameIndices[iChannelIndex++], ChangeInterval, pPrevAnimation->Get_Finished());
			}
		}
	}
}

void CAnimation::Reset()
{
	m_CurrentPosition = 0.0;
	m_isFinished = false;
	m_CurrentChangePosition = 0.0;
	m_isChanged = false;
	ZeroMemory(&m_CurrentKeyFrameIndices.front(), sizeof(_uint) * m_iNumChannels);
}

CAnimation* CAnimation::Create(const aiAnimation* pAnimation, const vector<CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAnimation, Bones)))
	{
		MSG_BOX("Failed To Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Create(const BAiAnimation* pAnimation, const vector<class CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAnimation, Bones)))
	{
		MSG_BOX("Failed To Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
