#include "Camera_Manager.h"
#include "GameInstance.h"

CCamera_Manager::CCamera_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CCamera_Manager::Initialize()
{
	return E_NOTIMPL;
}

void CCamera_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CCamera_Manager::Tick(_float fTimeDelta)
{
}

void CCamera_Manager::Late_Tick(_float fTimeDelta)
{
}

void CCamera_Manager::Render()
{
}

void CCamera_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}
