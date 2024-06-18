#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

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
	void Set_Scaled(_float x, _float y, _float z);
	void Set_Rotation(_uint iAxis, _float vRadian, _float fTimeDelta);

public:
	void Change_Model(wstring strModelName);

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	wstring m_strModelName;
	_uint	m_iAnimIndex = { 0 };

public:
	static CAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
