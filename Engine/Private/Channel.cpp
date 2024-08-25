#include "Channel.h"

#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	_uint iBoneIndex = { 0 };

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
		{
			if (true == pBone->Compare_Name(m_szName))
				return true;

			++iBoneIndex;

			return false;
		});

	m_iBoneIndex = iBoneIndex;

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	KEYFRAME			KeyFrame{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&KeyFrame.vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
		}
		if (pAIChannel->mNumRotationKeys > i)
		{
			KeyFrame.vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			KeyFrame.vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			KeyFrame.vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			KeyFrame.vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
		}
		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&KeyFrame.vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mPositionKeys[i].mTime;
		}

		m_KeyFrames.emplace_back(KeyFrame);
	}
	
	return S_OK;
}

HRESULT CChannel::Initialize(const BAiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIChannel->mName);

	_uint iBoneIndex = { 0 };

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
		{
			if (true == pBone->Compare_Name(m_szName))
				return true;

			++iBoneIndex;

			return false;
		});

	m_iBoneIndex = iBoneIndex;

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	KEYFRAME			KeyFrame{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&KeyFrame.vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
		}
		if (pAIChannel->mNumRotationKeys > i)
		{
			memcpy(&KeyFrame.vRotation, &pAIChannel->mRotationKeys[i].mValue, sizeof(_float4));
			KeyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
		}
		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&KeyFrame.vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mPositionKeys[i].mTime;
		}

		m_KeyFrames.emplace_back(KeyFrame);
	}

	return S_OK;
}

// �Ϲ� Ű�����Ӱ� �������� �Լ�
void CChannel::Update_TransformationMatrix(_double CurrentPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float3* fCenterMoveValue, _float4* fCenterRotationValue, _bool isRoot, string strExcludeBoneName)
{
	if (0.0 == CurrentPosition)
		*pCurrentKeyFrameIndex = 0;

	// 1. ������ Ű�����ӿ� ���� ������ ���� �۾�
	KEYFRAME LastKeyFrame = m_KeyFrames.back();

	_vector			vScale, vRotation, vTranslation;

	//vScale = XMLoadFloat3(&LastKeyFrame.vScale);
	//vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
	//vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);

	if (CurrentPosition >= LastKeyFrame.Time)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		

		if (!Bones[m_iBoneIndex]->Compare_Name("center_c_n"))
		{
			vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
			vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
		}
		else
		{
			//vRotation = XMVectorZero();
			vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);

			// ��Ʈ�ִϸ��̼��� ������
			if(isRoot)
				vTranslation = XMVectorSet(0, 0, XMVectorGetZ(XMLoadFloat3(&LastKeyFrame.vPosition)), 1);
			else
				vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);

			XMStoreFloat3(fCenterMoveValue, XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f));
			XMStoreFloat4(fCenterRotationValue, XMLoadFloat4(&LastKeyFrame.vRotation));
		}
	}
	else
	{
		// ������ �������� �� �޽ð� �������� ���׸� �����Ѵ�.
		while (CurrentPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].Time)
			++*pCurrentKeyFrameIndex;

		// (���� ��ġ - �� ���ʿ� �ִ� Ű�������� ��ġ) / (�� ������ Ű������ ��ġ - �� ���� Ű������ ��ġ)�� ���� Ratio�� ���Ѵ�. (���������� �� ����� �����̴�)
		_double fRatio = (CurrentPosition - m_KeyFrames[*pCurrentKeyFrameIndex].Time) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].Time - m_KeyFrames[*pCurrentKeyFrameIndex].Time);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale), fRatio);
		
		// �ִϸ��̼� �������� ��ü�� �����ϰ� ���� Ű�����Ӱ� �״��
		if (strExcludeBoneName != "" && Bones[m_iBoneIndex]->Compare_Name(strExcludeBoneName.c_str()))
		{
			vRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
			vTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), 1.f);
		}
		else if (!Bones[m_iBoneIndex]->Compare_Name("center_c_n"))
		{
			vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation), fRatio);
			vTranslation = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), 1.f), XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), 1.f), fRatio);
		}
		else
		{
			//vRotation = XMVectorZero();
			vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation), fRatio);

			if (isRoot)
				vTranslation = XMVectorSet(0, 0, XMVectorGetZ(XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), 1.f), XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), 1.f), fRatio)), 1);
			else
				vTranslation = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), 1.f), XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), 1.f), fRatio);

			//XMStoreFloat4(fCenterRotationValue, XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation), fRatio));
			XMStoreFloat4(fCenterRotationValue, XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation));
			XMStoreFloat3(fCenterMoveValue, XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), 1.f), XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), 1.f), fRatio));
		}
		
	}
	//XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), 1.f);
	//XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), 1.f);

	//_matrix		TransformationMatrix = XMMatrixIdentity();
	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

// �ִϸ��̼ǰ� ���������ϴ� �Լ�
void CChannel::Update_TransformationMatrix(_double CurrentPosition, const vector<class CBone*>& Bones, CChannel* pSrcChannel, _uint PrevKeyFrameIndex, _double ChangeInterval, _bool isFinished, _float3* fCenterMoveValue, _float4* fCenterRotationValue, _bool isRoot)
{
	_vector			vScale, vRotation, vTranslation;

	// (���������� �� ������ �� - ���� �ִϸ��̼� ���� ������) / ���͹�
	// fRatio�� ������ ������ ��������
	_double fRatio = (ChangeInterval - CurrentPosition) / ChangeInterval;

	if (0 > fRatio)
		return;

	if (PrevKeyFrameIndex > pSrcChannel->m_KeyFrames.size())
		return;

	KEYFRAME KeyFrame = pSrcChannel->m_KeyFrames[PrevKeyFrameIndex];

	if (isFinished)
		KeyFrame = pSrcChannel->Get_Last_KeyFrame();

	vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames.front().vScale), XMLoadFloat3(&KeyFrame.vScale), fRatio);
	
	// center_c_n ���� �ƴϸ� ������ �ϴ� �� �״�� �ϱ�
	if (!Bones[m_iBoneIndex]->Compare_Name("center_c_n"))
	{
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames.front().vRotation), XMLoadFloat4(&KeyFrame.vRotation), fRatio);
		vTranslation = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_KeyFrames.front().vPosition), 1.f), XMVectorSetW(XMLoadFloat3(&KeyFrame.vPosition), 1.f), fRatio);
	}
	else
	{
		// center_c_n ����� ȸ���� ���̱�
		//vRotation = XMVectorZero();
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames.front().vRotation), XMLoadFloat4(&KeyFrame.vRotation), fRatio);
		if (isRoot)
			vTranslation = XMVectorSet(0, 0, XMVectorGetZ(XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_KeyFrames.front().vPosition), 1.f), XMVectorSetW(XMLoadFloat3(&KeyFrame.vPosition), 1.f), fRatio)), 1);
		else
			vTranslation = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_KeyFrames.front().vPosition), 1.f), XMVectorSetW(XMLoadFloat3(&KeyFrame.vPosition), 1.f), fRatio);

		XMStoreFloat3(fCenterMoveValue, XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_KeyFrames.front().vPosition), 1.f), XMVectorSetW(XMLoadFloat3(&KeyFrame.vPosition), 1.f), fRatio));
		//XMStoreFloat4(fCenterRotationValue, XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames.front().vRotation), XMLoadFloat4(&KeyFrame.vRotation), fRatio));
		XMStoreFloat4(fCenterRotationValue, XMLoadFloat4(&m_KeyFrames.front().vRotation));
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Update_KeyFrame(_double CurrentPosition, _uint* pCurrentKeyFrameIndex)
{
	if (0.0 == CurrentPosition)
		*pCurrentKeyFrameIndex = 0;

	_double dMin = 999999;

	_uint i = 0;
	_uint index = 0;

	for (auto& keyFrame : m_KeyFrames)
	{
		if (abs(CurrentPosition - keyFrame.Time) < dMin)
		{
			index = i;
			dMin = abs(CurrentPosition - keyFrame.Time);
		}

		i++;
	}

	if (0 < index)
		index--;

	*pCurrentKeyFrameIndex = index;
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX("Failed To Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel* CChannel::Create(const BAiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX("Failed To Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	m_KeyFrames.clear();
}
