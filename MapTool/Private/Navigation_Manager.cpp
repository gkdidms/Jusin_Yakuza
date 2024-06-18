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
//	if (0 == m_Cells.size())
//		return S_OK;
//
//#ifdef _DEBUG
//	/*if (nullptr != m_pNavigationCom)
//		m_pNavigationCom->Render();*/
//
//	if (0 < m_Cells.size())
//	{
//		_float4x4			WorldMatrix;
//		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
//		//WorldMatrix._42 += 0.1f;
//
//		m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
//
//		m_pShader->Bind_Matrix("g_ViewMatrix", CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW));
//		m_pShader->Bind_Matrix("g_ProjMatrix", CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ));
//
//		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4))))
//			return E_FAIL;
//
//		m_pShader->Begin(0);
//
//		for (auto& pcell : m_Cells)
//			pcell->Render();
//
//
//		WorldMatrix._42 += 0.1f;
//		m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
//
//		m_pShader->Bind_Matrix("g_ViewMatrix", CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW));
//		m_pShader->Bind_Matrix("g_ProjMatrix", CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ));
//
//		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4))))
//			return E_FAIL;
//
//		m_pShader->Begin(0);
//
//		m_Cells[m_iCurrentCellIndex]->Render();
//	}
//
//#endif
//
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
