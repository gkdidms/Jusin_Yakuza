#include "LandObject.h"  

#include "SystemManager.h"
#include "Collision_Manager.h"


CLandObject::CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext},
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pCollisionManager{ CCollision_Manager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
}
CLandObject::CLandObject(const CLandObject& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ CSystemManager::GetInstance()},
	m_pCollisionManager{ CCollision_Manager::GetInstance()}
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLandObject::Priority_Tick(const _float& fTimeDelta)
{
}

void CLandObject::Tick(const _float& fTimeDelta)
{
}

void CLandObject::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

HRESULT CLandObject::Add_Componenets()
{
	return S_OK;
}

HRESULT CLandObject::Bind_ResourceData()
{
	return S_OK;
}

void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pSystemManager);
	Safe_Release(m_pCollisionManager);
}
