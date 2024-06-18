#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	_bool Get_Finished() const {
		return m_isFinished;
	}

	_bool Get_Changed() const {
		return m_isChanged;
	}

public:
	HRESULT Initialize(const aiAnimation* pAnimation, const vector<class CBone*>& Bones);
	HRESULT Initialize(const BAiAnimation* pAnimation, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);
	void Update_Change_Animation(_float fTimeDelta, const vector<class CBone*>& Bones, CAnimation* pPrevAnimation, _double ChangeInterval);
	void Reset();

private:
	_char					m_szName[MAX_PATH] = { "" };
	_double					m_Duration = { 0.0 };
	_double					m_TickPerSecond = { 0.0 };
	_double					m_CurrentPosition = { 0.0 };
	_double					m_CurrentChangePosition = { 0.0 };
	_bool					m_isFinished = { false };
	_bool					m_isChanged = { true };

	_uint					m_iNumChannels = { 0 };
	vector<_uint>			m_CurrentKeyFrameIndices;
	vector<class CChannel*>	m_Channels;

public:
	static CAnimation* Create(const aiAnimation* pAnimation, const vector<class CBone*>& Bones);
	static CAnimation* Create(const BAiAnimation* pAnimation, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END