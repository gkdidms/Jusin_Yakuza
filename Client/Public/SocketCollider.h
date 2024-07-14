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
	enum COLLIDER_ACTION_TYPE
	{
		ATTACK, HIT, TYPE_END
	};
	enum COLLIDER_PART_TYPE
	{
		HAND_A, FOOT_A, JOINT_A, HEAD_A,
		HEAD_H = 10, BODY_H, LEG_H
	};

public:
	struct SOCKET_COLLIDER_DESC : public SOCKETOBJECT_DESC
	{
		_uint iBoneIndex;
		CCharacterData::COLLIDER_STATE ColliderState;
		class CLandObject* pParentObject;
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

	virtual _bool Intersect(CCollider* pTargetObject, _float fDistance = 1.f);

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
		return static_cast<_uint>(m_eColliderActionType);
	}

	CCollider* Get_Collider()	{
		return m_pColliderCom;
	}

	class CLandObject* Get_Parent() {
		return m_pParentObject;
	}

	const _float3& Get_MoveDir() {
		return m_vMoveDir;
	}

	_float Get_Damage() {
		return m_fDamage;
	}

public:
	void ParentObject_Hit(const _float3& vDir, _float fDamage, class CLandObject* pParentObject, _bool isBlowAttack = false);

private:
	COLLIDER_ACTION_TYPE			m_eColliderActionType = { ATTACK };
	COLLIDER_PART_TYPE				m_eColliderPartType = { HAND_A };

	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	CCollider*						m_pColliderCom = { nullptr };


	class CLandObject*				m_pParentObject = { nullptr };


	_float							m_fDamage = { 0.f };
	
	_bool							m_isOn = { true };

	_float3							m_vPrevMovePos, m_vMovePos;
	_float3							m_vMoveDir;

public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CSocketCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END