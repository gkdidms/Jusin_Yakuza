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

	_uint Get_MaterialIndex() { return m_iMaterialIndex; }
	_uint Get_NumBones() { return m_iNumBones; }

	const _char* Get_Name() const {
		return m_szName;
	}

	HRESULT Ready_Vertices_For_NonAnimMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_NonAnimMesh(const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_AnimMesh(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);
	HRESULT Ready_Vertices_For_AnimMesh(const BAiMesh* pAIMesh, const vector<class CBone*>& Bones);

	void	Fill_Matrices(vector<class CBone*>& Bones, _float4x4* pMeshBoneMatrices);

private:
	_char				m_szName[MAX_PATH] = "";

	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumBones = { 0 }; /* 이 메시에 영향을 주는 뼈의 갯수 */

	vector<_uint>		m_BoneIndices;
	vector<_float4x4>	m_OffsetMatrices;

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODELTYPE eModelType, const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END