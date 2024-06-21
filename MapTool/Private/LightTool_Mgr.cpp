#include "LightTool_Mgr.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLightTool_Mgr)

CLightTool_Mgr::CLightTool_Mgr()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLightTool_Mgr::Initialize()
{
	return S_OK;
}

void CLightTool_Mgr::Priority_Tick(_float fTimeDelta)
{
}

void CLightTool_Mgr::Tick(_float fTimeDelta)
{
}

void CLightTool_Mgr::Late_Tick(_float fTimeDelta)
{
}

void CLightTool_Mgr::Render()
{
}

void CLightTool_Mgr::Free()
{
	Safe_Release(m_pGameInstance);
}
