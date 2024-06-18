
#include "ObjPlace_Manager.h"
#include "GameInstance.h"

#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"



#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"

#include <iostream>
#include <io.h>



bool useWindow = false;
int gizmoCount = 1;
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);


// TRANSFORM의 MATRIX
void EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
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
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		/* if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
			 mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;*/
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
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

		switch (mCurrentGizmoOperation)
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
	if (useWindow)
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
		// 이거만 신경쓰기 얼마만큼 기즈모 화면값
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	//격자
	//ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
	// 큐브 그려주는애
	//ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);

	// 옮기는애
	// 객체 잡고 동그라미좌표계 그려주는애
	// 객체 잡고 동그라미좌표계 그려주는애
	// matrix : 객체월드행ㄹ려
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	// 화면 상단 카메라 돌아가는애
	//ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);


	if (useWindow)
	{
		ImGui::End();
		// 이거해보기 터지면
		ImGui::PopStyleColor(1);
	}
}

IMPLEMENT_SINGLETON(CObjPlace_Manager)



CObjPlace_Manager::CObjPlace_Manager()
{
}

HRESULT CObjPlace_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return S_OK;
}

void CObjPlace_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CObjPlace_Manager::Tick(_float fTimeDelta)
{
	//for (auto& iter = m_GameObjects.begin(); iter != m_GameObjects.end(); iter++)
	//{
	//	iter->second->Tick(fTimeDelta);
	//}
}

void CObjPlace_Manager::Late_Tick(_float fTimeDelta)
{
	//for (auto& iter = m_GameObjects.begin(); iter != m_GameObjects.end(); iter++)
	//{
	//	iter->second->Late_Tick(fTimeDelta);
	//}
}

void CObjPlace_Manager::Render()
{
}


//void CObjPlace_Manager::Set_GameObjectsList()
//{
//	////ImGuizmo::BeginFrame(); //new와 같은것 선언해주기
//
//	//ImGui::Begin(u8"배치 오브젝트");
//
//	//ImGui::Text(u8" 파일 이름 ");
//
//	//if (m_FileNames.empty())
//	//{
//	//	Update_FileName();
//	//}
//
//	//static int layer_current_idx;
//	//if (ImGui::BeginListBox("listbox 0"))
//	//{
//	//	for (int n = 0; n < m_FileNames.size(); n++)
//	//	{
//	//		const bool is_selected = (layer_current_idx == n);
//	//		if (ImGui::Selectable(m_FileNames[n], is_selected))
//	//			layer_current_idx = n;
//
//	//		if (is_selected)
//	//			ImGui::SetItemDefaultFocus();
//	//	}
//	//	ImGui::EndListBox();
//	//}
//
//	//static int iLevel;
//	//ImGui::InputInt(u8"레벨 : ", &iLevel);
//
//	//if (ImGui::Button(u8"맵오브젝트저장"))
//	//{
//	//	Save_GameObject(iLevel);
//	//	Update_FileName();
//	//}
//
//	//if (ImGui::Button(u8"맵 정보 로드"))
//	//{
//	//	Load_GameObject(layer_current_idx);
//	//}
//
//
//	//if (0 < m_GameObjects.size())
//	//{
//
//	//	ImGui::Text(u8" 배치 오브젝트 리스트 ");
//
//	//	list<string>	m_layer;
//
//	//	static int layer_current_idx;
//	//	if (ImGui::BeginListBox("listbox 1"))
//	//	{
//	//		for (int n = 0; n < m_ObjectNames.size(); n++)
//	//		{
//	//			const bool is_selected = (layer_current_idx == n);
//	//			if (ImGui::Selectable(m_ObjectNames[n], is_selected))
//	//				layer_current_idx = n;
//
//	//			if (is_selected)
//	//				ImGui::SetItemDefaultFocus();
//	//		}
//	//		ImGui::EndListBox();
//	//	}
//
//
//	//	Edit_GameObject(layer_current_idx);
//
//
//
//
//	//	if (ImGui::Button(u8"object 삭제"))
//	//	{
//	//		Delete_Object(layer_current_idx);
//	//		/* list 이름 업데이트 */
//	//		Update_ObjectNameList();
//	//	}
//
//
//	//	if (ImGui::Button(u8"오브젝트 전체 삭제"))
//	//	{
//	//		for (auto& Pair : m_GameObjects)
//	//			Safe_Release(Pair.second);
//
//	//		m_GameObjects.clear();
//
//
//	//		Update_ObjectNameList();
//	//	}
//
//
//	//}
//
//	//ImGui::End();
//
//}
//
//
//void CObjPlace_Manager::Save_GameObject(int iLevel)
//{
//
//	///* 맵 저장 정보 저장 */
//	//MAP_TOTALINFORM_DESC pMapTotalInform;
//
//	//pMapTotalInform.iLevelIndex = iLevel;
//	//pMapTotalInform.vPlaneSize = CTerrain_Manager::GetInstance()->Get_LandScale();
//
//	//pMapTotalInform.iNumMapObj = m_GameObjects.size();
//	//pMapTotalInform.pMapObjDesc = new OBJECTPLACE_DESC[m_GameObjects.size()];
//
//	//int iIndex = 0;
//
//	//for (auto& iter = m_GameObjects.begin(); iter != m_GameObjects.end(); iter++)
//	//{
//	//	/* 오브젝트 id 받기 - Palm 같은 오브젝트 model 구별을 위해 */
//	//	pMapTotalInform.pMapObjDesc[iIndex].iObjID = iter->second->Get_ObjID();
//	//	XMStoreFloat4x4(&pMapTotalInform.pMapObjDesc[iIndex].vTransform, iter->second->Get_TransformCom()->Get_WorldMatrix());
//
//	//	if (true == Compare_Layer(iter->first, TEXT("Layer_Land")))
//	//	{
//	//		/* land object */
//	//		const wchar_t* layer = TEXT("Layer_Land");
//	//		string strlayer = TCHARToString(layer);
//	//		strcpy(pMapTotalInform.pMapObjDesc[iIndex].strLayer, StringToChar(strlayer));
//	//	}
//	//	else if (true == Compare_Layer(iter->first, TEXT("Layer_Palm")))
//	//	{
//	//		/* palm object */
//	//		const wchar_t* layer = TEXT("Layer_Palm");
//	//		string strlayer = TCHARToString(layer);
//	//		strcpy(pMapTotalInform.pMapObjDesc[iIndex].strLayer, StringToChar(strlayer));
//	//	}
//	//	else if (true == Compare_Layer(iter->first, TEXT("Layer_Cliff")))
//	//	{
//	//		/* cliff object */
//	//		const wchar_t* layer = TEXT("Layer_Cliff");
//	//		string strlayer = TCHARToString(layer);
//	//		strcpy(pMapTotalInform.pMapObjDesc[iIndex].strLayer, StringToChar(strlayer));
//	//	}
//	//	else if (true == Find_Layer(m_Monsters, iter->first))
//	//	{
//	//		/* monster object */
//	//		const wchar_t* layer = m_Layers[3].c_str();
//	//		string strlayer = TCHARToString(layer);
//	//		strcpy(pMapTotalInform.pMapObjDesc[iIndex].strLayer, StringToChar(strlayer));
//	//	}
//	//	else if (true == Find_Layer(m_Objects, iter->first))
//	//	{
//	//		/* object */
//	//		const wchar_t* layer = m_Layers[4].c_str();
//	//		string strlayer = TCHARToString(layer);
//	//		strcpy(pMapTotalInform.pMapObjDesc[iIndex].strLayer, StringToChar(strlayer));
//	//	}
//	//	else if (true == Find_Layer(m_GroundObjects, iter->first))
//	//	{
//	//		/* object */
//	//		const wchar_t* layer = m_Layers[4].c_str();
//	//		string strlayer = TCHARToString(layer);
//	//		strcpy(pMapTotalInform.pMapObjDesc[iIndex].strLayer, StringToChar(strlayer));
//	//	}
//
//	//	const wchar_t* objectname = iter->first.c_str();
//	//	string strobject = TCHARToString(objectname);
//	//	strcpy(pMapTotalInform.pMapObjDesc[iIndex].strPrototype, StringToChar(strobject));
//
//	//	iIndex++;
//	//}
//
//
//	//CMapDataMgr::GetInstance()->Save_Bin_Map_Data(&pMapTotalInform);
//
//	//Safe_Delete_Array(pMapTotalInform.pMapObjDesc);
//
//}
//
//void CObjPlace_Manager::Load_GameObject(int iNum)
//{
//
//	////string fileName = m_FileNames[iNum];
//
//	////// ".bin" 확장자 제거
//	////int pos = fileName.rfind('.');
//	////if (pos != std::string::npos) {
//	////	fileName = fileName.substr(0, pos);
//	////}
//
//	////string delimiter = "_";
//	////pos = 0;
//
//	////while ((pos = fileName.find(delimiter)) != string::npos) {
//	////	std::string token = fileName.substr(0, pos);
//	////	fileName.erase(0, pos + delimiter.length());
//	////}
//	////
//	////int iFileNum = stoi(fileName);
//
//
//	///* 맵 저장 정보 로드 */
//	//for (auto& Pair : m_GameObjects)
//	//	Safe_Release(Pair.second);
//	//m_GameObjects.clear();
//
//
//	//MAP_TOTALINFORM_DESC		mapTotalInform;
//	//CMapDataMgr::GetInstance()->Load_Bin_Map_Data_OnTool(&mapTotalInform, m_FileNames[iNum]);
//
//	//CTerrain_Manager::GetInstance()->Change_LandScale(mapTotalInform.vPlaneSize.x, mapTotalInform.vPlaneSize.y);
//
//	//for (int i = 0; i < mapTotalInform.iNumMapObj; i++)
//	//{
//	//	mapTotalInform.pMapObjDesc[i].strLayer;
//	//	string strLayer(mapTotalInform.pMapObjDesc[i].strLayer);
//	//	string strPrototype(mapTotalInform.pMapObjDesc[i].strPrototype);
//
//	//	CGameObject::GAMEOBJECT_DESC gameObjectDesc;
//	//	gameObjectDesc.StartPos = mapTotalInform.pMapObjDesc[i].vTransform;
//	//	gameObjectDesc.iObjID = mapTotalInform.pMapObjDesc[i].iObjID;
//
//	//	TCHAR* wLayer = StringToTCHAR(strLayer);
//	//	TCHAR* wPrototype = StringToTCHAR(strPrototype);
//
//	//	m_GameObjects.emplace(wPrototype, CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, wPrototype, &gameObjectDesc));
//
//	//	Safe_Delete(wLayer);
//	//	Safe_Delete(wPrototype);
//	//}
//
//	//for (int i = 0; i < mapTotalInform.iNumMapObj; i++)
//	//{
//	//	Safe_Delete(mapTotalInform.pMapObjDesc);
//	//}
//
//	//Update_ObjectNameList();
//	////Safe_Delete_Array(mapTotalInform.pMapObjDesc);
//
//}
//
//bool CObjPlace_Manager::Add_CloneObject_Imgui(_uint iLayerIndex, _uint iObjectIndex)
//{
//	if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB))
//	{
//		_bool		isPick;
//		_vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);
//
//		XMFLOAT4X4		startPos;
//		XMStoreFloat4x4(&startPos, XMMatrixIdentity());
//		startPos._41 = vTargetPos.m128_f32[0];
//		startPos._42 = vTargetPos.m128_f32[1];
//		startPos._43 = vTargetPos.m128_f32[2];
//		startPos._44 = vTargetPos.m128_f32[3];
//
//		CGameObject::GAMEOBJECT_DESC gameDesc;
//		gameDesc.StartPos = startPos;
//
//		if (0 == iLayerIndex)
//		{
//			gameDesc.iObjID = iObjectIndex;
//			m_GameObjects.emplace(TEXT("Prototype_GameObject_Land"), CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Land"), &gameDesc));
//		}
//		else if (1 == iLayerIndex)
//		{
//			gameDesc.iObjID = iObjectIndex;
//			m_GameObjects.emplace(TEXT("Prototype_GameObject_Palm"), CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Palm"), &gameDesc));
//		}
//		else if (2 == iLayerIndex)
//		{
//			gameDesc.iObjID = iObjectIndex;
//			m_GameObjects.emplace(TEXT("Prototype_GameObject_Cliff"), CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Cliff"), &gameDesc));
//		}
//		else if (3 == iLayerIndex)
//		{
//			m_GameObjects.emplace(m_Monsters[iObjectIndex], CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, m_Monsters[iObjectIndex], &gameDesc));
//		}
//		else if (4 == iLayerIndex)
//		{
//			m_GameObjects.emplace(m_Objects[iObjectIndex], CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, m_Objects[iObjectIndex], &gameDesc));
//		}
//		else if (5 == iLayerIndex)
//		{
//			gameDesc.iObjID = iObjectIndex;
//			m_GameObjects.emplace(TEXT("Prototype_GameObject_LandCover"), CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_LandCover"), &gameDesc));
//		}
//
//		Update_ObjectNameList();
//
//		return true;
//	}
//
//	return false;
//}
//
//bool CObjPlace_Manager::Find_Layer(vector<wstring> layer, wstring objname)
//{
//	bool	bFind = false;
//
//	for (auto& iter : layer)
//	{
//		const wchar_t* layer = iter.c_str();
//		const wchar_t* object = objname.c_str();
//
//		if (0 == wcscmp(layer, object))
//		{
//			bFind = true;
//			break;
//		}
//	}
//
//	return bFind;
//}
//
//bool CObjPlace_Manager::Compare_Layer(wstring layer, wstring objname)
//{
//	bool	bFind = false;
//
//
//	const wchar_t* layername = layer.c_str();
//	const wchar_t* objectname = objname.c_str();
//
//	if (0 == wcscmp(layername, objectname))
//	{
//		return true;
//	}
//
//
//	return bFind;
//}
//
//void CObjPlace_Manager::Delete_Object(int iNumObject)
//{
//	auto& objectnameiter = m_ObjectNames.begin();
//	auto& iter = m_GameObjects.begin();
//
//	if (0 != iNumObject)
//	{
//		for (int i = 0; i < iNumObject; i++)
//		{
//			iter++;
//			objectnameiter++;
//		}
//	}
//
//	Safe_Release(iter->second);
//	m_GameObjects.erase(iter);
//
//	m_ObjectNames.erase(objectnameiter);
//	Update_ObjectNameList();
//}
//
//
//
//void CObjPlace_Manager::Update_ObjectNameList()
//{
//	for (auto& iter : m_ObjectNames)
//		Safe_Delete(iter);
//
//	m_ObjectNames.clear();
//
//	int iIndex = 0;
//	for (auto& iter = m_GameObjects.begin(); iter != m_GameObjects.end(); iter++)
//	{
//		const wchar_t* layer = iter->first.c_str();
//		string strlayer = TCHARToString(layer);
//		char* clayertag = new char[MAX_PATH];
//		strcpy(clayertag, StringToChar(strlayer));
//
//		char buff[MAX_PATH];
//		sprintf(buff, "%d", iIndex);
//		strcat(clayertag, buff);
//		m_ObjectNames.push_back(clayertag);
//		iIndex++;
//	}
//}
//
//void CObjPlace_Manager::Edit_GameObject(int iNumObject)
//{
//	auto& iter = m_GameObjects.begin();
//
//	if (0 != iNumObject)
//	{
//		for (int i = 0; i < iNumObject; i++)
//		{
//			iter++;
//		}
//	}
//
//	EditTransform((float*)CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW),
//		(float*)CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ),
//		(float*)iter->second->Get_TransformCom()->Get_WorldFloat4x4(),
//		true);
//
//
//}
//
//void CObjPlace_Manager::Show_FileName()
//{
//	ImGui::Text(u8" 배치 오브젝트 리스트 ");
//
//	if (m_FileNames.empty())
//	{
//		Update_FileName();
//	}
//
//	static int layer_current_idx;
//	if (ImGui::BeginListBox("listbox 1"))
//	{
//		for (int n = 0; n < m_FileNames.size(); n++)
//		{
//			const bool is_selected = (layer_current_idx == n);
//			if (ImGui::Selectable(m_FileNames[n], is_selected))
//				layer_current_idx = n;
//
//			if (is_selected)
//				ImGui::SetItemDefaultFocus();
//		}
//		ImGui::EndListBox();
//	}
//
//}
//
//void CObjPlace_Manager::Update_FileName()
//{
//	for (auto& iter : m_FileNames)
//		Safe_Delete(iter);
//
//	m_FileNames.clear();
//	string path = "../../../Client/Bin/Data/MapData/*.bin";
//
//	struct _finddata_t fd;
//	intptr_t handle;
//
//	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
//		return; // 파일없을때
//	do
//	{
//		char* cfilename = new char[MAX_PATH];
//		strcpy(cfilename, StringToChar(fd.name));
//		m_FileNames.push_back(cfilename);
//	} while (_findnext(handle, &fd) == 0);
//	_findclose(handle);
//}



void CObjPlace_Manager::Free()
{
	//for (auto& Pair : m_GameObjects)
	//	Safe_Release(Pair.second);
	//m_GameObjects.clear();

	//for (auto& iter : m_ObjectNames)
	//	Safe_Delete(iter);

	//for (auto& iter : m_FileNames)
	//	Safe_Delete(iter);


	//m_Layers.clear();
	//m_Lands.clear();
	//m_Palms.clear();
	//m_Cliffs.clear();
	//m_Monsters.clear();
	//m_Objects.clear();

	//m_ObjectNames.clear();
}
