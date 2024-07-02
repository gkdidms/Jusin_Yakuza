#pragma once
#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}

	int Get_ParentBoneIndex() {
		return m_iParentBoneIndex;
	}
	
	const _float4x4* Get_TransformationMatrix() const {
		return &m_TransformationMatrix;
	}

	const _float4x4* Get_CombinedTransformationMatrix() const {
		return &m_CombinedTransformationMatrix;
	}

public:
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	_bool Compare_Name(const _char* pName) {
		return !strcmp(m_szName, pName);
	}

public:
	HRESULT Initialize(const aiNode* pBoneNode, _int iParentIndex);
	HRESULT Initialize(const BAiNode* pBoneNode);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);

private:
	_char				m_szName[MAX_PATH] = "";
	_int				m_iParentBoneIndex = { 0 };
	_float4x4			m_TransformationMatrix, m_CombinedTransformationMatrix;

public:
	static CBone* Create(const aiNode* pBoneNode, _int iParentIndex);
	static CBone* Create(const BAiNode* pBoneNode);
	CBone* Clone();
	virtual void Free() override;
};

END