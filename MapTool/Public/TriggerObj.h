#pragma once

#include "GameObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

class CTriggerObj : public CGameObject
{
public:
	typedef struct tColliderObjDesc : public CGameObject::GAMEOBJECT_DESC
	{
		XMMATRIX		vStartPos;
		Trigger_IO		tTriggerDesc;
	}TRIGGEROBJ_DESC;


private:
	CTriggerObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTriggerObj(const CTriggerObj& rhs);
	virtual ~CTriggerObj() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Priority_Tick(const _float& fTimeDelta) override;
	virtual void					Tick(const _float& fTimeDelta) override;
	virtual void					Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	CTransform*						Get_Transform() { return m_pTransformCom; }
	void							Set_TriggerDesc(Trigger_IO triggerDesc);
	Trigger_IO						Get_TriggerDesc() { return m_tTriggerDesc; }


private:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	CCollider*						m_pColliderCom = { nullptr };

	Trigger_IO						m_tTriggerDesc;

public:
	HRESULT							Add_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CTriggerObj*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

