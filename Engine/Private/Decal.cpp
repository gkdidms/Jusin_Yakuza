#include "Decal.h"

CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice,pContext }
{
}

CDecal::CDecal(const CDecal& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CDecal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal::Initialize(void* pArg)
{
	return S_OK;
}

void CDecal::Priority_Tick(const _float& fTimeDelta)
{
}

void CDecal::Tick(const _float& fTimeDelta)
{
}

void CDecal::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CDecal::Render()
{
	return S_OK;
}

CGameObject* CDecal::Clone(void* pArg)
{
	return nullptr;
}

void CDecal::Free()
{
	__super::Free();
}
