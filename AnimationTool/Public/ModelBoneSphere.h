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

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	_uint m_iTextureIndex = { 0 };

public:
	static CModelBoneSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
