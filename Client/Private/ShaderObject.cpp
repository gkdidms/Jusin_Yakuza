#include "ShaderObject.h"

CShaderObject::CShaderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CShaderObject::CShaderObject(const CShaderObject& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CShaderObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShaderObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CShaderObject::Priority_Tick(const _float& fTimeDelta)
{
}

void CShaderObject::Tick(const _float& fTimeDelta)
{
}

void CShaderObject::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CShaderObject::Render()
{
	return S_OK;
}

HRESULT CShaderObject::Shader_Rander()
{
	return S_OK;
}

void CShaderObject::Free()
{
	__super::Free();
}
