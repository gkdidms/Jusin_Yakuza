#include "Group.h"

#include "GameInstance.h"
#include "Object_Manager.h"
#include "Image_Texture.h"
#include "Text.h"
#include "Btn.h"
#include "UI_Effect.h"	

CGroup::CGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Object{ pDevice, pContext}
{
}

CGroup::CGroup(const CGroup& rhs)
	: CUI_Object{ rhs }
{
}

void CGroup::Remove_PartObject(_uint iIndex)
{
	auto iter = m_PartObjects.begin();

	Safe_Release(m_PartObjects[iIndex]);

	m_PartObjects.erase(iter + iIndex);

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

HRESULT CGroup::Save_binary(const string strDirectory)
{

	return S_OK;
}

HRESULT CGroup::Save_Groupbinary(ofstream& out)
{
	for (auto& pObject : m_PartObjects)
	{
		pObject->Save_Groupbinary(out);
	}

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
