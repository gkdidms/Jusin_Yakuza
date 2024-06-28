#include "Group.h"

#include "GameInstance.h"

CGroup::CGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Object{ pDevice, pContext}
{
}

CGroup::CGroup(const CGroup& rhs)
	: CUI_Object{ rhs }
{
}

HRESULT CGroup::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGroup::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CGroup::Priority_Tick(const _float& fTimeDelta)
{
	for (auto& pObject : m_PartObjects)
		pObject->Priority_Tick(fTimeDelta);
}

void CGroup::Tick(const _float& fTimeDelta)
{
	for (auto& pObject : m_PartObjects)
		pObject->Tick(fTimeDelta);
}

void CGroup::Late_Tick(const _float& fTimeDelta)
{
	for (auto& pObject : m_PartObjects)
		pObject->Late_Tick(fTimeDelta);
}

HRESULT CGroup::Render()
{
	for (auto& pObject : m_PartObjects)
		pObject->Render();

	return S_OK;
}

CGroup* CGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGroup* pInstance = new CGroup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CGroup::Clone(void* pArg)
{
	CGroup* pInstance = new CGroup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CGroup::Free()
{
	__super::Free();

	for (auto& pObject : m_PartObjects)
		Safe_Release(pObject);
}
