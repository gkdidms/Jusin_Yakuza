#include "../Public/Navigation_Manager.h"
#include "GameInstance.h"

#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include "Shader.h"
#include "VIBuffer_Cell.h"
#include "Navigation.h"
#include "Terrain_Manager.h"
#include "ObjPlace_Manager.h"

#include <iostream>
#include <io.h>



IMPLEMENT_SINGLETON(CNavigation_Manager)

_float4x4	CNavigation_Manager::m_WorldMatrix{};

CNavigation_Manager::CNavigation_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
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


	m_pShaderLineCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Line.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShaderLineCom)
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
	if (0 == m_Cells.size())
		return S_OK;

#ifdef _DEBUG

	if (0 < m_Cells.size())
	{
		_float4x4			WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		WorldMatrix._42 += 0.1f;

		m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);

		m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
		m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

		_float4		vColor = _float4(0.f, 1.f, 0.f, 1.f);
		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		for (auto& pcell : m_Cells)
			pcell->Render();


		WorldMatrix._42 += 0.2f;
		m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);

		m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
		m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

		_float4		vCurColor = _float4(1.f, 0.f, 0.f, 1.f);
		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vCurColor, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		m_Cells[m_iCurrentCellIndex]->Render();
	}

#endif

	if (nullptr != m_pVIBufferCom)
	{
		XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
		m_WorldMatrix.m[3][1] += 0.2f;
		m_pShaderLineCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
		m_pShaderLineCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
		m_pShaderLineCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

		_vector vColor = { 0.f, 0.f, 1.f, 1.f };
		m_pShaderLineCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector));

		m_pShaderLineCom->Begin(0);
		m_pVIBufferCom->Render();
	}

	

	return S_OK;
}

TCHAR* CNavigation_Manager::StringToTCHAR(string& s)
{
	TCHAR tstr;
	const char* all = s.c_str();
	int len = 1 + (int)strlen(all);
	wchar_t* t = new wchar_t[len];
	if (NULL == t) throw std::bad_alloc();
	mbstowcs(t, all, len);
	return (TCHAR*)t;
}

void CNavigation_Manager::Load_Cell_IMGUI()
{
	ImGui::Begin(u8"��");

	ImGui::Text(u8" ���� �̸� ");

	if (m_FileNames.empty())
	{
		Update_FileName();
	}

	static int layer_current_idx = 0;
	if (ImGui::BeginListBox("listbox 0"))
	{
		for (int n = 0; n < m_FileNames.size(); n++)
		{
			const bool is_selected = (layer_current_idx == n);
			if (ImGui::Selectable(m_FileNames[n], is_selected))
				layer_current_idx = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (m_iCurrentFileNavi != layer_current_idx)
	{
		m_iCurrentFileNavi = layer_current_idx;
		Load_Cells(m_iCurrentFileNavi);
	}


	if (ImGui::Button(u8"Cell ���� �ε�"))
	{
		Load_Cells(layer_current_idx);
	}

	ImGui::NewLine();
	ImGui::Text(u8"Route ����� ��ư - route �����ϴ� â ����");

	if (ImGui::Button(u8"Route �����"))
	{
		m_bMakeRoute_IMGUI = true;
	}

	/* Route File �ε� */

	static int route_layer_current_idx = 0;

	if (ImGui::BeginListBox(u8"��Ʈ"))
	{
		for (int n = 0; n < m_RouteName.size(); n++)
		{
			const bool is_selected = (route_layer_current_idx == n);
			if (ImGui::Selectable(m_RouteName[n], is_selected))
				route_layer_current_idx = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (m_iCurrentFileRoute != route_layer_current_idx)
	{
		m_iCurrentFileRoute = route_layer_current_idx;
		Load_Route(m_iCurrentFileRoute);
	}


	if (ImGui::Button(u8"Route ���� "))
	{
		Delete_Route(route_layer_current_idx);
	}


	ImGui::Text(u8"�� route index�� cell ���� ����");
	if (ImGui::Button(u8" route cell ���� "))
	{
		m_Routes[route_layer_current_idx] = m_Route_CellIndexes;
	}

	ImGui::End();
}

void CNavigation_Manager::Make_Point(_vector vPickingPos)
{
	_float3			vFinalpickpos;
	_float			minDistance = MAX_PATH;

	/* ����� �Ÿ� ���� �� Ž�� */
	if (0 < m_Cells.size())
		vFinalpickpos = Find_ClosestPoint(vPickingPos, &minDistance);

	/* �Ÿ��� �ʹ� �ָ� �׳� ���� ��*/
	/* �� ũ�⿡ ���� ���������� �ٲٱ� */
	if (0.3f < minDistance)
		XMStoreFloat3(&vFinalpickpos, vPickingPos);

	m_vPoints.push_back(vFinalpickpos);

	//���� ���� ������ 3���� �ȴٸ�. �ﰢ���� �׷����� Cell�� �߰��Ѵ�.
	if (3 == m_vPoints.size())
	{
		Make_Cell();
		m_vPoints.clear();
	}


	/* IMGUI ���� */
	Update_CellsName();
}

HRESULT CNavigation_Manager::Make_Cell()
{
	_float3 vPoints[CCell::POINT_END];

	for (int i = 0; i < CCell::POINT_END; i++)
	{
		vPoints[i] = m_vPoints[i];
	}

	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size(), CCell::OPTION_NONE);

	if (nullptr == pCell)
		return E_FAIL;
	m_Cells.push_back(pCell);

	return S_OK;
}

HRESULT CNavigation_Manager::Save_Cells(_uint iIndex)
{

	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/NaviData/");

	strcat_s(fullPath, "Navigation");


	string strNum = to_string(iIndex);
	char cIndex[20];
	strcpy_s(cIndex, strNum.c_str());

	strcat_s(fullPath, "_");
	strcat_s(fullPath, cIndex);
	strcat_s(fullPath, ".dat");

	fs::path path(fullPath);

	ofstream out(fullPath, ios::binary);

	if (!out) {
		return E_FAIL;
	}

	_ulong		dwByte = {};
	_float3		vPoints[CCell::POINT_END] = {};

	_uint		iCellNum = m_Cells.size();
	out.write((_char*)&iCellNum, sizeof(_uint));

	for (int i = 0; i < iCellNum; i++)
	{
		/* ����Ʈ ���� */
		XMStoreFloat3(&vPoints[CCell::POINT_A], m_Cells[i]->Get_Point(CCell::POINT_A));
		XMStoreFloat3(&vPoints[CCell::POINT_B], m_Cells[i]->Get_Point(CCell::POINT_B));
		XMStoreFloat3(&vPoints[CCell::POINT_C], m_Cells[i]->Get_Point(CCell::POINT_C));

		out.write((_char*)vPoints, sizeof(_float3) * 3);

		/* �ɼ� ���� */
		_int		iOption = m_Cells[i]->Get_Option();
		out.write((_char*)&iOption, sizeof(_int));
	}

	// ��Ʈ ���� ����
	_uint		iRouteCnt = m_Routes.size();
	out.write((_char*)&iRouteCnt, sizeof(_uint));

	for (int i = 0; i < iRouteCnt; i++)
	{
		_uint		iCellCnt = m_Routes.find(i)->second.size();

		out.write((_char*)&iCellCnt, sizeof(_uint));

		ROUTE_IO* arr = new ROUTE_IO[iCellCnt];

		for (int j = 0; j < iCellCnt; j++)
		{
			arr[j] = m_Routes.find(i)->second[j];
		}

		out.write(reinterpret_cast<char*>(arr), iCellCnt * sizeof(ROUTE_IO));

		Safe_Delete(arr);
	}



	out.close();

	Update_FileName();
}

HRESULT CNavigation_Manager::Load_Cells(_uint iIndex)
{
	for (auto& iter : m_Cells)
		Safe_Release(iter);
	m_Cells.clear();

	for (auto& cell : m_RouteCells)
		Safe_Release(cell);
	m_RouteCells.clear();

	for (auto& iter : m_IndexesName)
		Safe_Delete(iter);
	m_IndexesName.clear();

	for (auto& iter : m_RouteName)
		Safe_Delete(iter);
	m_RouteName.clear();

	Safe_Release(m_pVIBufferCom);

	m_Route_CellIndexes.clear();

	for (auto& Pair : m_Routes)
		Pair.second.clear();
	m_Routes.clear();


	/* ���� �ε��� ���� index �ٽ� 0���� �ǵ�����*/
	m_iCurrentCellIndex = 0;
	m_iCurrentRouteCellIndex = 0;
	m_iCurrentFileRoute = 0;

	_ulong		dwByte = {};
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/NaviData/");

	strcat_s(fullPath, m_FileNames[iIndex]);

	string strPath(fullPath);

	ifstream in(fullPath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("���� ���� ����");
		return E_FAIL;
	}

	_uint NumCells = { 0 };
	in.read((_char*)&NumCells, sizeof(_uint));

	for (size_t i = 0; i < NumCells; ++i)
	{
		/* point �ε� */
		_float3* vPoint = new _float3[3];
		in.read((_char*)vPoint, sizeof(_float3) * 3);

		/* �ɼ� �ε� */
		_int iOption = { 0 };
		in.read((_char*)&iOption, sizeof(_int));

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoint, i, CCell::OPTION(iOption));

		m_Cells.emplace_back(pCell);
		Safe_Delete(vPoint);
	}



	// ��Ʈ ���� �о����
	_uint		iRouteCnt = 0;
	in.read((_char*)&iRouteCnt, sizeof(_uint));

	for (int i = 0; i < iRouteCnt; i++)
	{
		_uint		iCellCnt = 0;

		in.read((_char*)&iCellCnt, sizeof(_uint));

		ROUTE_IO* arr = new ROUTE_IO[iCellCnt];
		in.read(reinterpret_cast<char*>(arr), iCellCnt * sizeof(ROUTE_IO));

		vector<ROUTE_IO>		routeCells;

		for (int j = 0; j < iCellCnt; j++)
		{
			routeCells.push_back(arr[j]);
		}

		m_Routes.emplace(i, routeCells);

		Safe_Delete(arr);
	}


	in.close();

	/* option ���� ���� ���� �°� */
	/* ���� cell�� �ϳ��� ������ option�� �°� ��ġ */
	if (NumCells != 0)
	{
		m_iCurrentOption = m_Cells[m_iCurrentCellIndex]->Get_Option();
	}
	
	
	Update_CellsName();
	Update_RouteName();
	Update_IndexesName();
	Load_Route(m_iCurrentFileRoute);
}


HRESULT CNavigation_Manager::Load_Route(_uint iIndex)
{

	if (iIndex < m_Routes.size())
	{
		m_Route_CellIndexes.clear();

		int iNums = m_Routes.find(iIndex)->second.size();

		for (int i = 0; i < iNums; i++)
		{
			m_Route_CellIndexes.push_back(m_Routes.find(iIndex)->second[i]);
		}

		for (auto& cell : m_RouteCells)
			Safe_Release(cell);
		m_RouteCells.clear();



		for (size_t i = 0; i < iNums; ++i)
		{
			if (m_Cells.size() > 0)
			{
				m_RouteCells.push_back(m_Cells[m_Route_CellIndexes[i].iCellNums]);
				Safe_AddRef(m_Cells[m_Route_CellIndexes[i].iCellNums]);
			}
		}

		if (0 < m_RouteCells.size())
		{
			Safe_Release(m_pVIBufferCom);

			m_pVIBufferCom = CVIBuffer_Line::Create(m_pDevice, m_pContext, m_Route_CellIndexes);
			if (nullptr == m_pVIBufferCom)
				MSG_BOX("VIBuffer_Lint ���� �Ұ�");
		}

		Update_IndexesName();
	}
	

	return S_OK;
}

void CNavigation_Manager::Show_FileName()
{
	ImGui::Text(u8" �׺���̼� ����Ʈ ");

	if (m_FileNames.empty())
	{
		Update_FileName();
	}

	static int layer_current_idx;
	if (ImGui::BeginListBox("listbox 1"))
	{
		for (int n = 0; n < m_FileNames.size(); n++)
		{
			const bool is_selected = (layer_current_idx == n);
			if (ImGui::Selectable(m_FileNames[n], is_selected))
				layer_current_idx = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}


}

void CNavigation_Manager::Update_FileName()
{
	for (auto& iter : m_FileNames)
		Safe_Delete(iter);

	m_FileNames.clear();
	string path = "../../Client/Bin/DataFiles/NaviData/*.dat";

	struct _finddata_t fd;
	intptr_t handle;

	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
		return; // ���Ͼ�����
	do
	{
		char* cfilename = new char[MAX_PATH];
		strcpy(cfilename, fd.name);
		m_FileNames.push_back(cfilename);
	} while (_findnext(handle, &fd) == 0);
	_findclose(handle);
}

void CNavigation_Manager::Update_RouteFileName()
{
	for (auto& iter : m_RouteFileNames)
		Safe_Delete(iter);

	m_RouteFileNames.clear();
	string path = "../../Client/Bin/DataFiles/RouteData/*.dat";

	struct _finddata_t fd;
	intptr_t handle;

	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
		return; // ���Ͼ�����
	do
	{
		char* cfilename = new char[MAX_PATH];
		strcpy(cfilename, fd.name);
		m_RouteFileNames.push_back(cfilename);
	} while (_findnext(handle, &fd) == 0);
	_findclose(handle);
}

void CNavigation_Manager::Delete_Cell(_uint iIndex)
{
	vector<class CCell*>::iterator	iter = m_Cells.begin();

	for (int i = 0; i < iIndex; i++)
	{
		iter++;
	}

	Safe_Release(*iter);
	m_Cells.erase(iter);

	/* �ε��� �Ű��ֱ� */
	if (0 > m_iCurrentCellIndex - 1)
	{
		m_iCurrentCellIndex = 0;
	}
	else
	{
		m_iCurrentCellIndex -= 1;
	}

	Update_CellsName();
}

void CNavigation_Manager::Delete_AllCell()
{
	for (auto& cell : m_Cells)
		Safe_Release(cell);

	m_Cells.clear();

	Update_CellsName();

	/* �ε����� ���� �ʱ�ȭ */
	m_iCurrentCellIndex = 0;
}

void CNavigation_Manager::Delete_RouteCell(_int& iIndex)
{
	vector<ROUTE_IO>::iterator	iter = m_Route_CellIndexes.begin();
	list<CCell*>::iterator	cellIter = m_RouteCells.begin();

	for (int i = 0; i < iIndex; i++)
	{
		iter++;
		cellIter++;
	}

	m_Route_CellIndexes.erase(iter);

	Safe_Release(*cellIter);
	m_RouteCells.erase(cellIter);

	/* �ε��� �Ű��ֱ� */
	if (0 > m_iCurrentRouteCellIndex - 1)
	{
		m_iCurrentRouteCellIndex = 0;
	}
	else
	{
		m_iCurrentRouteCellIndex -= 1;
	}

	Update_IndexesName();
}

void CNavigation_Manager::Delete_AllRouteCell(_int& iIndex)
{

	m_Route_CellIndexes.clear();

	for (auto& cell : m_RouteCells)
		Safe_Release(cell);
	m_RouteCells.clear();

	Update_IndexesName();

	/* �ε����� ���� �ʱ�ȭ */
	iIndex = 0;
}

int CNavigation_Manager::Get_Player_Monster_NaviIndex(_vector vPosition)
{
	for (int i = 0; i < m_Cells.size(); i++)
	{
		/* �׳� �־��ִ°� */
		int			iNeighborsIndex;

		if (true == m_Cells[i]->isIn(vPosition, &iNeighborsIndex))
		{
			return i;
		}
	}

	return -1;
}

void CNavigation_Manager::Find_Cells()
{
	/* F ������ �۵� */
	if (m_pGameInstance->GetKeyState(DIK_F) == HOLD)
	{
		if (0 < m_Cells.size())
		{
			if (CGameInstance::GetInstance()->GetMouseState(DIM_LB) == AWAY)
			{
				_bool		isPick;
				_vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);

				for (int i = 0; i < m_Cells.size(); i++)
				{
					/* �׳� �־��ִ°� */
					int			iNeighborsIndex;

					if (true == m_Cells[i]->isIn(vTargetPos, &iNeighborsIndex))
					{
						m_iCurrentCellIndex = i;
					}
				}
			}
		}
	}
}

void CNavigation_Manager::Make_Route()
{
	if (true == m_bMakeRoute_IMGUI)
	{
		ImGui::Begin(u8" Route Num ");
		ImGui::Text(u8" 1. Cell ã�� - F ������ Ŭ�� - ������ ������ �ٶ󺸰Բ� ");
		ImGui::Text(u8" 2. ���ϴ� cell�� ���õ����� cell - route�� �߰� ��ư ������ ");
		ImGui::NewLine();

		static int index_current_idx;

		if (index_current_idx != m_iCurrentRouteCellIndex)
		{
			index_current_idx = m_iCurrentRouteCellIndex;
		}

		if (ImGui::BeginListBox("listbox 1"))
		{
			for (int n = 0; n < m_IndexesName.size(); n++)
			{
				const bool is_selected = (index_current_idx == n);
				if (ImGui::Selectable(m_IndexesName[n], is_selected))
					index_current_idx = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		if (m_iCurrentRouteCellIndex != index_current_idx)
		{
			m_iCurrentRouteCellIndex = index_current_idx;
		}

		ImGui::Text(u8"��ư Ŭ�� �� ��ŷ�ϸ� �߰���");
		
		if (m_bMakeRoute_Picking == true && m_pGameInstance->GetMouseState(DIM_LB) == AWAY)
		{
			m_bMakeRoute_Picking = false;

			_bool		isPick;
			_vector		vTargetPos = m_pGameInstance->Picking(&isPick);

			for (int i = 0; i < m_Cells.size(); i++)
			{
				/* �׳� �־��ִ°� */
				int			iNeighborsIndex;

				if (true == m_Cells[i]->isIn(vTargetPos, &iNeighborsIndex))
				{
					m_iCurrentCellIndex = i;
				}
			}

			ROUTE_IO		routeIO;
			routeIO.iCellNums = m_iCurrentCellIndex;
			XMStoreFloat4(&routeIO.vPosition, vTargetPos);
			m_Route_CellIndexes.push_back(routeIO);
			m_RouteCells.push_back(m_Cells[m_iCurrentCellIndex]);
			Safe_AddRef(m_Cells[m_iCurrentCellIndex]);

			// route ���� index update
			Update_IndexesName();

			if (2 <= m_Route_CellIndexes.size())
			{
				Safe_Release(m_pVIBufferCom);

				m_pVIBufferCom = CVIBuffer_Line::Create(m_pDevice, m_pContext, m_Route_CellIndexes);
				if (nullptr == m_pVIBufferCom)
					MSG_BOX("VIBuffer_Lint ���� �Ұ�");
			}
			
		}

		if (ImGui::Button(u8"cell - route�� �߰� "))
		{
			m_bMakeRoute_Picking = true;

		}

		if (ImGui::Button(u8" cell Index ���� "))
		{
			Delete_RouteCell(m_iCurrentRouteCellIndex);
		}
		
		ImGui::SameLine();

		if (ImGui::Button(u8" cell Index ��ü���� "))
		{
			Delete_AllRouteCell(m_iCurrentRouteCellIndex);

			Safe_Release(m_pVIBufferCom);
		}

		if (ImGui::Button(u8"�׺� ��Ʈ�߰�"))
		{
			Add_Route_In_Navi();
			Update_RouteName();
		}

		ImGui::NewLine();



		//static int iRouteFileNum = 70;
		//ImGui::InputInt(u8"��Ʈ ���� index : ", &iRouteFileNum);

		//if (ImGui::Button(u8"��Ʈ ����"))
		//{
		//	Save_Route(iRouteFileNum);
		//}

		ImGui::NewLine();

		if (ImGui::Button(u8" â �ݱ� "))
		{
			m_bMakeRoute_IMGUI = false;
		}

		ImGui::End();
	}
	
}

void CNavigation_Manager::Add_Route_In_Navi()
{
	vector<ROUTE_IO>		routes;
	
	for (int i = 0; i < m_Route_CellIndexes.size(); i++)
	{
		routes.push_back(m_Route_CellIndexes[i]);
	}

	m_Routes.emplace(m_Routes.size(), routes);
}


_float3 CNavigation_Manager::Find_ClosestPoint(_vector pickPos, _float* pMinDistance)
{
	_float		minDistance;

	/* terrain���� Ŭ �� ������ �ִ�ġ�� ��� */
	_float2		terrainSize = CTerrain_Manager::GetInstance()->Get_LandScale();
	terrainSize.x > terrainSize.y ? minDistance = terrainSize.x : minDistance = terrainSize.y;

	_float3		resultPos;

	/* ���� ����� �� ã�� - fDistance���� �Ÿ� �־��ֱ�*/
	for (auto& cell : m_Cells)
	{
		_vector vPoint[CCell::POINT_END];
		vPoint[CCell::POINT_A] = cell->Get_Point(CCell::POINT_A);
		vPoint[CCell::POINT_B] = cell->Get_Point(CCell::POINT_B);
		vPoint[CCell::POINT_C] = cell->Get_Point(CCell::POINT_C);

		for (int i = 0; i < CCell::POINT_END; i++)
		{
			_float fdistance = XMVectorGetX(XMVector3Length(pickPos - vPoint[i]));

			if (fdistance > minDistance)
				continue;

			/* �����Ÿ� �ֱ� */
			minDistance = fdistance;
			XMStoreFloat3(&resultPos, vPoint[i]);
		}
	}

	/* �����Ÿ� �־��ֱ� */
	*pMinDistance = minDistance;

	return resultPos;
}



void CNavigation_Manager::Show_Cells_IMGUI()
{
	ImGui::Text(u8" F ������ ���� ���콺 Ŭ���ϱ� - CELL ã�� ");
	ImGui::Text(u8" X �ѹ� ������ Ŭ��. �ð��������!!!!!!!!! - �� ���� ");
	ImGui::Text(u8" �ð����!!!!!!!!!!! ");

	//Find_Cells();

	if (0 < m_CellsName.size())
	{
		static int cell_current_idx;

		if (cell_current_idx != m_iCurrentCellIndex)
		{
			cell_current_idx = m_iCurrentCellIndex;
		}

		if (ImGui::BeginListBox("listbox 0"))
		{
			for (int n = 0; n < m_CellsName.size(); n++)
			{
				/*const bool is_selected = (cell_current_idx == n);*/
				const bool is_selected = (cell_current_idx == n);
				if (ImGui::Selectable(m_CellsName[n], is_selected))
				{
					/*cell_current_idx = n;
					m_iCurrentCellIndex = cell_current_idx;*/
					cell_current_idx = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		//Find_Cells();

		ImGui::Text(u8"�׺���̼� �ɼ�");
		static int naviOption = m_Cells[m_iCurrentCellIndex]->Get_Index();

		/* �ٸ� ������Ʈ Ŭ���� */
		if (m_iCurrentCellIndex != cell_current_idx && m_Cells.size() > 0)
		{
			m_iCurrentCellIndex = cell_current_idx;
			m_iCurrentOption = m_Cells[m_iCurrentCellIndex]->Get_Option();
		}

		Find_Cells();

		if (m_iCurrentCellIndex != cell_current_idx && m_Cells.size() > 0)
		{
			cell_current_idx = m_iCurrentCellIndex;
			m_iCurrentOption = m_Cells[m_iCurrentCellIndex]->Get_Option();
		}

		if (ImGui::RadioButton(u8"�ٴ�", m_iCurrentOption == 0))
		{
			naviOption = 0;
			m_iCurrentOption = 0;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton(u8"���", m_iCurrentOption == 1))
		{
			naviOption = 1;
			m_iCurrentOption = 1;
		}

		m_Cells[m_iCurrentCellIndex]->Set_Option(static_cast<CCell::OPTION>(m_iCurrentOption));

		/*if (ImGui::Button(u8"�׺� �ɼ� ����"))
		{
			m_Cells[m_iCurrentCellIndex]->Set_Option(static_cast<CCell::OPTION>(m_iCurrentOption));
		}*/

	}

	static int iNavIndex = 70;
	ImGui::InputInt(u8"�׺���̼� ���� index : ", &iNavIndex);

	if (ImGui::Button(u8"�׺���̼� ����"))
	{
		Save_Cells(iNavIndex);
	}


	if (ImGui::Button(u8"�� ����"))
	{
		Delete_Cell(m_iCurrentCellIndex);
	}

	if (ImGui::Button(u8"�� ��ü ����"))
	{
		Delete_AllCell();
	}
}

void CNavigation_Manager::Update_CellsName()
{
	if (0 < m_CellsName.size())
	{
		for (auto& iter : m_CellsName)
			Safe_Delete(iter);

		m_CellsName.clear();
	}

	if (0 < m_Cells.size())
	{

		for (int i = 0; i < m_Cells.size(); i++)
		{
			char* szName = new char[MAX_PATH];
			strcpy(szName, "Cell");
			char buff[MAX_PATH];
			sprintf(buff, "%d", i);
			strcat(szName, buff);
			m_CellsName.push_back(szName);
		}
	}
}

void CNavigation_Manager::Update_RouteName()
{
	if (0 < m_RouteName.size())
	{
		for (auto& iter : m_RouteName)
			Safe_Delete(iter);

		m_RouteName.clear();
	}

	if (0 < m_Routes.size())
	{
		for (int i = 0; i < m_Routes.size(); i++)
		{
			char* szName = new char[MAX_PATH];
			strcpy(szName, "Route");
			char buff[MAX_PATH];
			sprintf(buff, "%d", i);
			strcat(szName, buff);
			m_RouteName.push_back(szName);
		}
	}
}

void CNavigation_Manager::Update_IndexesName()
{
	if (0 < m_IndexesName.size())
	{
		for (auto& iter : m_IndexesName)
			Safe_Delete(iter);

		m_IndexesName.clear();
	}

	if (0 < m_Route_CellIndexes.size())
	{
		for (int i = 0; i < m_Route_CellIndexes.size(); i++)
		{
			char* szName = new char[MAX_PATH];
			strcpy(szName, "Index");
			char buff[MAX_PATH];
			sprintf(buff, "%d", m_Route_CellIndexes[i].iCellNums);
			strcat(szName, buff);

			m_IndexesName.push_back(szName);
		}
	}
}

void CNavigation_Manager::Update_Routes()
{
	map<int, vector<ROUTE_IO>> new_m_Routes;
	int new_key = 0;
	for (const auto& pair : m_Routes) {
		new_m_Routes[new_key] = pair.second;
		new_key++;
	}

	m_Routes = move(new_m_Routes);

}

void CNavigation_Manager::Delete_Route(_int& iIndex)
{
	if (0 < m_RouteCells.size())
	{
		map<int, vector<ROUTE_IO>>::iterator iter = m_Routes.begin();

		for (int i = 0; i < iIndex; i++)
		{
			iter++;
		}

		iter->second.clear();
		m_Routes.erase(iIndex);

		/* �ε��� �Ű��ֱ� */
		if (0 > iIndex - 1)
		{
			iIndex = 0;
		}
		else
		{
			iIndex -= 1;
		}

		Update_Routes();
		Update_RouteName();
		Update_IndexesName();

		Load_Route(iIndex);
	}
	
}


void CNavigation_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pShader);
	Safe_Release(m_pShaderLineCom);

	for (auto& cell : m_Cells)
		Safe_Release(cell);
	m_Cells.clear();

	for (auto& cell : m_RouteCells)
		Safe_Release(cell);
	m_RouteCells.clear();

	for (auto& iter : m_CellsName)
		Safe_Delete(iter);
	m_CellsName.clear();

	for (auto& iter : m_IndexesName)
		Safe_Delete(iter);
	m_IndexesName.clear();

	for (auto& iter : m_RouteName)
		Safe_Delete(iter);
	m_RouteName.clear();


	for (auto& iter : m_FileNames)
		Safe_Delete(iter);
	m_FileNames.clear();

	Safe_Release(m_pVIBufferCom);


	m_Route_CellIndexes.clear();



	for (auto& Pair : m_Routes)
		Pair.second.clear();
	m_Routes.clear();


	Safe_Release(m_pGameInstance);
}
