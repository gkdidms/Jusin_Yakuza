#pragma once


#include "SocketObject.h"
#include "CharacterData.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CSocketCollider final : public CSocketObject
{
public:
	enum SOKET_COLLIDER_TYPE
	{
		ATTACK, HIT, TYPE_END
	};

public:
	struct SOCKET_COLLIDER_DESC : public SOCKETOBJECT_DESC
	{
		_uint iType;
		_uint iBoneIndex;
		CCharacterData::COLLIDER_STATE ColliderState;
	};

private:
	CSocketCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSocketCollider(const CSocketCollider& rhs);
	virtual ~CSocketCollider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _bool Intersect(CCollider* pTargetObject);

public:
	void On() {
		m_isOn = true;
	}
	void Off() {
		m_isOn = { false };
	}

public:
	_bool IsOn() {
		return m_isOn;
	}

	_uint Get_CollierType() {
		return static_cast<_uint>(m_eColliderType);
	}

	CCollider* Get_Collider()	{
		return m_pColliderCom;
	}

private:
	SOKET_COLLIDER_TYPE				m_eColliderType = { ATTACK };

	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	CCollider*						m_pColliderCom = { nullptr };

	_float							m_fAlphaScale = { 0.f };
	
	_bool							m_isOn = { true };

public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CSocketCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END