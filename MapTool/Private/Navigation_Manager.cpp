#include "../Public/Navigation_Manager.h"
#include "GameInstance.h"

#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include "Shader.h"
#include "VIBuffer_Cell.h"
#include "Navigation.h"
//#include "Terrain_Manager.h"
//#include "ObjPlace_Manager.h"

#include <iostream>
#include <io.h>


IMPLEMENT_SINGLETON(CNavigation_Manager)

//_float4x4	CNavigation_Manager::m_WorldMatrix{};

CNavigation_Manager::CNavigation_Manager()
{
}

HRESULT CNavigation_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	/* For.Com_Shader */
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

void CNavigation_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CNavigation_Manager::Tick(_float fTimeDelta)
{

}

void CNavigation_Manager::Late_Tick(_float fTimeDelta)
{
}

HRESULT CNavigation_Manager::Render()
{

	return S_OK;
}


void CNavigation_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pShader);

	//for (auto& cell : m_Cells)
	//	Safe_Release(cell);

	//m_Cells.clear();

	//for (auto& iter : m_CellsName)
	//	Safe_Delete(iter);

	//for (auto& iter : m_FileNames)
	//	Safe_Delete(iter);

	//m_CellsName.clear();

	//m_Cells.clear();

}
