#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODELTYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

	struct ANIMATION_DESC
	{
		ANIMATION_DESC(_uint iAnimIndex, _bool isLoop)
			: iAnimIndex{ iAnimIndex }, isLoop{ isLoop } {}

		_uint iAnimIndex = { 0 };
		_bool isLoop = { false };
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

	HRESULT Render(_uint iMeshIndex);

	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iNumMeshIndex, aiTextureType eTextureType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iNumMeshIndex);

	void Play_Animation(_float fTimeDelta);
	void Set_AnimationIndex(const ANIMATION_DESC& AnimDesc, _double ChangeInterval = 0.0);
	void Set_AnimationIndex(_uint iAnimIndex, _double ChangeInterval = 0.0);
	void Reset_Animation(const ANIMATION_DESC& AnimDesc);

public:
	_uint Get_NumMeshes() { return m_iNumMeshes; }
	_uint Get_NumAnimations() { return m_iAnimations; }
	_uint Get_NumBones() { return m_iNumBones; }

	_bool Get_AnimFinished() const;
	_bool Get_AnimChanged() const;
	_bool Get_AnimRestart() const;

	_bool Get_AnimLoop(_uint iAnimIndex) { 
		if (m_AnimLoops.size() <= iAnimIndex) return false;			//배열 범위 예외처리

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

	void	Copy_DecalMaterial(vector<DECAL_DESC>* pDecals);
	CTexture* Copy_DecalTexture(int	iMaterialNum);

	const _char* Get_AnimationName(_uint iAnimIndex);
	const _double* Get_AnimationCurrentPosition();
	const _double* Get_AnimationDuration();

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

	HRESULT Import_Model(string& pBinFilePath, _bool isTool);
	HRESULT Import_Bones(ifstream& in);
	HRESULT Import_Meshes(ifstream& in);
	HRESULT Import_Materials(ifstream& in, _bool isTool);
	HRESULT Import_Animations(ifstream& in);

	void	Find_Mesh_Using_DECAL();

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

	_bool						m_isAnimChange = { false };
	_float						m_fAnimRatio = { 0.2f };
	_double						m_ChangeInterval = { 0.2 };


	vector<DECAL_DESC>			m_vDecalMaterials;
	bool						m_bOrigin = { false };

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool isExported, _bool isTool = false);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END