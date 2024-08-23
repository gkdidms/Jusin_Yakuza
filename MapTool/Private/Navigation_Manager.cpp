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

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"


bool useWindowN = false;
int gizmoCountN = 1;
static ImGuizmo::OPERATION mCurrentGizmoOperationN(ImGuizmo::TRANSLATE);


// TRANSFORM�� MATRIX
void EditTransformN(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
{
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	editTransformDecomposition = true;
	if (editTransformDecomposition)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_T))
			mCurrentGizmoOperationN = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGizmoOperationN = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
			mCurrentGizmoOperationN = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperationN == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperationN = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperationN == ImGuizmo::ROTATE))
			mCurrentGizmoOperationN = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperationN == ImGuizmo::SCALE))
			mCurrentGizmoOperationN = ImGuizmo::SCALE;
		/* if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
			 mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;*/
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

		if (mCurrentGizmoOperationN != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_S))
			useSnap = !useSnap;
		ImGui::Checkbox("##UseSnap", &useSnap);
		ImGui::SameLine();

		switch (mCurrentGizmoOperationN)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &snap[0]);
			break;
		}
		ImGui::Checkbox("Bound Sizing", &boundSizing);
		if (boundSizing)
		{
			ImGui::PushID(3);
			ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
			ImGui::SameLine();
			ImGui::InputFloat3("Snap", boundsSnap);
			ImGui::PopID();
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;
	if (useWindowN)
	{
		ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
		ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
		viewManipulateTop = ImGui::GetWindowPos().y;
		//ImGuiWindow* window = ImGui::GetCurrentWindow();
		//gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
	}
	else
	{
		// �̰Ÿ� �Ű澲�� �󸶸�ŭ ����� ȭ�鰪
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	//����
	//ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
	// ť�� �׷��ִ¾�
	//ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);

	// �ű�¾�
	// ��ü ��� ���׶����ǥ�� �׷��ִ¾�
	// ��ü ��� ���׶����ǥ�� �׷��ִ¾�
	// matrix : ��ü�����ऩ��
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperationN, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	// ȭ�� ��� ī�޶� ���ư��¾�
	//ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);


	if (useWindowN)
	{
		ImGui::End();
		// �̰��غ��� ������
		ImGui::PopStyleColor(1);
	}
}



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
	if (true == m_bRouteAllView)
	{

		int iCurRoute = 0;

		for (auto& pair : m_Routes)
		{
			for (auto& iter : pair.second)
			{
				// m_iCurrentRouteNum : ���� route ��ȣ
				if (iCurRoute != m_iCurrentRouteNum)
				{
					iter->Set_Color(1);
					iter->Late_Tick(fTimeDelta);


				}
			}

			iCurRoute++;
		}



		for (auto& iter : m_Route_CellIndexes)
		{
			iter->Late_Tick(fTimeDelta);
			iter->Set_Color(3);


		}
	}
	else
	{
		int iIndex = 0;

		for (auto& iter : m_Route_CellIndexes)
		{
			iter->Late_Tick(fTimeDelta);
			iter->Set_Color(3);

		}
	}
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

	if (0 != m_vLineBufferLine.size())
	{
		for (int i = 0; i < m_vLineBufferLine.size(); i++)
		{
			XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
			m_WorldMatrix.m[3][1] += 0.2f;
			m_pShaderLineCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
			m_pShaderLineCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
			m_pShaderLineCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

			_vector vColor = { 0.f, 0.f, 1.f, 1.f };
			m_pShaderLineCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector));

			m_pShaderLineCom->Begin(0);
			m_vLineBufferLine[i]->Render();
		}
		
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

	if (m_iCurrentRouteNum != route_layer_current_idx)
	{
		m_iCurrentRouteNum = route_layer_current_idx;
		m_iCurrentRouteCellIndex = 0;
		Load_Route(m_iCurrentRouteNum);
		Update_PointRouteNumName();
	}


	if (ImGui::Button(u8"Route ���� "))
	{
		Delete_Route(route_layer_current_idx);
	}


	ImGui::Text(u8"�̹� ������� route�� ���� �����Ҷ� �� �����ֱ� - �׷����� �ð������� ����");
	if (ImGui::Button(u8" route cell ���� "))
	{
		for (auto& iter : m_Routes[route_layer_current_idx])
		{
			Safe_Release(iter);
		}

		m_Routes[route_layer_current_idx].clear();

		for (auto& iter : m_Route_CellIndexes)
		{
			Safe_AddRef(iter);
			m_Routes[route_layer_current_idx].push_back(iter);
		}
		

		Update_CellIndex_Draw();
		Update_Route_Draw();
		
	}

	// �ٺ��̰Բ�
	static int RouteAllView = 1;
	if (ImGui::RadioButton(u8"Route ���� ���� - O", RouteAllView == 0))
	{
		RouteAllView = 0;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"Route ���� ���� - X", RouteAllView == 1))
	{
		RouteAllView = 1;
		m_bRouteAllView = false;
		for (auto& iter : m_vLineBufferLine)
			Safe_Release(iter);
		m_vLineBufferLine.clear();

	}

	if (0 == RouteAllView && false == m_bRouteAllView)
	{
		m_bRouteAllView = true;

		for (int i = 0; i < m_Routes.size(); i++)
		{
			CVIBuffer_Line*		pBuffer = CVIBuffer_Line::Create(m_pDevice, m_pContext, m_Routes_Draw[i]);
			if (nullptr == pBuffer)
				MSG_BOX("VIBuffer_Lint ���� �Ұ�");

			m_vLineBufferLine.push_back(pBuffer);
		}
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
	if (0.7f < minDistance)
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
			arr[j] = m_Routes.find(i)->second[j]->Get_RouteIO();
		}

		//out.write(reinterpret_cast<char*>(arr), iCellCnt * sizeof(ROUTE_IO));

		for (int j = 0; j < iCellCnt; j++)
		{
			//NaviIndex ã�� ���
			_vector		vPos = XMLoadFloat4(&arr[j].vPosition);
			int		iIndex = Get_Player_Monster_NaviIndex(vPos);

			if (-1 == iIndex)
				iIndex = 0;

			out.write((char*)&iIndex, sizeof(int));
			out.write((_char*)&arr[j].vPosition, sizeof(_float4));
			out.write((char*)&arr[j].iPointOption, sizeof(int));

			// ��Ʈ �ε��� ����
			out.write((char*)&arr[j].iRouteNums, sizeof(int));

			if (0 < arr[j].iRouteNums)
			{
				for (int k = 0; k < arr[j].iRouteNums; k++)
				{
					out.write((char*)&arr[j].pRouteID[k], sizeof(int));
				}
			}

		}
		

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


	for (auto& iter : m_IndexesName)
		Safe_Delete(iter);
	m_IndexesName.clear();

	for (auto& iter : m_RouteName)
		Safe_Delete(iter);
	m_RouteName.clear();

	Safe_Release(m_pVIBufferCom);

	for (auto& iter : m_vLineBufferLine)
		Safe_Release(iter);
	m_vLineBufferLine.clear();



	for (auto& iter : m_Route_CellIndexes)
		Safe_Release(iter);
	m_Route_CellIndexes.clear();

	for (auto& iter : m_Routes)
	{
		for (auto& pair : iter.second)
			Safe_Release(pair);

		iter.second.clear();
	}
	m_Routes.clear();


	m_Route_CellIndexes_Draw.clear();

	for (auto& Pair : m_Routes_Draw)
		Pair.second.clear();
	m_Routes_Draw.clear();


	/* ���� �ε��� ���� index �ٽ� 0���� �ǵ�����*/
	m_iCurrentCellIndex = 0;
	m_iCurrentRouteCellIndex = 0;
	m_iCurrentRouteNum = 0;

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

		for (int j = 0; j < iCellCnt; j++)
		{
			in.read((char*)&arr[j].iCellNums, sizeof(int));
			in.read((_char*)&arr[j].vPosition, sizeof(_float4));
			in.read((char*)&arr[j].iPointOption, sizeof(int));

			// ��Ʈ �ε��� ����
			in.read((char*)&arr[j].iRouteNums, sizeof(int));

			if (0 < arr[j].iRouteNums)
			{
				arr[j].pRouteID = new int[arr[j].iRouteNums];

				for (int k = 0; k < arr[j].iRouteNums; k++)
				{
					in.read((char*)&arr[j].pRouteID[k], sizeof(int));
				}
			}
		}


		vector<CNaviObj*>		routeObjs;

		for (int j = 0; j < iCellCnt; j++)
		{
			CNaviObj::NAVIOBJ_DESC naviobjdesc;
			naviobjdesc.tNaviDesc.iCellNums = arr[j].iCellNums;
			naviobjdesc.tNaviDesc.vPosition = arr[j].vPosition;
			naviobjdesc.tNaviDesc.iPointOption = arr[j].iPointOption;
			naviobjdesc.tNaviDesc.iRouteNums = arr[j].iRouteNums;
			naviobjdesc.tNaviDesc.pRouteID = arr[j].pRouteID;

			XMMATRIX startPos = XMMatrixIdentity();
			startPos.r[3].m128_f32[0] = arr[j].vPosition.x;
			startPos.r[3].m128_f32[1] = arr[j].vPosition.y;
			startPos.r[3].m128_f32[2] = arr[j].vPosition.z;
			startPos.r[3].m128_f32[3] = arr[j].vPosition.w;

			naviobjdesc.vStartPos = startPos;

			routeObjs.push_back(dynamic_cast<CNaviObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_NaviObj"), &naviobjdesc)));
		}

		m_Routes.emplace(i, routeObjs);



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
	Load_Route(m_iCurrentRouteNum);

	Update_CellIndex_Draw();
	Update_Route_Draw();
}


HRESULT CNavigation_Manager::Load_Route(_uint iIndex)
{

	if (iIndex < m_Routes.size())
	{
		for (auto& iter : m_Route_CellIndexes)
			Safe_Release(iter);
		m_Route_CellIndexes.clear();


		m_Route_CellIndexes_Draw.clear();




		int iNums = m_Routes.find(iIndex)->second.size();

		for (int i = 0; i < iNums; i++)
		{
			Safe_AddRef(m_Routes.find(iIndex)->second[i]);
			m_Route_CellIndexes.push_back(m_Routes.find(iIndex)->second[i]);
		}

		if (0 < m_Route_CellIndexes_Draw.size())
		{
			Safe_Release(m_pVIBufferCom);

			for (auto& iter : m_vLineBufferLine)
				Safe_Release(iter);
			m_vLineBufferLine.clear();

			m_pVIBufferCom = CVIBuffer_Line::Create(m_pDevice, m_pContext, m_Route_CellIndexes_Draw);
			if (nullptr == m_pVIBufferCom)
				MSG_BOX("VIBuffer_Lint ���� �Ұ�");
		}

		Update_IndexesName();
	}

	Update_CellIndex_Draw();
	Update_Route_Draw();
	
	iIndex = 0;

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
	vector<CNaviObj*>::iterator	iter = m_Route_CellIndexes.begin();

	for (int i = 0; i < iIndex; i++)
	{
		iter++;
	}

	Safe_Release(*iter);
	m_Route_CellIndexes.erase(iter);


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
	Update_CellIndex_Draw();

	Update_Route_Draw();

}

void CNavigation_Manager::Delete_AllRouteCell(_int& iIndex)
{

	for (auto& iter : m_Route_CellIndexes)
		Safe_Release(iter);
	m_Route_CellIndexes.clear();


	Update_IndexesName();
	Update_CellIndex_Draw();
	Update_Route_Draw();

	/* �ε����� ���� �ʱ�ȭ */
	iIndex = 0;
}

int CNavigation_Manager::Get_Player_Monster_NaviIndex(_vector vPosition)
{
	for (int i = 0; i < m_Cells.size(); i++)
	{
		/* �׳� �־��ִ°� */
		int			iNeighborsIndex;
		XMVECTOR		vSlidingNormal;
		if (true == m_Cells[i]->isIn(vPosition, &iNeighborsIndex, vSlidingNormal))
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
					XMVECTOR	vSlidingNormal;
					if (true == m_Cells[i]->isIn(vTargetPos, &iNeighborsIndex, vSlidingNormal))
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
	if (true == m_bMakeRoute_IMGUI && 0 < m_Cells.size())
	{
		ImGui::Begin(u8" Route Num ");
		ImGui::Text(u8" 1. Cell ã�� - F ������ Ŭ�� - ������ ������ �ٶ󺸰Բ� ");
		ImGui::Text(u8" 2. ���ϴ� cell�� ���õ����� cell - route�� �߰� ��ư ������ ");
		ImGui::NewLine();

		ImGui::Text(u8" ó�� �Է� ���� ��ȣ ");
		static int iNewPointOpiton = 0;
		if (ImGui::RadioButton(u8"������", iNewPointOpiton == 0))
		{
			iNewPointOpiton = 0;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton(u8"�ڳ�", iNewPointOpiton == 1))
		{
			iNewPointOpiton = 1;
		}

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

		Click_To_Select_Object(index_current_idx);

		// �ٸ� �� ����
		if (m_iCurrentRouteCellIndex != index_current_idx && 0 < m_Route_CellIndexes.size())
		{
			m_iCurrentRouteCellIndex = index_current_idx;
			m_iPointOption = m_Route_CellIndexes[m_iCurrentRouteCellIndex]->Get_PointOpiton();


			Update_PointRouteNumName();
		}



		Edit_GameObject_Transform(m_iCurrentRouteCellIndex);


		ImGui::Text(u8" ����Ȯ�� + ������ ");
		static int iPointOpiton = m_iPointOption;
		if (ImGui::RadioButton(u8"������ - ������", m_iPointOption == 0))
		{
			iPointOpiton = 0;
			m_iPointOption = 0;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton(u8"�ڳ� - ������", m_iPointOption == 1))
		{
			iPointOpiton = 1;
			m_iPointOption = 1;
		}

		static int point_route_Num = 0;

		// point�� �Ѿ ��Ʈ ��ȣ �����ϱ�
		if (ImGui::BeginListBox(u8"point�� ��Ʈ����"))
		{
			for (int n = 0; n < m_PointRouteSaveNumName.size(); n++)
			{
				const bool is_selected = (point_route_Num == n);
				if (ImGui::Selectable(m_PointRouteSaveNumName[n], is_selected))
					point_route_Num = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		static int iRouteNum = 0;
		ImGui::InputInt(u8"��Ʈ��ȣ �Է� : ", &iRouteNum);

		if (ImGui::Button(u8"����Ʈ�� ��Ʈ��ȣ ����"))
		{
			if (m_iCurrentRouteCellIndex < m_Route_CellIndexes.size())
			{
				m_Route_CellIndexes[m_iCurrentRouteCellIndex]->Add_RouteNum_InVec(iRouteNum);
				Update_PointRouteNumName();
			}
		}
		
		if (ImGui::Button(u8"����Ʈ - ��Ʈ��ȣ ����"))
		{
			if (m_iCurrentRouteCellIndex < m_Route_CellIndexes.size())
			{
				m_Route_CellIndexes[m_iCurrentRouteCellIndex]->Delete_RouteNum_InVec(point_route_Num);
				Update_PointRouteNumName();
			}
			
		}

		if (ImGui::Button(u8"����Ʈ - ��Ʈ��ȣ ��ü����"))
		{
			if (m_iCurrentRouteCellIndex < m_Route_CellIndexes.size())
			{
				m_Route_CellIndexes[m_iCurrentRouteCellIndex]->DeleteAll_RouteNum_InVec();
				Update_PointRouteNumName();
			}	
		}


		if (0 < m_Route_CellIndexes.size())
		{
			if (m_iCurrentRouteCellIndex < m_Route_CellIndexes.size())
			{
				m_Route_CellIndexes[m_iCurrentRouteCellIndex]->Set_PointOption(m_iPointOption);
			}
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
				XMVECTOR	vSlidingNormal;
				if (true == m_Cells[i]->isIn(vTargetPos, &iNeighborsIndex, vSlidingNormal))
				{
					m_iCurrentCellIndex = i;
				}
			}

			CNaviObj::NAVIOBJ_DESC naviobjdesc;
			naviobjdesc.tNaviDesc.iCellNums = m_iCurrentCellIndex;
			naviobjdesc.tNaviDesc.iPointOption = iNewPointOpiton;
			XMStoreFloat4(&naviobjdesc.tNaviDesc.vPosition, vTargetPos);

			XMMATRIX startPos = XMMatrixIdentity();
			startPos.r[3].m128_f32[0] = vTargetPos.m128_f32[0];
			startPos.r[3].m128_f32[1] = vTargetPos.m128_f32[1];
			startPos.r[3].m128_f32[2] = vTargetPos.m128_f32[2];
			startPos.r[3].m128_f32[3] = vTargetPos.m128_f32[3];

			naviobjdesc.vStartPos = startPos;

			m_Route_CellIndexes.push_back(dynamic_cast<CNaviObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_NaviObj"), &naviobjdesc)));
			Safe_AddRef(m_Cells[m_iCurrentCellIndex]);

			m_iCurrentRouteCellIndex = m_Route_CellIndexes.size() - 1;
			m_iPointOption = m_Route_CellIndexes[m_iCurrentRouteCellIndex]->Get_PointOpiton();

			// route ���� index update
			Update_IndexesName();
			
			Update_CellIndex_Draw();
			Update_Route_Draw();

			if (2 <= m_Route_CellIndexes.size())
			{
				Safe_Release(m_pVIBufferCom);


				m_pVIBufferCom = CVIBuffer_Line::Create(m_pDevice, m_pContext, m_Route_CellIndexes_Draw);
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
			Update_CellIndex_Draw();
			Update_Route_Draw();

			Safe_Release(m_pVIBufferCom);

			for (auto& iter : m_vLineBufferLine)
				Safe_Release(iter);
			m_vLineBufferLine.clear();
		}

		if (ImGui::Button(u8"�׺� ��Ʈ�߰�"))
		{
			Add_Route_In_Navi();
			Update_RouteName();
		}

		ImGui::NewLine();


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
	vector<CNaviObj*>		routes;
	
	for (int i = 0; i < m_Route_CellIndexes.size(); i++)
	{
		Safe_AddRef(m_Route_CellIndexes[i]);
		routes.push_back(m_Route_CellIndexes[i]);
	}

	m_Routes.emplace(m_Routes.size(), routes);

	Update_Route_Draw();
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
				const bool is_selected = (cell_current_idx == n);
				if (ImGui::Selectable(m_CellsName[n], is_selected))
				{
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
			sprintf(buff, "%d", m_Route_CellIndexes[i]->Get_RouteIO().iCellNums);
			strcat(szName, buff);
			char buff_idx[MAX_PATH];
			sprintf(buff_idx, "%d", i);
			strcat(szName, buff_idx);

			m_IndexesName.push_back(szName);

			m_Route_CellIndexes[i]->Set_ObjID(i);
		}
	}

}

void CNavigation_Manager::Update_PointRouteNumName()
{
	if (0 < m_PointRouteSaveNumName.size())
	{
		for (auto& iter : m_PointRouteSaveNumName)
			Safe_Delete(iter);

		m_PointRouteSaveNumName.clear();
	}


	if (0 < m_Route_CellIndexes[m_iCurrentRouteCellIndex]->Get_VecNum())
	{
		for (int i = 0; i < m_Route_CellIndexes[m_iCurrentRouteCellIndex]->Get_RouteNumsVec().size(); i++)
		{
			char* szName = new char[MAX_PATH];
			strcpy(szName, "RouteNum");
			char buff[MAX_PATH];
			sprintf(buff, "%d", m_Route_CellIndexes[m_iCurrentRouteCellIndex]->Get_RouteNumsVec()[i]);
			strcat(szName, buff);

			m_PointRouteSaveNumName.push_back(szName);

		}
	}
}

void CNavigation_Manager::Update_Routes()
{
	map<int, vector<class CNaviObj*>> new_m_Routes;
	int new_key = 0;
	for (const auto& pair : m_Routes) {
		new_m_Routes[new_key] = pair.second;
		new_key++;
	}

	m_Routes = move(new_m_Routes);

}

void CNavigation_Manager::Delete_Route(_int& iIndex)
{
	if (m_Routes.size() > 0)
	{
		map<int, vector<class CNaviObj*>>::iterator iter = m_Routes.begin();

		for (int i = 0; i < iIndex; i++)
		{
			iter++;
		}

		for (auto& i : iter->second)
		{
			Safe_Release(i);
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

		Update_CellIndex_Draw();
		Update_Route_Draw();
	}
	
	m_iCurrentRouteCellIndex = 0;
}

void CNavigation_Manager::Update_CellIndex_Draw()
{
	// �׸��� ���� ���� ������Ʈ
	m_Route_CellIndexes_Draw.clear();

	Safe_Release(m_pVIBufferCom);

	for (auto& iter : m_Route_CellIndexes)
	{
		m_Route_CellIndexes_Draw.push_back(iter->Get_RouteIO());
	}

	m_pVIBufferCom = CVIBuffer_Line::Create(m_pDevice, m_pContext, m_Route_CellIndexes_Draw);
	if (nullptr == m_pVIBufferCom)
		MSG_BOX("VIBuffer_Lint ���� �Ұ�");

}

void CNavigation_Manager::Update_Route_Draw()
{
	// �׸��� ���� ���� ������Ʈ
	for (auto& Pair : m_Routes_Draw)
		Pair.second.clear();
	m_Routes_Draw.clear();

	int index = 0;

	for (auto& pair : m_Routes)
	{
		vector<ROUTE_IO>	routeIO;
		for (auto& iter : pair.second)
		{
			routeIO.push_back(iter->Get_RouteIO());
		}

		m_Routes_Draw.emplace(index, routeIO);

		index++;
	}

}

void CNavigation_Manager::Edit_GameObject_Transform(int iNumObject)
{
	if (0 < m_Route_CellIndexes.size())
	{
		vector<CNaviObj*>::iterator iter = m_Route_CellIndexes.begin();

		if (iNumObject > m_Route_CellIndexes.size())
			return;

		if (0 != iNumObject)
		{
			for (int i = 0; i < iNumObject; i++)
			{
				iter++;
			}
		}

		EditTransformN((float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW),
			(float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ),
			(float*)dynamic_cast<CGameObject*>(*iter)->Get_TransformCom()->Get_WorldFloat4x4(),
			true);
	}
	
}

void CNavigation_Manager::Click_To_Select_Object(int& iObjectNum)
{
	if (HOLD == m_pGameInstance->GetKeyState(DIK_G))
	{
		m_bFindObject = true;
	}
	else
	{
		m_bFindObject = false;
	}

	if (true == m_bFindObject)
	{
		m_bFindObject = false;

		_bool		isPick;

		_float		iIndex = 0;
		iIndex = m_pGameInstance->FindObjID(&isPick);

		if (iIndex > 0)
		{
			/* ID�� 1���� �����ϴϱ� INDEX �������� -1 ���ֱ� */
			iObjectNum = iIndex - 1;

		}

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

	m_vPoints.clear();

	for (auto& iter : m_FileNames)
		Safe_Delete(iter);
	m_FileNames.clear();

	for (auto& iter : m_RouteFileNames)
		Safe_Delete(iter);
	m_RouteFileNames.clear();

	for (auto& iter : m_CellsName)
		Safe_Delete(iter);
	m_CellsName.clear();

	for (auto& iter : m_IndexesName)
		Safe_Delete(iter);
	m_IndexesName.clear();

	for (auto& iter : m_RouteName)
		Safe_Delete(iter);
	m_RouteName.clear();


	Safe_Release(m_pVIBufferCom);

	for (auto& iter : m_vLineBufferLine)
		Safe_Release(iter);
	m_vLineBufferLine.clear();


	for (auto& iter : m_Route_CellIndexes)
		Safe_Release(iter);
	m_Route_CellIndexes.clear();

	for (auto& iter : m_Routes)
	{
		for (auto& pair : iter.second)
			Safe_Release(pair);

		iter.second.clear();
	}
	m_Routes.clear();


	m_Route_CellIndexes_Draw.clear();


	for (auto& Pair : m_Routes_Draw)
	{
		Pair.second.clear();
	}
	m_Routes_Draw.clear();


	Safe_Release(m_pGameInstance);
}
