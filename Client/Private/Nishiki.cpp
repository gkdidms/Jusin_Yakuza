#include "Nishiki.h"

CNishiki::CNishiki(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNPC{ pDevice, pContext }
{
}

CNishiki::CNishiki(const CNishiki& rhs)
	: CNPC{ rhs }
{
}

HRESULT CNishiki::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNishiki::Initialize(void* pArg)
{
	return S_OK;
}

void CNishiki::Priority_Tick(const _float& fTimeDelta)
{
}

void CNishiki::Tick(const _float& fTimeDelta)
{
}

void CNishiki::Late_Tick(const _float& fTimeDelta)
{
}

void CNishiki::Change_Animation()
{
}

HRESULT CNishiki::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_GameObject_Nishiki"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CNishiki* CNishiki::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNishiki* pInstance = new CNishiki(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CNishiki::Clone(void* pArg)
{
	CNishiki* pInstance = new CNishiki(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNishiki::Free()
{
	__super::Free();
}
