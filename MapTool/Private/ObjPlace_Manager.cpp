
#include "ObjPlace_Manager.h"
#include "GameInstance.h"

#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"

#include "Construction.h"

#include <iostream>
#include <io.h>

char* StringToCharDIY(string s)
{
	_char chr[MAX_PATH];
	strcpy_s(chr, s.c_str());
	return chr;
}



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
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CObjPlace_Manager::Initialize()
{
	Load_ModelName();
	return S_OK;
}

void CObjPlace_Manager::Priority_Tick(const _float& fTimeDelta)
{
}

void CObjPlace_Manager::Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_GameObjects)
		iter.second->Tick(fTimeDelta);
}

void CObjPlace_Manager::Late_Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_GameObjects)
		iter.second->Late_Tick(fTimeDelta);
}

void CObjPlace_Manager::Render()
{
}

void CObjPlace_Manager::Set_GameObjectsList()
{
	//ImGui::Begin(u8"배치 오브젝트");

	//ImGui::Text(u8" 파일 이름 ");

	//if (m_FileNames.empty())
	//{
	//	Update_FileName();
	//}

	//static int layer_current_idx;
	//if (ImGui::BeginListBox("listbox 0"))
	//{
	//	for (int n = 0; n < m_FileNames.size(); n++)
	//	{
	//		const bool is_selected = (layer_current_idx == n);
	//		if (ImGui::Selectable(m_FileNames[n], is_selected))
	//			layer_current_idx = n;

	//		if (is_selected)
	//			ImGui::SetItemDefaultFocus();
	//	}
	//	ImGui::EndListBox();
	//}

	//static int iLevel;
	//ImGui::InputInt(u8"레벨 : ", &iLevel);

	//if (ImGui::Button(u8"맵오브젝트저장"))
	//{
	//	Save_GameObject(iLevel);
	//	Update_FileName();
	//}

	//if (ImGui::Button(u8"맵 정보 로드"))
	//{
	//	Load_GameObject(layer_current_idx);
	//}


	//if (0 < m_GameObjects.size())
	//{

	//	ImGui::Text(u8" 배치 오브젝트 리스트 ");

	//	list<string>	m_layer;

	//	static int layer_current_idx;
	//	if (ImGui::BeginListBox("listbox 1"))
	//	{
	//		for (int n = 0; n < m_ObjectNames.size(); n++)
	//		{
	//			const bool is_selected = (layer_current_idx == n);
	//			if (ImGui::Selectable(m_ObjectNames[n], is_selected))
	//				layer_current_idx = n;

	//			if (is_selected)
	//				ImGui::SetItemDefaultFocus();
	//		}
	//		ImGui::EndListBox();
	//	}


	//	Edit_GameObject(layer_current_idx);




	//	if (ImGui::Button(u8"object 삭제"))
	//	{
	//		Delete_Object(layer_current_idx);
	//		/* list 이름 업데이트 */
	//		Update_ObjectNameList();
	//	}


	//	if (ImGui::Button(u8"오브젝트 전체 삭제"))
	//	{
	//		for (auto& Pair : m_GameObjects)
	//			Safe_Release(Pair.second);

	//		m_GameObjects.clear();


	//		Update_ObjectNameList();
	//	}


	//}

	//ImGui::End();
}

void CObjPlace_Manager::Save_GameObject(int iLevel)
{
}

void CObjPlace_Manager::Load_GameObject(int iNum)
{
}

void CObjPlace_Manager::Edit_GameObject(int iNumObject)
{
	//auto& iter = m_GameObjects.begin();

	//if (0 != iNumObject)
	//{
	//	for (int i = 0; i < iNumObject; i++)
	//	{
	//		iter++;
	//	}
	//}

	//EditTransform((float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW),
	//	(float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ),
	//	(float*)iter->second->Get_TransformCom()->Get_WorldFloat4x4(),
	//	true);

}

bool CObjPlace_Manager::Add_CloneObject_Imgui(_uint iLayerIndex, _uint iObjectIndex)
{
	if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB))
	{
		_bool		isPick;
		_vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);

		_matrix			startPos;
		startPos = XMMatrixIdentity();
		startPos.r[3].m128_f32[0] = vTargetPos.m128_f32[0];
		startPos.r[3].m128_f32[1] = vTargetPos.m128_f32[1];
		startPos.r[3].m128_f32[2] = vTargetPos.m128_f32[2];
		startPos.r[3].m128_f32[3] = vTargetPos.m128_f32[3];

		CConstruction::MAPOBJ_DESC		mapDesc;
		mapDesc.vStartPos = startPos;

		//if (0 == iLayerIndex)
		//{
		//	gameDesc.iObjID = iObjectIndex;
		//	m_GameObjects.emplace(TEXT("Prototype_GameObject_Land"), CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Land"), &gameDesc));
		//}
		//else if (1 == iLayerIndex)
		//{
		//	gameDesc.iObjID = iObjectIndex;
		//	m_GameObjects.emplace(TEXT("Prototype_GameObject_Palm"), CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Palm"), &gameDesc));
		//}
		//else if (2 == iLayerIndex)
		//{
		//	gameDesc.iObjID = iObjectIndex;
		//	m_GameObjects.emplace(TEXT("Prototype_GameObject_Cliff"), CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Cliff"), &gameDesc));
		//}
		//else if (3 == iLayerIndex)
		//{
		//	m_GameObjects.emplace(m_Monsters[iObjectIndex], CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, m_Monsters[iObjectIndex], &gameDesc));
		//}
		//else if (4 == iLayerIndex)
		//{
		//	m_GameObjects.emplace(m_Objects[iObjectIndex], CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, m_Objects[iObjectIndex], &gameDesc));
		//}
		//else if (5 == iLayerIndex)
		//{
		//	gameDesc.iObjID = iObjectIndex;
		//	m_GameObjects.emplace(TEXT("Prototype_GameObject_LandCover"), CGameInstance::GetInstance()->Clone_Object(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_LandCover"), &gameDesc));
		//}

		//Update_ObjectNameList();

		return true;
	}

	return false;
}

void CObjPlace_Manager::Set_Map_Object()
{
	ImGui::Text(u8"LayerTag 이름");

	const char* pLayerArray[] = { "Map0", "Map1" };
	static int layer_current_idx = 0;
	if (ImGui::BeginListBox("listbox 1"))
	{
		for (int n = 0; n < IM_ARRAYSIZE(pLayerArray); n++)
		{
			const bool is_selected = (layer_current_idx == n);
			if (ImGui::Selectable(pLayerArray[n], is_selected))
				layer_current_idx = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	static int object_current_idx = 0;

	if (0 == layer_current_idx)
	{
		if (ImGui::BeginListBox(u8"모델0-엘베있는1층"))
		{
			for (int n = 0; n < m_ObjectNames_Map0.size(); n++)
			{
				const bool is_selected = (layer_current_idx == n);
				if (ImGui::Selectable(m_ObjectNames_Map0[n], is_selected))
					layer_current_idx = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}
	if (1 == layer_current_idx)
	{

		if (ImGui::BeginListBox(u8"모델1-엘베있는2층"))
		{
			for (int n = 0; n < m_ObjectNames_Map1.size(); n++)
			{
				const bool is_selected = (layer_current_idx == n);
				if (ImGui::Selectable(m_ObjectNames_Map1[n], is_selected))
					layer_current_idx = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}


	ImGui::NewLine();

	ImGui::Text(u8"오브젝트유형");
	static int a = 0;
	ImGui::RadioButton(u8"radio a", &a, 0);
	ImGui::RadioButton(u8"radio b", &a, 1);
	ImGui::RadioButton(u8"radio c", &a, 2);
	ImGui::RadioButton(u8"radio d", &a, 3);


	ImGui::NewLine();


	ImGui::Text(u8"쉐이더");
	static int e = 0;
	ImGui::RadioButton("radio a", &e, 0); 
	ImGui::RadioButton("radio b", &e, 1); 
	ImGui::RadioButton("radio c", &e, 2);
	ImGui::RadioButton("radio d", &e, 3);


	ImGui::NewLine();


	if (ImGui::Button(u8"Object 추가"))
	{
		m_bDoing_Place_Object = true;
	}

	if (true == m_bDoing_Place_Object)
	{
		bool    bcheckFinished = false;

		bcheckFinished = Add_CloneObject_Imgui(layer_current_idx, object_current_idx);

		if (true == bcheckFinished)
		{
			m_bDoing_Place_Object = false;
		}
	}
}

void CObjPlace_Manager::Load_ModelName()
{
	vector<string>		vObjectNames;
	
	/* map0 모델 로드*/
	m_pGameInstance->Get_FileNames("../../Client/Bin/Resources/Models/NonAnim/Map/Map0/Bin", vObjectNames);

	for (int i = 0; i < vObjectNames.size(); i++)
	{
		string modifiedString = modifyString(vObjectNames[i]);

		char* cfilename = new char[MAX_PATH];
		strcpy(cfilename, StringToCharDIY(modifiedString));
		m_ObjectNames_Map0.push_back(cfilename);
	}

	vObjectNames.clear();

	/* map1 모델 로드*/
	m_pGameInstance->Get_FileNames("../../Client/Bin/Resources/Models/NonAnim/Map/Map1/Bin", vObjectNames);

	for (int i = 0; i < vObjectNames.size(); i++)
	{
		string modifiedString = modifyString(vObjectNames[i]);

		char* cfilename = new char[MAX_PATH];
		strcpy(cfilename, StringToCharDIY(modifiedString));
		m_ObjectNames_Map1.push_back(cfilename);
	}
}

string CObjPlace_Manager::modifyString(string& input)
{
	// "."을 기준으로 문자열을 분리
	size_t pos = input.find('.');
	if (pos == std::string::npos) {
		return input; // "." 없음
	}

	std::string base = input.substr(0, pos);

	std::string newString = "Prototype_Component_Model_" + base;

	return newString;
}


void CObjPlace_Manager::Free()
{
	for (auto& Pair : m_GameObjects)
		Safe_Release(Pair.second);
	m_GameObjects.clear();

	for (auto& iter : m_ObjectNames_Map0)
		Safe_Delete(iter);
	m_ObjectNames_Map0.clear();

	for (auto& iter : m_ObjectNames_Map1)
		Safe_Delete(iter);
	m_ObjectNames_Map1.clear();

	for (auto& iter : m_FileNames)
		Safe_Delete(iter);
	m_FileNames.clear();

	for (auto& iter : m_Layers)
		Safe_Delete(iter);
	m_Layers.clear();

	for (auto& iter : m_ModelNames)
		Safe_Delete(iter);
	m_ModelNames.clear();

	Safe_Release(m_pGameInstance);
}
