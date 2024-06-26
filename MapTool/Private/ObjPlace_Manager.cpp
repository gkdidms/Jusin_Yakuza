
#include "ObjPlace_Manager.h"
#include "GameInstance.h"

#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"

#include "MapDataMgr.h"
#include "Terrain_Manager.h"

#include <iostream>
#include <io.h>

char* StringToCharDIY(string s)
{
	_char chr[MAX_PATH];
	strcpy_s(chr, s.c_str());
	return chr;
}

string TCHARToString(const TCHAR* ptsz)
{
	int len = (int)wcslen((wchar_t*)ptsz);
	char* psz = new char[2 * len + 1];
	wcstombs(psz, (wchar_t*)ptsz, 2 * len + 1);
	std::string s = psz;
	delete[] psz;
	return s;
}

char* StringToChar(string s)
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
	//for (auto& iter : m_GameObjects)
	//	iter->Tick(fTimeDelta);

	for (auto& iter : m_GameObjects)
		iter.second->Tick(fTimeDelta);

	if (nullptr != m_pShownObject)
	{
		m_pShownObject->Tick(fTimeDelta);
	}

}

void CObjPlace_Manager::Late_Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_GameObjects)
		iter.second->Late_Tick(fTimeDelta);

	if (nullptr != m_pShownObject)
	{
		m_pShownObject->Late_Tick(fTimeDelta);
	}
}

void CObjPlace_Manager::Render()
{
}

void CObjPlace_Manager::Show_Installed_GameObjectsList()
{
	ImGui::Begin(u8"배치 오브젝트");
	
	ImGui::Text(u8" 파일 이름 ");

	if (m_FileNames.empty())
	{
		Update_FileName();
	}

	/* 맵 저장 파일 관련 */
	static int layer_current_idx;
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


	static int iLevel;
	ImGui::InputInt(u8"레벨 : ", &iLevel);

	if (ImGui::Button(u8"맵오브젝트저장"))
	{
		Save_GameObject(iLevel);
		Update_FileName();
	}

	if (ImGui::Button(u8"맵 정보 로드"))
	{
		Load_GameObject(layer_current_idx);

	}


	/* 오브젝트 관련 */
	if (0 < m_GameObjects.size())
	{
		ImGui::Text(u8" 배치 오브젝트 리스트 ");

		list<string>	m_layer;

		static int object_current_idx = 0;

		if (m_iCurrentObjectIndex != object_current_idx)
		{
			/* 밑에서 수정이 생겼을 경우 */
			object_current_idx = m_iCurrentObjectIndex;
			Get_CurrentGameObject_Desc(&m_tCurrentObjectDesc, m_iCurrentObjectIndex);
		}

		if (ImGui::BeginListBox("listbox 1"))
		{
			for (int n = 0; n < m_ObjectNames.size(); n++)
			{
				const bool is_selected = (object_current_idx == n);
				if (ImGui::Selectable(m_ObjectNames[n], is_selected))
					object_current_idx = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		int iLayer = Click_To_Select_Object();


		if (iLayer >= 0)
		{
			object_current_idx = iLayer;
		}

		/* 다른 오브젝트 클릭시 */
		if (m_iCurrentObjectIndex != object_current_idx)
		{
			m_iCurrentObjectIndex = object_current_idx;
			Get_CurrentGameObject_Desc(&m_tCurrentObjectDesc, m_iCurrentObjectIndex);
		}

		/* 구조체 한번 더 업데이트 해줘야하는지 파악 */
		bool		bChange = false;

		if (ImGui::Button(u8"object 삭제"))
		{
			Delete_Object(m_iCurrentObjectIndex);
			/* list 이름 업데이트 */
			Update_ObjectNameList();
			bChange = true;
		}

		if (ImGui::Button(u8"오브젝트 전체 삭제"))
		{
			for (auto& Pair : m_GameObjects)
				Safe_Release(Pair.second);

			m_GameObjects.clear();

			object_current_idx = 0;
			m_iCurrentObjectIndex = 0;

			Update_ObjectNameList();
		}
		

		/* index는 같아도 다른 오브젝트로 됐을때 */
		if (bChange == true && !m_GameObjects.empty())
		{
			Get_CurrentGameObject_Desc(&m_tCurrentObjectDesc, m_iCurrentObjectIndex);
		}

		if (!m_GameObjects.empty())
		{
			ImGui::NewLine();

			static int LayerType = 0;
			if (ImGui::RadioButton(u8"오브젝트 수정", LayerType == 0))
			{
				LayerType = 0;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton(u8"데칼 수정", LayerType == 1))
			{
				LayerType = 1;
			}

			ImGui::NewLine();

			if (LayerType == 0)
			{
				m_bAddDecal_IMGUI = false;
				Edit_Installed_GameObject(m_iCurrentObjectIndex);

				if (ImGui::Button(u8"맵 오브젝트 수정"))
				{
					Edit_CurrentGameObject_Desc(&m_tCurrentObjectDesc, object_current_idx);
				}
			}
			else if (LayerType == 1)
			{
				m_bAddDecal_IMGUI = true;

				if (ImGui::Button(u8"데칼 수정"))
				{
					Get_Decal_Texture(object_current_idx);
				}
			}
			
		}
		else if (m_GameObjects.empty())
		{
			/* 초기화 */
			object_current_idx = 0;
			m_iCurrentObjectIndex = 0;
		}


		ImGui::NewLine();
		ImGui::NewLine();
			
#pragma region 안씀
		///* 데이터 추가할때마다 수정 */
		//if (0 < m_GameObjects.size())
		//{
		//	
		//	ImGui::Text(u8"Layer");
		//	static int LayerType = m_tCurrentObjectDesc.iLayer;
		//	if (ImGui::RadioButton(m_Layers[0], m_tCurrentObjectDesc.iLayer == 0))
		//	{
		//		LayerType = 0;
		//		m_tCurrentObjectDesc.iLayer = 0;
		//	}

		//	if (ImGui::RadioButton(m_Layers[1], m_tCurrentObjectDesc.iLayer == 1))
		//	{
		//		LayerType = 1;
		//		m_tCurrentObjectDesc.iLayer = 1;
		//	}



		//	ImGui::NewLine();

		//	ImGui::Text(u8"오브젝트유형");
		//	static int objectType = m_tCurrentObjectDesc.iObjType;
		//	if (ImGui::RadioButton(u8"그냥건물", m_tCurrentObjectDesc.iObjType == 0))
		//	{
		//		objectType = 0;
		//		m_tCurrentObjectDesc.iObjType = 0;
		//	}

		//	if (ImGui::RadioButton(u8"상호작용가능", m_tCurrentObjectDesc.iObjType == 1))
		//	{
		//		objectType = 1;
		//		m_tCurrentObjectDesc.iObjType = 1;
		//	}

		//	if (ImGui::RadioButton(u8"아이템", m_tCurrentObjectDesc.iObjType == 2))
		//	{
		//		objectType = 2;
		//		m_tCurrentObjectDesc.iObjType = 2;
		//	}

		//	if (ImGui::RadioButton(u8"몬스터", m_tCurrentObjectDesc.iObjType == 3))
		//	{
		//		objectType = 3;
		//		m_tCurrentObjectDesc.iObjType = 3;
		//	}

		//	ImGui::NewLine();

		//	ImGui::Text(u8"쉐이더");
		//	static int shaderType = m_tCurrentObjectDesc.iShaderPass;
		//	if (ImGui::RadioButton(u8"shader1", m_tCurrentObjectDesc.iShaderPass == 0))
		//	{
		//		shaderType = 0;
		//		m_tCurrentObjectDesc.iShaderPass = 0;
		//	}

		//	if (ImGui::RadioButton(u8"shader2", m_tCurrentObjectDesc.iShaderPass == 1))
		//	{
		//		shaderType = 1;
		//		m_tCurrentObjectDesc.iShaderPass = 1;
		//	}

		//	if (ImGui::RadioButton(u8"shader3", m_tCurrentObjectDesc.iShaderPass == 2))
		//	{
		//		shaderType = 2;
		//		m_tCurrentObjectDesc.iShaderPass = 2;
		//	}

		//	if (ImGui::RadioButton(u8"shader4", m_tCurrentObjectDesc.iShaderPass == 3))
		//	{
		//		shaderType = 3;
		//		m_tCurrentObjectDesc.iShaderPass = 3;
		//	}



		//	if (ImGui::Button(u8"맵 오브젝트 수정"))
		//	{
		//		Edit_CurrentGameObject_Desc(&m_tCurrentObjectDesc, object_current_idx);
		//	}

		//	ImGui::SameLine();
		//	
		//	if (ImGui::Button(u8"데칼 텍스처 추가"))
		//	{

		//		Get_Decal_Texture(object_current_idx);
		//	}
		//}
#pragma endregion

		
	}

	ImGui::End();
}

void CObjPlace_Manager::Save_GameObject(int iLevel)
{
	/* 맵 저장 정보 저장 */
	MAP_TOTALINFORM_DESC pMapTotalInform;

	pMapTotalInform.iLevelIndex = iLevel;
	pMapTotalInform.vPlaneSize = CTerrain_Manager::GetInstance()->Get_LandScale();

	pMapTotalInform.iNumMapObj = m_GameObjects.size();
	pMapTotalInform.pMapObjDesc = new OBJECTPLACE_DESC[m_GameObjects.size()];

	int iIndex = 0;

	for (auto& iter : m_GameObjects)
	{
		int iLayer = dynamic_cast<CConstruction*>(iter.second)->Get_ObjPlaceDesc(&pMapTotalInform.pMapObjDesc[iIndex]);

		/* iLayer -> construction에서 layer에 대한 정보를 읽어와서 이에 대해 저장 */
		XMStoreFloat4x4(&pMapTotalInform.pMapObjDesc[iIndex].vTransform, iter.second->Get_TransformCom()->Get_WorldMatrix());
		//string strlayer = m_pGameInstance->WstringToString(m_Layers[iLayer]);
		strcpy(pMapTotalInform.pMapObjDesc[iIndex].strLayer, m_Layers[iLayer]);

		iIndex++;
	}


	Export_Bin_Map_Data(&pMapTotalInform);

	Safe_Delete_Array(pMapTotalInform.pMapObjDesc);
}

void CObjPlace_Manager::Load_GameObject(int iNum)
{

	/* 맵 저장 정보 로드 */
	for (auto& Pair : m_GameObjects)
		Safe_Release(Pair.second);
	m_GameObjects.clear();


	MAP_TOTALINFORM_DESC		mapTotalInform;
	Import_Bin_Map_Data_OnTool(&mapTotalInform, m_FileNames[iNum]);

	CTerrain_Manager::GetInstance()->Change_LandScale(mapTotalInform.vPlaneSize.x, mapTotalInform.vPlaneSize.y);

	for (int i = 0; i < mapTotalInform.iNumMapObj; i++)
	{

		CConstruction::MAPOBJ_DESC		mapDesc;
		mapDesc.vStartPos = XMLoadFloat4x4(&mapTotalInform.pMapObjDesc[i].vTransform);
		int		iLayer = Find_Layers_Index(mapTotalInform.pMapObjDesc[i].strLayer);

		/* Layer 정보 안들어옴 */
		if (iLayer < 0)
			return;
		mapDesc.iLayer = iLayer;
		mapDesc.wstrModelName = m_pGameInstance->StringToWstring(mapTotalInform.pMapObjDesc[i].strModelCom);
		mapDesc.iShaderPass = mapTotalInform.pMapObjDesc[i].iShaderPassNum;
		mapDesc.iObjType = mapTotalInform.pMapObjDesc[i].iObjType;

		m_GameObjects.emplace(mapDesc.wstrModelName, CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_Construction"), &mapDesc));
	}

	for (int i = 0; i < mapTotalInform.iNumMapObj; i++)
	{
		Safe_Delete(mapTotalInform.pMapObjDesc);
	}

	Update_ObjectNameList();

	Safe_Delete_Array(mapTotalInform.pMapObjDesc);

	/* 로드할때는 현재 index 초기화 */
	m_iCurrentObjectIndex = 0;
}

void CObjPlace_Manager::Edit_Installed_GameObject(int iNumObject)
{
	Edit_GameObject_Transform(iNumObject);

	ImGui::Text(u8"Layer");
	static int LayerType = m_tCurrentObjectDesc.iLayer;
	if (ImGui::RadioButton(m_Layers[0], m_tCurrentObjectDesc.iLayer == 0))
	{
		LayerType = 0;
		m_tCurrentObjectDesc.iLayer = 0;
	}

	if (ImGui::RadioButton(m_Layers[1], m_tCurrentObjectDesc.iLayer == 1))
	{
		LayerType = 1;
		m_tCurrentObjectDesc.iLayer = 1;
	}



	ImGui::NewLine();

	ImGui::Text(u8"오브젝트유형");
	static int objectType = m_tCurrentObjectDesc.iObjType;
	if (ImGui::RadioButton(u8"그냥건물", m_tCurrentObjectDesc.iObjType == 0))
	{
		objectType = 0;
		m_tCurrentObjectDesc.iObjType = 0;
	}

	if (ImGui::RadioButton(u8"상호작용가능", m_tCurrentObjectDesc.iObjType == 1))
	{
		objectType = 1;
		m_tCurrentObjectDesc.iObjType = 1;
	}

	if (ImGui::RadioButton(u8"아이템", m_tCurrentObjectDesc.iObjType == 2))
	{
		objectType = 2;
		m_tCurrentObjectDesc.iObjType = 2;
	}

	if (ImGui::RadioButton(u8"몬스터", m_tCurrentObjectDesc.iObjType == 3))
	{
		objectType = 3;
		m_tCurrentObjectDesc.iObjType = 3;
	}

	ImGui::NewLine();

	ImGui::Text(u8"쉐이더");
	static int shaderType = m_tCurrentObjectDesc.iShaderPass;
	if (ImGui::RadioButton(u8"shader1", m_tCurrentObjectDesc.iShaderPass == 0))
	{
		shaderType = 0;
		m_tCurrentObjectDesc.iShaderPass = 0;
	}

	if (ImGui::RadioButton(u8"shader2", m_tCurrentObjectDesc.iShaderPass == 1))
	{
		shaderType = 1;
		m_tCurrentObjectDesc.iShaderPass = 1;
	}

	if (ImGui::RadioButton(u8"shader3", m_tCurrentObjectDesc.iShaderPass == 2))
	{
		shaderType = 2;
		m_tCurrentObjectDesc.iShaderPass = 2;
	}

	if (ImGui::RadioButton(u8"shader4", m_tCurrentObjectDesc.iShaderPass == 3))
	{
		shaderType = 3;
		m_tCurrentObjectDesc.iShaderPass = 3;
	}

}


void CObjPlace_Manager::Edit_GameObject_Transform(int iNumObject)
{
	/* 회전, 위치 등 transform 관련 수정*/

	multimap<wstring,CGameObject*>::iterator iter = m_GameObjects.begin();

	if (0 != iNumObject)
	{
		for (int i = 0; i < iNumObject; i++)
		{
			iter++;
		}
	}

	EditTransform((float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW),
		(float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ),
		(float*)iter->second->Get_TransformCom()->Get_WorldFloat4x4(),
		true);

}

void CObjPlace_Manager::Get_CurrentGameObject_Desc(CConstruction::MAPOBJ_DESC* mapDesc, int iNumObject)
{
	multimap<wstring, CGameObject*>::iterator iter = m_GameObjects.begin();

	if (0 != iNumObject)
	{
		for (int i = 0; i < iNumObject; i++)
		{
			iter++;
		}
	}

	*mapDesc = dynamic_cast<CConstruction*>(iter->second)->Send_GameObject_Information();
}

void CObjPlace_Manager::Edit_CurrentGameObject_Desc(CConstruction::MAPOBJ_DESC* mapDesc, int iNumObject)
{
	multimap<wstring, CGameObject*>::iterator iter = m_GameObjects.begin();

	if (0 != iNumObject)
	{
		for (int i = 0; i < iNumObject; i++)
		{
			iter++;
		}
	}

	dynamic_cast<CConstruction*>(iter->second)->Edit_GameObject_Information(*mapDesc);
}

void CObjPlace_Manager::Update_CurrentGameObject_Desc()
{
}

bool CObjPlace_Manager::Add_CloneObject_Imgui(MAPTOOL_OBJPLACE_DESC objDesc, _uint iFolderNum, _uint iObjectIndex)
{
	if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB))
	{
		string strObjName = Find_ModelName(objDesc.iLayer, iObjectIndex);
		wstring wstr = m_pGameInstance->StringToWstring(strObjName);


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
		mapDesc.iLayer = objDesc.iLayer;
		mapDesc.wstrModelName = wstr;
		mapDesc.iShaderPass = objDesc.iShaderPass;
		mapDesc.iObjType = objDesc.iObjType;
		mapDesc.iObjPropertyType = objDesc.iObjPropertyType;

		m_GameObjects.emplace(wstr, CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_Construction"), &mapDesc));

		Update_ObjectNameList();

		return true;
	}

	return false;
}

void CObjPlace_Manager::Set_Map_Object()
{
	ImGui::Text(u8"LayerTag 이름");

	const char* pLayerArray[] = { "Map0", "Map1" };
	static int folder_current_idx = 0;
	if (ImGui::BeginListBox("listbox 1"))
	{
		for (int n = 0; n < IM_ARRAYSIZE(pLayerArray); n++)
		{
			const bool is_selected = (folder_current_idx == n);
			if (ImGui::Selectable(pLayerArray[n], is_selected))
				folder_current_idx = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	static int object_current_idx = 0;

	if (0 == folder_current_idx)
	{
		if (ImGui::BeginListBox(u8"모델0-엘베있는1층"))
		{
			for (int n = 0; n < m_ObjectNames_Map0.size(); n++)
			{
				const bool is_selected = (object_current_idx == n);
				if (ImGui::Selectable(m_ObjectNames_Map0[n], is_selected))
					object_current_idx = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}
	if (1 == folder_current_idx)
	{

		if (ImGui::BeginListBox(u8"모델1-엘베있는2층"))
		{
			for (int n = 0; n < m_ObjectNames_Map1.size(); n++)
			{
				const bool is_selected = (object_current_idx == n);
				if (ImGui::Selectable(m_ObjectNames_Map1[n], is_selected))
					object_current_idx = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}

	ImGui::NewLine();

	ImGui::Text(u8"Layer");
	static int LayerType = 0;
	ImGui::RadioButton(m_Layers[0], &LayerType, 0);
	ImGui::RadioButton(m_Layers[1], &LayerType, 1);


	/* 데이터 추가할때마다 수정 */
	ImGui::NewLine();

	ImGui::Text(u8"오브젝트유형");
	static int objectType = 0;
	ImGui::RadioButton(u8"그냥건물", &objectType, OBJECT_TYPE::CONSTRUCTION);
	ImGui::RadioButton(u8"상호작용가능", &objectType, OBJECT_TYPE::ITEM);
	ImGui::RadioButton(u8"아이템", &objectType, OBJECT_TYPE::MONSTER);
	ImGui::RadioButton(u8"몬스터", &objectType, OBJECT_TYPE::OBJ_END);

	ImGui::NewLine();

	ImGui::Text(u8"쉐이더");
	static int shaderType = 0;
	ImGui::RadioButton("radio a", &shaderType, 0); 
	ImGui::NewLine();
	ImGui::RadioButton("d b", &shaderType, 1); 
	ImGui::NewLine();
	ImGui::RadioButton("raddvio c", &shaderType, 2);
	ImGui::NewLine();
	ImGui::RadioButton("raddio d", &shaderType, 3);

	ImGui::NewLine();

	ImGui::Text(u8"오브젝트속성유형");
	static int objectPropertyType = 0;
	ImGui::RadioButton(u8"회복", &objectPropertyType, 0);
	ImGui::RadioButton(u8"부수기", &objectPropertyType, 1);



	ImGui::NewLine();

	ImGui::Text(u8"T 누르면 모델보임");
	if (m_pGameInstance->GetKeyState(DIK_T) == HOLD)
	{
		m_bShowExample = true;
	}
	else if (m_pGameInstance->GetKeyState(DIK_T) == NONE)
	{
		m_bShowExample = false;
	}

	/* 데이터 추가할때마다 수정 */
	MAPTOOL_OBJPLACE_DESC			objDesc;
	objDesc.iLayer = LayerType;
	objDesc.iObjType = objectType;
	objDesc.iShaderPass = shaderType;
	objDesc.iObjPropertyType = objectPropertyType;

	Show_ExampleModel(objDesc, folder_current_idx, object_current_idx);


	if (ImGui::Button(u8"Object 추가"))
	{
		m_bDoing_Place_Object = true;
	}

	if (true == m_bDoing_Place_Object)
	{
		bool    bcheckFinished = false;

		bcheckFinished = Add_CloneObject_Imgui(objDesc, folder_current_idx, object_current_idx);

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

HRESULT CObjPlace_Manager::Import_Bin_Map_Data_OnTool(MAP_TOTALINFORM_DESC* mapObjData, char* fileName)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/MapData/");

	strcat_s(fullPath, fileName);

	/*char cLevel[2] = "";
	_itoa_s(mapObjData->iLevelIndex, cLevel, 10);
	strcat_s(fullPath, cLevel);
	strcat_s(fullPath, ".bin");*/


	ifstream in(fullPath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("파일 개방 실패");
		return E_FAIL;
	}


	in.read((char*)&mapObjData->iLevelIndex, sizeof(int));

	in.read((char*)&mapObjData->vPlaneSize, sizeof(XMFLOAT2));

	in.read((char*)&mapObjData->iNumMapObj, sizeof(int));

	mapObjData->pMapObjDesc = new OBJECTPLACE_DESC[mapObjData->iNumMapObj];

	for (int i = 0; i < mapObjData->iNumMapObj; i++)
	{
		in.read((char*)&mapObjData->pMapObjDesc[i], sizeof(OBJECTPLACE_DESC));
	}

	in.close();

	return S_OK;
}

HRESULT CObjPlace_Manager::Export_Bin_Map_Data(MAP_TOTALINFORM_DESC* mapObjData)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/MapData/");

	strcat_s(fullPath, "MapObject_Data");

	char cLevel[2] = "";
	_itoa_s(mapObjData->iLevelIndex, cLevel, 10);
	strcat_s(fullPath, "_");
	strcat_s(fullPath, cLevel);
	strcat_s(fullPath, ".dat");

	fs::path path(fullPath);

	//if (!exists(path))
	//	fs::create_(path);

	ofstream out(fullPath, ios::binary);
	//file.open(fullPath, ios::out | ios::binary);

	if (!out) {
		return E_FAIL;
	}


	int		iLevel = mapObjData->iLevelIndex;
	out.write((char*)&iLevel, sizeof(int));

	XMFLOAT2 planeSize = mapObjData->vPlaneSize;
	out.write((char*)&planeSize, sizeof(XMFLOAT2));


	int iNumMapObj = mapObjData->iNumMapObj;
	out.write((char*)&iNumMapObj, sizeof(int));

	for (int i = 0; i < mapObjData->iNumMapObj; i++)
	{
		OBJECTPLACE_DESC PObjPlaceDesc = mapObjData->pMapObjDesc[i];
		out.write((char*)&PObjPlaceDesc, sizeof(OBJECTPLACE_DESC));
	}

	out.close();

	return S_OK;
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

void CObjPlace_Manager::Show_ExampleModel(MAPTOOL_OBJPLACE_DESC objDesc, _uint iFolderNum, _uint iObjectIndex)
{
	if (true == m_bShowExample)
	{
		//
		//m_pShownObject = m_pGameInstance->Clone_Object();

		if (nullptr == m_pShownObject)
		{
			string strObjName = Find_ModelName(iFolderNum, iObjectIndex);
			wstring wstr = m_pGameInstance->StringToWstring(strObjName);


			_bool		isPick;
			_vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);

			_matrix			startPos;
			startPos = XMMatrixIdentity();
			startPos.r[3].m128_f32[0] = vTargetPos.m128_f32[0];
			startPos.r[3].m128_f32[1] = vTargetPos.m128_f32[1];
			startPos.r[3].m128_f32[2] = vTargetPos.m128_f32[2];
			startPos.r[3].m128_f32[3] = vTargetPos.m128_f32[3];

			/* 데이터 추가할때마다 수정 */
			CConstruction::MAPOBJ_DESC		mapDesc;
			mapDesc.vStartPos = startPos;
			mapDesc.wstrModelName = wstr;
			mapDesc.iLayer = objDesc.iLayer;
			mapDesc.iShaderPass = objDesc.iShaderPass;
			mapDesc.iObjType = objDesc.iObjType;
			mapDesc.iObjPropertyType = objDesc.iObjPropertyType;


			m_pShownObject = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Construction"), &mapDesc);

			/* 한번만 추가되게 - 같은 위치에 */
			m_bInstallOneTime = true;
		}
		else
		{
			if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB) && true == m_bInstallOneTime)
			{
				/* 막아놓기 */
				m_bInstallOneTime = false;

				string strObjName = Find_ModelName(objDesc.iLayer, iObjectIndex);
				wstring wstr = m_pGameInstance->StringToWstring(strObjName);

				CConstruction::MAPOBJ_DESC		mapDesc;
				mapDesc = dynamic_cast<CConstruction*>(m_pShownObject)->Get_MapObjDesc_For_AddList();

				/* 데이터 추가할때마다 수정 */
				mapDesc.iLayer = objDesc.iLayer;
				mapDesc.iObjType = objDesc.iObjType;
				mapDesc.iShaderPass = objDesc.iShaderPass;
				mapDesc.iObjPropertyType = objDesc.iObjPropertyType;


				m_GameObjects.emplace(wstr, CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_Construction"), &mapDesc));

				Update_ObjectNameList();
			}
		}
	}
	else
	{
		/* 설치 전 무슨 모델인지 마우스에다가 보여주는 형식 */
		if (nullptr != m_pShownObject)
		{
			Safe_Release(m_pShownObject);
		}
	}

}

string CObjPlace_Manager::Find_ModelName(_uint iFolderNum, _uint iObjectIndex)
{
	string strResult;

	if (0 == iFolderNum)
	{
		strResult = m_ObjectNames_Map0[iObjectIndex];
	}
	else if (1 == iFolderNum)
	{
		strResult = m_ObjectNames_Map1[iObjectIndex];
	}
	return strResult;
}

void CObjPlace_Manager::Delete_Object(int iNumObject)
{

	vector<char*>::iterator objectnameiter = m_ObjectNames.begin();
	multimap<wstring, CGameObject*>::iterator iter = m_GameObjects.begin();

	if (0 != iNumObject)
	{
		for (int i = 0; i < iNumObject; i++)
		{
			iter++;
			objectnameiter++;
		}
	}

	Safe_Release(iter->second);
	m_GameObjects.erase(iter);

	m_ObjectNames.erase(objectnameiter);
	Update_ObjectNameList();

	/* index 조정 */
	if (m_iCurrentObjectIndex >= m_GameObjects.size())
	{
		m_iCurrentObjectIndex = m_GameObjects.size() - 1;
	}

}

void CObjPlace_Manager::Update_ObjectNameList()
{
	for (auto& iter : m_ObjectNames)
		Safe_Delete(iter);

	m_ObjectNames.clear();

	int iIndex = 0;

	if (!m_GameObjects.empty())
	{
		for (auto& iter : m_GameObjects)
		{
			/* 0 이상의 숫자로 화면에 기록해야하니까 */
			iter.second->Set_ObjID(iIndex+1);

			const wchar_t* layer = iter.first.c_str();
			string strlayer = TCHARToString(layer);
			char* clayertag = new char[MAX_PATH];
			strcpy(clayertag, StringToChar(strlayer));

			char buff[MAX_PATH];
			sprintf(buff, "%d", iIndex);
			strcat(clayertag, buff);
			m_ObjectNames.push_back(clayertag);
			iIndex++;
		}
	}


}

void CObjPlace_Manager::Show_FileName()
{
	ImGui::Text(u8" 배치 오브젝트 리스트 ");

	if (m_FileNames.empty())
	{
		Update_FileName();
	}

	if (!m_FileNames.empty())
	{
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
	
}

void CObjPlace_Manager::Update_FileName()
{
	for (auto& iter : m_FileNames)
		Safe_Delete(iter);

	m_FileNames.clear();
	string path = "../../Client/Bin/DataFiles/MapData/*.dat";

	struct _finddata_t fd;
	intptr_t handle;

	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
		return; // 파일없을때
	do
	{
		char* cfilename = new char[MAX_PATH];
		strcpy(cfilename, StringToChar(fd.name));
		m_FileNames.push_back(cfilename);
	} while (_findnext(handle, &fd) == 0);
	_findclose(handle);
}

int CObjPlace_Manager::Click_To_Select_Object()
{
	_bool		isPick;

	_float		iIndex = 0;
	iIndex = m_pGameInstance->FindObjID(&isPick);

	if (iIndex > 0)
	{
		/* ID는 1부터 시작하니까 INDEX 받을때는 -1 해주기 */
		return (iIndex - 1);
	}

	return -1;
}

int CObjPlace_Manager::Find_Layers_Index(char* strLayer)
{
	for (int i = 0; i < m_Layers.size(); i++)
	{
		if (0 == strcmp(strLayer, m_Layers[i]))
		{
			return i;
		}
	}

	return -1;
}

void CObjPlace_Manager::Get_Decal_Texture(int iNumObject)
{
	if (0 < m_Decals.size())
	{
		m_Decals.clear();
	}

	multimap<wstring, CGameObject*>::iterator iter = m_GameObjects.begin();

	if (0 != iNumObject)
	{
		for (int i = 0; i < iNumObject; i++)
		{
			iter++;
		}
	}
	//int iNum = dynamic_cast<CConstruction*>(iter->second)->Get_Model()->Get_DecalMaterial().size();
	//memcpy(&m_Decals, &(dynamic_cast<CConstruction*>(iter->second)->Get_Model()->Get_DecalMaterial()), sizeof(DECAL_DESC) * iNum);
	dynamic_cast<CConstruction*>(iter->second)->Get_Model()->Copy_DecalMaterial(&m_Decals);
}

void CObjPlace_Manager::Update_DecalNameList()
{
	if (0 <= m_DecalNames.size())
	{
		for (auto& iter : m_DecalNames)
			Safe_Delete(iter);

		m_DecalNames.clear();
	}

	if (0 < m_Decals.size())
	{
		for (int i = 0; i < m_Decals.size(); i++)
		{
			char* szName = new char[MAX_PATH];
			strcpy(szName, "Decal");
			char buff[MAX_PATH];
			sprintf(buff, "%d", i);
			strcat(szName, buff);
			m_DecalNames.push_back(szName);
		}
	}
}

void CObjPlace_Manager::Edit_Decal()
{
}

void CObjPlace_Manager::Add_Decal_IMGUI()
{
	if (m_bAddDecal_IMGUI == true)
	{
		if (m_iDecalObjNum != m_iCurrentObjectIndex)
		{
			m_iDecalObjNum = m_iCurrentObjectIndex;
			Get_Decal_Texture(m_iDecalObjNum);
			Update_DecalNameList();
		}

		ImGui::Begin(u8" 데칼 ");

		static int decal_current_idx = 0;
		if (ImGui::BeginListBox("listbox 0"))
		{
			for (int n = 0; n < m_DecalNames.size(); n++)
			{
				const bool is_selected = (decal_current_idx == n);
				if (ImGui::Selectable(m_DecalNames[n], is_selected))
				{
					decal_current_idx = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		static int DecalFind = 0;
		if (ImGui::RadioButton(u8"Decal 찾기O", DecalFind == 0))
		{
			DecalFind = 0;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton(u8"Decal 찾기X", DecalFind == 1))
		{
			DecalFind = 1;
		}

		if(DecalFind == 0)
		{
			multimap<wstring, CGameObject*>::iterator iter = m_GameObjects.begin();

			if (0 != m_iCurrentObjectIndex)
			{
				for (int i = 0; i < m_iCurrentObjectIndex; i++)
				{
					iter++;
					m_iCurrentObjectIndex++;
				}
			}

			dynamic_cast<CConstruction*>(iter->second)->On_Find_DecalMesh(m_Decals[decal_current_idx].pMeshIndices, m_Decals[decal_current_idx].iMeshNum);
		}
		else if (DecalFind == 1)
		{
			multimap<wstring, CGameObject*>::iterator iter = m_GameObjects.begin();

			if (0 != m_iCurrentObjectIndex)
			{
				for (int i = 0; i < m_iCurrentObjectIndex; i++)
				{
					iter++;
					m_iCurrentObjectIndex++;
				}
			}

			dynamic_cast<CConstruction*>(iter->second)->Off_Find_DecalMesh();
		}


		ImGui::End();
	}
	
}


void CObjPlace_Manager::Free()
{
	if (nullptr != m_pShownObject)
	{
		Safe_Release(m_pShownObject);
	}

	for (auto& Pair : m_GameObjects)
		Safe_Release(Pair.second);
	m_GameObjects.clear();

	for (auto& iter : m_ObjectNames_Map0)
		Safe_Delete(iter);
	m_ObjectNames_Map0.clear();

	for (auto& iter : m_ObjectNames_Map1)
		Safe_Delete(iter);
	m_ObjectNames_Map1.clear();

	for (auto& iter : m_ObjectNames)
		Safe_Delete(iter);
	m_ObjectNames.clear();

	for (auto& iter : m_FileNames)
		Safe_Delete(iter);
	m_FileNames.clear();

	for (auto& iter : m_DecalNames)
		Safe_Delete(iter);
	m_DecalNames.clear();

	m_Layers.clear();

	m_Decals.clear();



	Safe_Release(m_pGameInstance);
}
