#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CAnimModel final : public CGameObject
{
public:
	struct ANIMMODEL_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_matrix matWorldMatrix;
	};

private:
	CAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimModel(const CAnimModel& rhs);
	virtual ~CAnimModel() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual void Priority_Tick(const _float& fTimeDelta);
	virtual void Tick(const _float& fTimeDelta);
	virtual void Late_Tick(const _float& fTimeDelta);

	virtual HRESULT Render();

public:
	void Set_Position(_fvector vPosition);
	void Set_Scaled(_float x, _float y, _float z);
	void Set_Rotation(_uint iAxis, _float vRadian, _float fTimeDelta);

public:
	const vector<class CAnimation*>& Get_Animations();
	const vector<class CBone*>& Get_Bones();
	const vector<class CMesh*>& Get_Meshes();

public:
	void Change_Model(wstring strModelName);
	void Change_Animation(_uint iAnimIndex);
	void Select_Bone(_uint iBoneIndex);
	void Select_Mesh(_uint iMeshIndex) {
		m_iSelectedMeshIndex = iMeshIndex;
	}

	void Create_BoneCollider(_uint iType, _uint iIndex);

	void Set_Collider_Center(_uint iIndex, const _float3& vCenter);
	void Set_Collider_Value(_uint iIndex, void* pDesc);

public:
	_bool Created_BoneCollider(_uint iIndex);

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();
	void	Ready_BoneSphere();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	vector<class CModelBoneSphere*> m_BoneSpheres;

	wstring m_strModelName;
	_uint	m_iAnimIndex = { 0 };
	_uint	m_iPassIndex = { 0 };
	_uint	m_iSelectedMeshIndex = { 0 };

public:
	static CAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
