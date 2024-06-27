#pragma once


#include "SoketObject.h"
#include "CharacterData.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CSoketCollider final : public CSoketObject
{
public:
	struct SOKET_COLLIDER_DESC : public SOKETOBJECT_DESC
	{
		_uint iBoneIndex;
		CCharacterData::COLLIDER_STATE ColliderState;
	};

private:
	CSoketCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSoketCollider(const CSoketCollider& rhs);
	virtual ~CSoketCollider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	//class CSystemManager*			m_pSystemManager = { nullptr };
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	CCollider*						m_pColliderCom = { nullptr };

	_float							m_fAlphaScale = { 0.f };
	
	_bool							m_isOn = { true };

public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CSoketCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END