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
	/*if (nullptr != m_pNavigationCom)
		m_pNavigationCom->Render();*/

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
	ImGui::Begin(u8"셀");

	ImGui::Text(u8" 파일 이름 ");

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


	//if (ImGui::Button(u8"Cell 정보 로드"))
	//{
	//	Load_Cells(layer_current_idx);
	//}

	ImGui::End();
}

void CNavigation_Manager::Make_Point(_vector vPickingPos)
{
	_float3			vFinalpickpos;
	_float			minDistance = MAX_PATH;

	/* 가까운 거리 상의 점 탐색 */
	if (0 < m_Cells.size())
		vFinalpickpos = Find_ClosestPoint(vPickingPos, &minDistance);

	/* 거리가 너무 멀면 그냥 찍은 점*/
	/* 맵 크기에 따라 유동적으로 바꾸기 */
	if (1.5f < minDistance)
		XMStoreFloat3(&vFinalpickpos, vPickingPos);

	m_vPoints.push_back(vFinalpickpos);

	//찍은 점의 개수가 3개가 된다면. 삼각형이 그려지니 Cell에 추가한다.
	if (3 == m_vPoints.size())
	{
		Make_Cell();
		m_vPoints.clear();
	}


	/* IMGUI 위한 */
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

	char cIndex[2] = "";
	_itoa_s(iIndex, cIndex, 10);
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
		/* 포인트 저장 */
		XMStoreFloat3(&vPoints[CCell::POINT_A], m_Cells[i]->Get_Point(CCell::POINT_A));
		XMStoreFloat3(&vPoints[CCell::POINT_B], m_Cells[i]->Get_Point(CCell::POINT_B));
		XMStoreFloat3(&vPoints[CCell::POINT_C], m_Cells[i]->Get_Point(CCell::POINT_C));

		out.write((_char*)vPoints, sizeof(_float3) * 3);

		/* 옵션 저장 */
		_int		iOption = m_Cells[i]->Get_Option();
		out.write((_char*)&iOption, sizeof(_int));
	}

	out.close();

	Update_FileName();
}

HRESULT CNavigation_Manager::Load_Cells(_uint iIndex)
{
	for (auto& iter : m_Cells)
		Safe_Release(iter);
	m_Cells.clear();

	/* 새로 로드할 때는 index 다시 0으로 되돌리기*/
	m_iCurrentCellIndex = 0;

	_ulong		dwByte = {};
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/NaviData/");

	strcat_s(fullPath, "Navigation");

	char cIndex[2] = "";
	_itoa_s(iIndex, cIndex, 10);
	strcat_s(fullPath, "_");
	strcat_s(fullPath, cIndex);
	strcat_s(fullPath, ".dat");

	string strPath(fullPath);

	ifstream in(fullPath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("파일 개방 실패");
		return E_FAIL;
	}

	_uint NumCells = { 0 };
	in.read((_char*)&NumCells, sizeof(_uint));

	for (size_t i = 0; i < NumCells; ++i)
	{
		/* point 로드 */
		_float3* vPoint = new _float3[3];
		in.read((_char*)vPoint, sizeof(_float3) * 3);

		/* 옵션 로드 */
		_int iOption = { 0 };
		in.read((_char*)&iOption, sizeof(_int));

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoint, i, CCell::OPTION(iOption));

		m_Cells.emplace_back(pCell);
		Safe_Delete(vPoint);
	}

	in.close();

	/* option 설정 현재 셀에 맞게 */
	/* 현재 cell이 하나라도 있을때 option에 맞게 배치 */
	if (NumCells != 0)
	{
		m_iCurrentOption = m_Cells[m_iCurrentCellIndex]->Get_Option();
	}
	

	Update_CellsName();
}

void CNavigation_Manager::Show_FileName()
{
	ImGui::Text(u8" 네비게이션 리스트 ");

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
		return; // 파일없을때
	do
	{
		char* cfilename = new char[MAX_PATH];
		strcpy(cfilename, fd.name);
		m_FileNames.push_back(cfilename);
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

	/* 인덱스 옮겨주기 */
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

	/* 인덱스도 같이 초기화 */
	m_iCurrentCellIndex = 0;
}

void CNavigation_Manager::Find_Cells()
{
	/* F 누르면 작동 */
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
					/* 그냥 넣어주는거 */
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


_float3 CNavigation_Manager::Find_ClosestPoint(_vector pickPos, _float* pMinDistance)
{
	_float		minDistance;

	/* terrain보다 클 순 없으니 최대치로 잡기 */
	_float2		terrainSize = CTerrain_Manager::GetInstance()->Get_LandScale();
	terrainSize.x > terrainSize.y ? minDistance = terrainSize.x : minDistance = terrainSize.y;

	_float3		resultPos;

	/* 가장 가까운 점 찾기 - fDistance에는 거리 넣어주기*/
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

			/* 최저거리 넣기 */
			minDistance = fdistance;
			XMStoreFloat3(&resultPos, vPoint[i]);
		}
	}

	/* 최저거리 넣어주기 */
	*pMinDistance = minDistance;

	return resultPos;
}

_float3 CNavigation_Manager::Find_ClosestCells(_vector pickPos, _float* pMinDistance)
{
	return _float3();
}

void CNavigation_Manager::Show_Cells_IMGUI()
{
	ImGui::Text(u8" F 누르고 왼쪽 마우스 클릭하기 - CELL 찾기 ");
	ImGui::Text(u8" X 한번 누르고 클릭. 시계방향으로!!!!!!!!! - 점 생성 ");
	ImGui::Text(u8" 시계방향!!!!!!!!!!! ");

	Find_Cells();

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

		ImGui::Text(u8"네비게이션 옵션");
		static int naviOption = m_Cells[m_iCurrentCellIndex]->Get_Index();

		/* 다른 오브젝트 클릭시 */
		if (m_iCurrentCellIndex != cell_current_idx && m_Cells.size() > 0)
		{
			m_iCurrentCellIndex = cell_current_idx;
			m_iCurrentOption = m_Cells[m_iCurrentCellIndex]->Get_Option();
		}


		if (ImGui::RadioButton(u8"navi옵션1", m_iCurrentOption == 0))
		{
			naviOption = 0;
			m_iCurrentOption = 0;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton(u8"navi옵션2", m_iCurrentOption == 1))
		{
			naviOption = 1;
			m_iCurrentOption = 1;
		}

		m_Cells[m_iCurrentCellIndex]->Set_Option(static_cast<CCell::OPTION>(m_iCurrentOption));

		/*if (ImGui::Button(u8"네비 옵션 수정"))
		{
			m_Cells[m_iCurrentCellIndex]->Set_Option(static_cast<CCell::OPTION>(m_iCurrentOption));
		}*/

	}




	



	static int iNavIndex;
	ImGui::InputInt(u8"네비게이션 저장 index : ", &iNavIndex);

	if (ImGui::Button(u8"네비게이션 저장"))
	{
		Save_Cells(iNavIndex);
	}


	if (ImGui::Button(u8"셀 삭제"))
	{
		Delete_Cell(m_iCurrentCellIndex);
	}

	if (ImGui::Button(u8"셀 전체 삭제"))
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


void CNavigation_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pShader);

	for (auto& cell : m_Cells)
		Safe_Release(cell);

	m_Cells.clear();

	for (auto& iter : m_CellsName)
		Safe_Delete(iter);

	for (auto& iter : m_FileNames)
		Safe_Delete(iter);

	m_CellsName.clear();

	m_Cells.clear();

	Safe_Release(m_pGameInstance);
}
