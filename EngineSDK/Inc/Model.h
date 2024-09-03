#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODELTYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_PARTICLE, TYPE_END };

	struct ANIMATION_DESC
	{
		ANIMATION_DESC(_uint iAnimIndex, _bool isLoop)
			: iAnimIndex{ iAnimIndex }, isLoop{ isLoop } {}

		_uint iAnimIndex = { 0 };
		_bool isLoop = { false };
	};

	struct FOV_ANIMATION
	{
		string strAnimationName;
		_uint iKeyFrameIndex;
		_float fFov;
	};

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool isExported, _bool isTool);
	virtual HRESULT Initialize(void* pArg);

	HRESULT	Ready_Meshes();
	HRESULT	Ready_Materials(const _char* pModelFilePath);
	HRESULT	Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT	Ready_Animations();
	HRESULT	Ready_CameraAnimations(string& pBinFilePath);

	HRESULT Render(_uint iMeshIndex);

	//��ǻƮ ���̴� ��
	void Bind_BoneMatrices(_uint iNumMeshIndex);
	HRESULT Bind_Compute(class CComputeShader* pComputeShader, _uint iNumMeshIndex);

	//�Ϲ� ���̴� ��
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iNumMeshIndex, aiTextureType eTextureType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iNumMeshIndex);
	bool	Check_Exist_Material(_uint iNumMeshIndex, aiTextureType eTextureType);

	void Play_Animation(_float fTimeDelta, _bool isRoot = true, string strExcludeBoneName = "");
	void Play_Animation(_float fTimeDelta, class CAnim* pAnim, _bool isLoop = false, _int iAnimIndex = -1, _bool isRoot = true, string strExcludeBoneName = "");
	void Play_Animation(_float fTimeDelta, const ANIMATION_DESC& AnimDesc, _bool isRoot = true, string strExcludeBoneName = "");
	void Play_Animation_CutScene(_float fTimeDelta, class CAnim* pAnim = nullptr, _bool isLoop = false, _int iAnimIndex = -1, _bool isRoot = true, string strExcludeBoneName = "");
	void Play_Animation_Monster(_float fTimeDelta, class CAnim* pAnim, _bool isLoop, _bool isRoot = true);
	void Play_Animation_Separation(_float fTimeDelta, _uint iAnimIndex, class CAnim* pAnim, _bool isLoop, _int iAnimType, _float fChangeInterval = 0.f);

	// iDir 0: X, 1: y, 2:z ������ ȸ����Ų��.
	void Play_Animation_Rotation_SeparationBone(_float fTimeDelta, string strBoneName, _uint iDir, _float fRadian, _bool isRoot = true, _bool isLoop = false, class CAnim* pAnim = nullptr);

	void Set_AnimationIndex(const ANIMATION_DESC& AnimDesc, _double ChangeInterval = 0.0);
	_bool Set_AnimationIndex(_uint iAnimIndex, _double ChangeInterval = 0.0);
	_bool Set_AnimationIndex(_uint iAnimIndex, vector<class CAnimation*> Animations, _double ChangeInterval = 0.0);
	void Set_ChangeInterval(_double ChangeInterval) {
		m_ChangeInterval = ChangeInterval;
	};

	void Reset_Animation(const ANIMATION_DESC& AnimDesc);
	void Reset_Animation(_uint iAnimIndex);

	void Set_PreAnimations(vector<class CAnimation*> PreAnimation) { m_PreAnimations = PreAnimation; }

	// iAnimType������ -1�� �شٸ�, �и����� �ʰڴ�
	// isParentException ���� true�� �شٸ� �θ�� ����ϴ� ���� �ڽĵ鿡�� �и��� ����
	void Set_Separation_ParentBone(string strBoneName, _int iAnimType);
	// Ư�� ������ �ִϸ��̼� ���� ����
	void Set_Separation_SingleBone(string strBoneName, _int iAnimType);

public:
	_uint Get_NumMeshes() { return m_iNumMeshes; }
	_uint Get_NumAnimations() { return m_iAnimations; }
	_uint Get_NumBones() { return m_iNumBones; }
	_uint Get_CurrentAnimationIndex() { return m_AnimDesc.iAnimIndex; }
	_uint Get_PrevAnimationIndex() { return m_iPrevAnimIndex; }
	const char* Get_MaterialName(_uint iIndex) { return m_Materials[iIndex].strMaterialName; }
	const vector<_uint>* Get_CurrentKeyFrameIndices(string strAnimationName = "");
	const vector<_uint>* Get_CurrentKeyFrameIndices(_uint iAnimIndex);

	_float Get_FoV(string strAnimationName, _uint iKeyFrameIndex);

	_bool Get_AnimFinished() const;
	_bool Get_AnimFinished(_uint iIndex) const;
	_bool Get_AnimChanged() const;
	_bool Get_AnimRestart() const;
	_bool Get_AnimLerp() const;			//m_ChangeInterval ���� 0�̶�� ���������� ���� �ʴ� �ִϸ��̼��̴�

	_bool Get_AnimLoop(_uint iAnimIndex) { 
		if (m_AnimLoops.size() <= iAnimIndex) return false;			//�迭 ���� ����ó��

		return m_AnimLoops[iAnimIndex];
	}

	const vector<class CAnimation*>& Get_Animations() {
		return m_Animations;
	}

	const vector<class CBone*>& Get_Bones() {
		return m_Bones;
	}

	const vector<class CMesh*>& Get_Meshes() {
		return m_Meshes;
	}

	const _float4x4* Get_BoneCombinedTransformationMatrix(const _char* pBoneName) const;
	const _float4x4* Get_BoneCombinedTransformationMatrix_AtIndex(_uint iBoneIndex) const;
	const _float4x4* Get_BoneTransformationMatrix(const _char* pBoneName) const;

	const _char* Get_AnimationName(_uint iAnimIndex);
	const _double* Get_AnimationCurrentPosition(class CAnim* pAnim = nullptr, _int iAnimIndex = -1);
	const _double* Get_AnimationDuration(class CAnim* pAnim = nullptr, _int iAnimIndex = -1);
	const _float3* Get_AnimationCenterMove(class CAnim* pAnim = nullptr, _int iAnimIndex = -1);
	const _float4* Get_AnimationCenterRotation(class CAnim* pAnim = nullptr, _int iAnimIndex = -1);

	const _float3* Get_LastKeyframe_Position(string strChannelName = "center_c_n", class CAnim* pAnim = nullptr, _int iAnimIndex = -1);
	const _float3* Get_FirstKeyframe_Position(string strChannelName = "center_c_n", class CAnim* pAnim = nullptr, _int iAnimIndex = -1);
	const _float4* Get_LastKeyframe_Rotation(string strChannelName = "center_c_n", class CAnim* pAnim = nullptr, _int iAnimIndex = -1);
	const _float4* Get_FirstKeyframe_Rotation(string strChannelName = "center_c_n", class CAnim* pAnim = nullptr, _int iAnimIndex = -1);

	// ���� ����ü�� ����
	_float4x4		Get_LocalMatrix();

	_float3				Get_LocalModelSize() { return m_vModelLocalSize; }
	_float3				Get_MaxPoints() { return m_vMaxPoints; }
	_float3				Get_MinPoints() { return m_vMinPoints; }

public:
	void	Copy_DecalMaterial(vector<DECAL_DESC>* pDecals); /* Decal ���� ������ */
	CTexture* Copy_DecalTexture(int	iMaterialNum); /* Decal �ؽ�ó ������ */

public:
	void Set_AnimLoop(_uint iAnimIndex, _bool isLoop)
	{
		if (m_AnimLoops.size() <= iAnimIndex) return;

		m_AnimLoops[iAnimIndex] = isLoop;
	}

private:
	HRESULT Export_Model(string& pBinFilePath, const _char* pModelFilePath);
	HRESULT Export_Bones(ofstream& out);
	HRESULT Export_Meshes(ofstream& out, const string& strOutDirectory);
	HRESULT Export_Materials(ofstream& out, const _char* pModelFilePath, const string& strOutDirectory);
	HRESULT Export_Animations(ofstream& out);
	HRESULT Export_CamNums(ofstream& out);

	HRESULT Import_Model(string& pBinFilePath, _bool isTool);
	HRESULT Import_Bones(ifstream& in);
	HRESULT Import_Meshes(ifstream& in);
	HRESULT Import_Materials(ifstream& in, _bool isTool);
	HRESULT Import_Animations(ifstream& in);
	HRESULT Import_CamNums(ifstream& in);

private:
	void	Find_Mesh_Using_DECAL();
	void	Check_Separation_Parents(CBone* pBone, _int iAnimType);

private:
	MODELTYPE					m_eModelType = { TYPE_END };

	const aiScene*				m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;
	_float4x4					m_PreTransformMatrix;			
	_float4x4					m_MeshBoneMatrices[512];

	vector<_bool>				m_AnimLoops;

	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;

	_uint						m_iNumBones = { 0 };
	vector<class CBone*>		m_Bones;

	_uint						m_iAnimations = { 0 };
	ANIMATION_DESC				m_AnimDesc{ 0, false };
	_uint						m_iPrevAnimIndex = { 0 };
	vector<class CAnimation*>	m_Animations;
	vector<class CAnimation*>	m_PreAnimations;

	_uint						m_iCameras = { 0 };
	vector<FOV_ANIMATION>		m_FovAnimation;

	_bool						m_isAnimChange = { false };
	_float						m_fAnimRatio = { 0.2f };
	_double						m_ChangeInterval = { 0.2 };

	vector<DECAL_DESC>			m_vDecalMaterials;
	bool						m_bOrigin = { false };

	_bool						m_bSaveMaterial = { true };

	_float3						m_vModelLocalSize; // Occulusion culling�� ����
	_float3						m_vMaxPoints;
	_float3						m_vMinPoints;

private:
	_bool m_isTool = { false };


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool isExported, _bool isTool = false);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END