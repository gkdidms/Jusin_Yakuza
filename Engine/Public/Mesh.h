#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	HRESULT Initialize(CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones);
	HRESULT Initialize(CModel::MODELTYPE eModelType, const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones);

	HRESULT Ready_Vertices_For_NonAnimMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_NonAnimMesh(const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_ParticleMesh(const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_AnimMesh(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);
	HRESULT Ready_Vertices_For_AnimMesh(const BAiMesh* pAIMesh, const vector<class CBone*>& Bones);

	void	Fill_Matrices(vector<class CBone*>& Bones, _float4x4* pMeshBoneMatrices);

public:
	_uint Get_MaterialIndex() { return m_iMaterialIndex; }
	_uint Get_NumBones() { return m_iNumBones; }

	VTXMESH* Get_Vertices() { return m_pVertices; }
	_uint* Get_Mesh_Indices() { return m_pIndices; }

	const _char* Get_Name() const {
		return m_szName;
	}

	const _bool& Get_AlphaApply() const {
		return m_isAlphaApply;
	}

	_float4x4	Get_LocalMatrix() { return m_localMatrix; }
	_float		Get_MeshScale() { return m_fScale; }

	_bool isCloth();
	_bool isSkin();
	_bool DisableRDRT();

public:
	void Set_AlphaApply(_bool isValue) { m_isAlphaApply = isValue; }

private:
	_char				m_szName[MAX_PATH] = "";

	_bool				m_isAlphaApply = { false };

	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumBones = { 0 }; /* 이 메시에 영향을 주는 뼈의 갯수 */

	vector<_uint>		m_BoneIndices;
	vector<_float4x4>	m_OffsetMatrices;

	_float4x4			m_localMatrix;
	_float				m_fScale;

	VTXMESH* m_pVertices = { nullptr };
	_uint* m_pIndices = { nullptr };
public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODELTYPE eModelType, const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END