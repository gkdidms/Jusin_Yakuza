#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CModelBoneSphere final : public CGameObject
{
public:
	struct BoneSphereDesc : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pBoneWorldMatrix;
		const _float4x4* pModelWorldMatrix;
	};

private:
	CModelBoneSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelBoneSphere(const CModelBoneSphere& rhs);
	virtual ~CModelBoneSphere() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(const _float& fTimeDelta);
	virtual void Tick(const _float& fTimeDelta);
	virtual void Late_Tick(const _float& fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	void Change_TexutreIndex(_bool isOn = false);

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	_uint				m_iTextureIndex = { 0 };
	const _float4x4*	m_pModelWorldMatrix = { nullptr };
	const _float4x4*	m_pBoneWorldMatrix = { nullptr };

public:
	static CModelBoneSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
