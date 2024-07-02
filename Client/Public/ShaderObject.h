#pragma once
#include "GameObject.h"

#include "Client_Defines.h"
BEGIN(Client)
class CShaderObject abstract : public CGameObject
{
protected:
    CShaderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CShaderObject(const CShaderObject& rhs);
    virtual ~CShaderObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Shader_Rander();

public:
    virtual void Free();
};
END
