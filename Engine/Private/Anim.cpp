#include "Anim.h"

#include "Bone.h"
#include "Animation.h"

#include "GameInstance.h"

CAnim::CAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CAnim::CAnim(const CAnim& rhs)
	: CComponent { rhs },
	m_iAnimations { rhs.m_iAnimations}
{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.emplace_back(pPrototypeAnimation->Clone());
}

_uint CAnim::Get_AnimationIndex(const _char* pName)
{
	_uint iIndex = 0;
	string strName = "c_cm_t_suitn_mus_armature|" + string(pName);
	for (auto& pAnim : m_Animations)
	{
		if (!strcmp(pAnim->Get_AnimName(), strName.c_str()))
			return iIndex;

		iIndex++;
	}

	return -1;
}

void CAnim::Reset_Animation()
{
	m_Animations[m_iCurrentIndex]->Reset();
}

_bool CAnim::Get_AnimFinished()
{
	return m_Animations[m_iCurrentIndex]->Get_Finished();
}

_bool CAnim::Get_LoopAnimFinished()
{
	return m_Animations[m_iCurrentIndex]->Get_Restrat();
}

const _double* CAnim::Get_AnimDuration()
{
	return m_Animations[m_iCurrentIndex]->Get_Duration();
}

const _double* CAnim::Get_AnimPosition()
{
	return m_Animations[m_iCurrentIndex]->Get_CurrentPosition();
}

HRESULT CAnim::Initialize_Prototype(const _char* pModelFilePath, _bool isSave)
{
	if (isSave)
		Export_Animation(pModelFilePath);
	else
		Load_File(pModelFilePath);

	return S_OK;
}

HRESULT CAnim::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CAnim::Export_Animation(const _char* pModelFilePath)
{
	// FBX ��� ������ �����ϱ� 
	_uint		iFlag = aiProcess_ConvertToLeftHanded;

	iFlag |= aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_SortByPType;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	// �޽ø� ����� ���� ���� ���� �����ص־��Ѵ�.
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	m_iAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	if (FAILED(Save_File(pModelFilePath, m_Bones)))
		return E_FAIL;

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	return S_OK;
}

HRESULT CAnim::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	//�ݺ����� �������� ������ ��쿡 �θ� ���� �׻� vector�� ���� �������� ���Ե� ���̱⶧����
	// �θ��� ���� size - 1�� �ȴ�.
	_uint		iParent = m_Bones.size() - 1;

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
	{
		Ready_Bones(pAINode->mChildren[i], iParent);
	}

	return S_OK;
}

HRESULT CAnim::Save_File(const _char* pModelFilePath, vector<CBone*> Bones)
{
	string strFileName = m_pGameInstance->Get_FileName(pModelFilePath);
	string strFilePath = "../../Client/Bin/DataFiles/AnimationData/" + strFileName + ".dat";

	ofstream out(strFilePath, ios::binary);

	// �� ����
	// ReadyBones���� �����ϴ� �����͵鸸 �����Ѵ�.
	_int iNumBones = Bones.size();
	out.write((char*)&iNumBones, sizeof(_uint));

	for (auto& Bone : Bones)
	{
		string strValue = Bone->Get_Name();
		_int iValue = strValue.size();
		out.write((char*)&iValue, sizeof(_uint));
		out.write(strValue.c_str(), strValue.size());

		iValue = Bone->Get_ParentBoneIndex();
		out.write((char*)&iValue, sizeof(_int));

		_float4x4 TransformationMatrix = *(Bone->Get_TransformationMatrix());
		out.write((char*)&TransformationMatrix, sizeof(_float4x4));
	}

	//�ִϸ��̼� ����
	out.write((char*)&m_iAnimations, sizeof(_uint));

	for (size_t i = 0; i < m_iAnimations; i++)
	{
		aiAnimation* Animation = m_pAIScene->mAnimations[i];

		_uint iAnimNameLength = strlen(Animation->mName.data);
		out.write((char*)&iAnimNameLength, sizeof(_uint));
		out.write(Animation->mName.data, iAnimNameLength);

		out.write((char*)&Animation->mDuration, sizeof(_double));
		out.write((char*)&Animation->mTicksPerSecond, sizeof(_double));
		out.write((char*)&Animation->mNumChannels, sizeof(_uint));

		for (size_t j = 0; j < Animation->mNumChannels; j++)
		{
			aiNodeAnim* Channel = Animation->mChannels[j];

			_uint iChannelNameLength = strlen(Channel->mNodeName.data);
			out.write((char*)&iChannelNameLength, sizeof(_uint));
			out.write(Channel->mNodeName.data, iChannelNameLength);

			out.write((char*)&Channel->mNumScalingKeys, sizeof(_uint));
			out.write((char*)&Channel->mNumRotationKeys, sizeof(_uint));
			out.write((char*)&Channel->mNumPositionKeys, sizeof(_uint));

			_uint iNumKeyFrames = max(Channel->mNumScalingKeys, Channel->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, Channel->mNumPositionKeys);

			for (size_t k = 0; k < iNumKeyFrames; k++)
			{
				if (Channel->mNumScalingKeys > k)
				{
					out.write((char*)&Channel->mScalingKeys[k].mTime, sizeof(_double));
					out.write((char*)&Channel->mScalingKeys[k].mValue, sizeof(_float3));
				}

				if (Channel->mNumRotationKeys > k)
				{
					_float4 vRotationKeys;
					vRotationKeys.x = Channel->mRotationKeys[k].mValue.x;
					vRotationKeys.y = Channel->mRotationKeys[k].mValue.y;
					vRotationKeys.z = Channel->mRotationKeys[k].mValue.z;
					vRotationKeys.w = Channel->mRotationKeys[k].mValue.w;

					out.write((char*)&Channel->mRotationKeys[k].mTime, sizeof(_double));
					out.write((char*)&vRotationKeys, sizeof(_float4));
				}

				if (Channel->mNumPositionKeys > k)
				{
					out.write((char*)&Channel->mPositionKeys[k].mTime, sizeof(_double));
					out.write((char*)&Channel->mPositionKeys[k].mValue, sizeof(_float3));
				}

			}
		}
	}

	return S_OK;
}

HRESULT CAnim::Load_File(const _char* pModelFilePath)
{
	ifstream in(pModelFilePath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("���� ���� ����");
		return E_FAIL;
	}

	// �� �о����
	_int iNumBones = 0;
	in.read((char*)&iNumBones, sizeof(_uint));

	vector<CBone*> Bones;
	for (size_t i = 0; i < iNumBones; i++)
	{
		BAiNode BAINode{};

		_uint iStringSize = 0;
		in.read((char*)&iStringSize, sizeof(_uint));

		in.read(BAINode.mName, iStringSize);
		in.read((char*)&BAINode.mParentBoneIndex, sizeof(_int));
		in.read((char*)&BAINode.mTransformationMatrix, sizeof(_float4x4));

		Bones.emplace_back(CBone::Create(&BAINode));
	}

	//�ִϸ��̼� ����
	in.read((char*)&m_iAnimations, sizeof(_uint));

	for (size_t i = 0; i < m_iAnimations; i++)
	{
		BAiAnimation Animation{};

		_uint	iAnimNameLength = 0;

		in.read((char*)&iAnimNameLength, sizeof(_uint));
		in.read(Animation.mName, iAnimNameLength);

		in.read((char*)&Animation.mDuration, sizeof(_double));
		in.read((char*)&Animation.mTicksPerSecond, sizeof(_double));
		in.read((char*)&Animation.mNumChannels, sizeof(_uint));

		Animation.mChannels = new BAiNodeAnim * [Animation.mNumChannels];

		for (size_t j = 0; j < Animation.mNumChannels; j++)
		{
			Animation.mChannels[j] = new BAiNodeAnim();

			_uint iChannelNameLength = 0;
			in.read((char*)&iChannelNameLength, sizeof(_uint));
			in.read(Animation.mChannels[j]->mName, iChannelNameLength);

			in.read((char*)&Animation.mChannels[j]->mNumScalingKeys, sizeof(_uint));
			in.read((char*)&Animation.mChannels[j]->mNumRotationKeys, sizeof(_uint));
			in.read((char*)&Animation.mChannels[j]->mNumPositionKeys, sizeof(_uint));

			_uint iNumKeyFrames = max(Animation.mChannels[j]->mNumScalingKeys, Animation.mChannels[j]->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, Animation.mChannels[j]->mNumPositionKeys);

			Animation.mChannels[j]->mScalingKeys = new VectorKey[iNumKeyFrames];
			Animation.mChannels[j]->mRotationKeys = new QuatKey[iNumKeyFrames];
			Animation.mChannels[j]->mPositionKeys = new VectorKey[iNumKeyFrames];

			for (size_t k = 0; k < iNumKeyFrames; k++)
			{
				if (Animation.mChannels[j]->mNumScalingKeys > k)
				{
					in.read((char*)&Animation.mChannels[j]->mScalingKeys[k].mTime, sizeof(_double));
					in.read((char*)&Animation.mChannels[j]->mScalingKeys[k].mValue, sizeof(_float3));
				}

				if (Animation.mChannels[j]->mNumRotationKeys > k)
				{
					in.read((char*)&Animation.mChannels[j]->mRotationKeys[k].mTime, sizeof(_double));
					in.read((char*)&Animation.mChannels[j]->mRotationKeys[k].mValue, sizeof(_float4));
				}

				if (Animation.mChannels[j]->mNumPositionKeys > k)
				{
					in.read((char*)&Animation.mChannels[j]->mPositionKeys[k].mTime, sizeof(_double));
					in.read((char*)&Animation.mChannels[j]->mPositionKeys[k].mValue, sizeof(_float3));
				}
			}
		}

		CAnimation* pAnimation = CAnimation::Create(&Animation, Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);

		for (size_t j = 0; j < Animation.mNumChannels; j++)
		{
			Safe_Delete_Array(Animation.mChannels[j]->mScalingKeys);
			Safe_Delete_Array(Animation.mChannels[j]->mRotationKeys);
			Safe_Delete_Array(Animation.mChannels[j]->mPositionKeys);

			Safe_Delete(Animation.mChannels[j]);
		}
		Safe_Delete_Array(Animation.mChannels);
	}

	for (auto& pBone : Bones)
		Safe_Release(pBone);

	return S_OK;
}

CAnim* CAnim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pConext, const _char* pModelFilePath, _bool isSave)
{
	CAnim* pInstance = new CAnim(pDevice, pConext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, isSave)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent* CAnim::Clone(void* pArg)
{
	CAnim* pInstance = new CAnim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAnim::Free()
{
	__super::Free();
}
