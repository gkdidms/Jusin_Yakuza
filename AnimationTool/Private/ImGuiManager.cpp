#include "ImguiManager.h"
#include "GameInstance.h"
#include "AnimModel.h"
#include "Animation.h"
#include "Bone.h"
#include "Channel.h"

#pragma region "Imgui"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"
#pragma endregion

CImguiManager::CImguiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImguiManager::Initialize(void* pArg)
{

	return S_OK;
}

void CImguiManager::Tick(const _float& fTimeDelta)
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

	if (ImGui::Button("Model Connecting"))
		Connect_Model_Ref();

	ModelList();

	//ImDrawList* draw_list = ImDrawList::Add();

	if (ImGui::Button("Open"))
		m_isAnimListWindow = !m_isAnimListWindow;

	if (m_isAnimListWindow)
	{
		AnimListWindow();
		BoneListWindow();
		KeyFrameWindow();
	}
}



HRESULT CImguiManager::Render()
{
	ImGui::Render();

	return S_OK;
}

void CImguiManager::ModelList()
{
	if (ImGui::DragFloat("Rotation X", &m_ModelRotation[0], 5.f))
	{
		Update_Model_RotationX();

		m_ModelRotation[0] = 0.f;
	}

	if (ImGui::DragFloat("Rotation Y", &m_ModelRotation[1], 5.f))
	{
		Update_Model_RotationY();

		m_ModelRotation[1] = 0.f;
	}

	if (ImGui::DragFloat("Rotation Z", &m_ModelRotation[2], 5.f))
	{
		Update_Model_RotationZ();

		m_ModelRotation[2] = 0.f;
	}

	if (ImGui::DragFloat("All Scale", &m_ModelScale, 0.05f))
	{
		if (0.01f > m_ModelScale) m_ModelScale = 0.01f;

		Update_Model_Scaled();
	}

	ImGui::NewLine();
	ImGui::Text("Model List");

	string strDirPath = "../Bin/Resources/Model/Anim/";

	m_pGameInstance->Get_DirectoryName(strDirPath, m_ModelNameList);

	vector<const char*> items;

	if (0 < m_ModelNameList.size()) {
		for (const string& str : m_ModelNameList) {
			items.push_back(str.c_str());
		}
	}

	if (ImGui::ListBox("##", &m_iModelSelectedIndex, items.data(), m_ModelNameList.size()))
	{
		m_iAnimIndex = 0;
		m_pRenderModel->Change_Model(m_pGameInstance->StringToWstring(m_ModelNameList[m_iModelSelectedIndex]));
	}

}

void CImguiManager::AnimListWindow()
{
	ImGui::Begin("Anim List", &m_isAnimListWindow);

	m_AnimNameList.clear();
	const vector<CAnimation*> pAnims = m_pRenderModel->Get_Animations();
	m_AnimNameList.resize(pAnims.size());

	_uint i = 0;

	vector<const char*> items;

	for (auto pAnim : pAnims)
	{
		string strName = m_pGameInstance->Extract_String(pAnim->Get_AnimName(), '[', ']');

		m_AnimNameList[i] = strName;
		items.push_back(m_AnimNameList[i].c_str());

		i++;
	}

	if (ImGui::ListBox("##", &m_iAnimIndex, items.data(), items.size()))
	{
		m_pRenderModel->Change_Animation(m_iAnimIndex);
	}

	ImGui::End();

}

void CImguiManager::BoneListWindow()
{
	ImGui::Begin("Bone List", &m_isAnimListWindow);

	m_BoneNameList.clear();
	const vector<CBone*> pBones = m_pRenderModel->Get_Bones();
	m_BoneNameList.resize(pBones.size());

	_uint i = 0;

	vector<const char*> items;

	for (auto pBone: pBones)
	{
		string strName = pBone->Get_Name();
		//string strName = m_pGameInstance->Extract_String(pBone->Get_Name(), '[', ']');

		m_BoneNameList[i] = strName;
		items.push_back(m_BoneNameList[i].c_str());

		i++;
	}

	if (ImGui::ListBox("##", &m_iBoneSelectedIndex, items.data(), items.size()))
	{
		m_pRenderModel->Select_Bone(m_iBoneSelectedIndex);
	}

	ImGui::End();
}


void CImguiManager::MeshListWindow()
{
	ImGui::Begin("Mesh List", &m_isAnimListWindow);

	m_MeshNameList.clear();
	const vector<CMesh*> pMeshes = m_pRenderModel->Get_Meshes();
	m_MeshNameList.resize(pMeshes.size());

	_uint i = 0;

	vector<const char*> items;

	for (auto pMesh : pMeshes)
	{
		string strName = pMesh->Get_Name();
		//string strName = m_pGameInstance->Extract_String(pMesh->Get_Name(), '[', ']');

		m_MeshNameList[i] = strName;
		items.push_back(m_MeshNameList[i].c_str());

		i++;
	}

	ImGui::ListBox("Model Mesh List", &m_iMeshSelectedIndex, items.data(), items.size());

	if (ImGui::Button("Add"))
	{
		m_AddedMeshNameList.push_back(m_MeshNameList[m_iMeshSelectedIndex]);
	}

	vector<const char*> Addeditems;

	for (auto& AddedMesh : m_AddedMeshNameList)
	{
		Addeditems.push_back(AddedMesh.c_str());
	}

	ImGui::ListBox("Added Meshes", &m_iAddedMeshSelectedIndex, Addeditems.data(), Addeditems.size());

	ImGui::End();
}

void CImguiManager::KeyFrameWindow()
{
	ImGui::Begin("KeyFrame", NULL);

	auto Anims = m_pRenderModel->Get_Animations();

	_float Duration = (_float)(*(Anims[m_iAnimIndex]->Get_Duration()));
	_float CurrentPosition = (_float)(*(Anims[m_iAnimIndex]->Get_CurrentPosition()));
	
	ImGui::SliderFloat("##", &CurrentPosition, 0.f, Duration);

	Anims[m_iAnimIndex]->Set_CurrentPosition((_double)CurrentPosition);
	Anims[m_iAnimIndex]->Update_KeyframeIndex();

	if (ImGui::Button("Stop/Play"))
	{
		m_isPause = !m_isPause;

		if (m_isPause)
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.f);
		else
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fTimeDeltaScale);
	}

	ImGui::SameLine();

	if (ImGui::Button("1x"))
	{
		m_fTimeDeltaScale = 1.f;
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fTimeDeltaScale);

	}

	ImGui::SameLine();

	if (ImGui::Button("2x"))
	{
		m_fTimeDeltaScale = 2.f;
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fTimeDeltaScale);
	}

	ImGui::SameLine();

	if (ImGui::Button("3x"))
	{
		m_fTimeDeltaScale = 3.f;
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fTimeDeltaScale);
	}


	DrawChannels();

	ImGui::End();
}

void CImguiManager::FXWindow(ImGuiIO& io)
{
	ImGui::Begin("FX", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImVec2 size(320.0f, 180.0f);
	ImGui::InvisibleButton("canvas", size);
	ImVec2 p0 = ImGui::GetItemRectMin();
	ImVec2 p1 = ImGui::GetItemRectMax();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->PushClipRect(p0, p1);

	//m_pGameInstance->Mouse

	ImVec4 mouse_data;
	mouse_data.x = (io.MousePos.x - p0.x) / size.x;
	mouse_data.y = (io.MousePos.y - p0.y) / size.y;
	mouse_data.z = io.MouseDownDuration[0];
	mouse_data.w = io.MouseDownDuration[1];

	vector<float> data = { 10.0f, 20.0f, 30.0f, 40.0f, 50.0f };
	FX(draw_list, p0, p1, size, mouse_data, (float)ImGui::GetTime());
	DrawTimeline(draw_list, data);
	draw_list->PopClipRect();
	ImGui::End();
}

void CImguiManager::FX(ImDrawList* d, V2 a, V2 b, V2 sz, ImVec4 m, float t)
{
	for (int n = 0; n < (1.0f + sinf(t * 5.7f)) * 40.0f; n++)
		d->AddCircle(V2(a.x + sz.x * 0.5f, a.y + sz.y * 0.5f), sz.y * (0.01f + n * 0.03f),
			IM_COL32(255, 140 - n * 4, n * 3, 255));
}

void CImguiManager::DrawTimeline(ImDrawList* draw_list, const vector<float>& data)
{
	//ImVec2 p0 = ImGui::GetItemRectMin();
	//ImVec2 p1 = ImGui::GetItemRectMax();

	//draw_list->AddRect(ImVec2(p0.x, p0.y), ImVec2(p1.x, p1.y), IM_COL32_WHITE);

	//// 타임라인 배경 그리기
	//draw_list->AddRect(ImVec2(p0.x, p0.y), ImVec2(p1.x * 0.5f, p1.y * 0.5f), IM_COL32(100, 100, 100, 255));

	//// X축 그리기
	//draw_list->AddLine(ImVec2(0.0f, 15.0f), ImVec2(ImGui::GetContentRegionMax().x, 15.0f), ImColor(0xAAAAAA));

	//// Y축 그리기
	//draw_list->AddLine(ImVec2(15.0f, 0.0f), ImVec2(15.0f, ImGui::GetContentRegionMax().y), ImColor(0xAAAAAA));

	  // 타임라인 배경 그리기
	draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32_WHITE);

	// X축 그리기
	draw_list->AddLine(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + 15.f), ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMin().y + 15.f), IM_COL32(255, 0, 0, 255));

	// Y축 그리기
	draw_list->AddLine(ImVec2(ImGui::GetItemRectMin().x + 15, ImGui::GetItemRectMin().y), ImVec2(ImGui::GetItemRectMin().x + 15, ImGui::GetItemRectMax().y), IM_COL32(255, 0, 0, 255));

	// 데이터 포인트 루프
	for (int i = 0; i < data.size(); i++) {
		float x = (data[i] - 1) / (10 - 1) * (ImGui::GetItemRectMax().x - 15.0f) + 15.0f;
		float y = data[i] * (ImGui::GetItemRectMax().y - 15.0f) + 15.0f;

		// 데이터 포인트 표시
		draw_list->AddCircle(ImVec2(x, y), 5.0f, IM_COL32_WHITE);
	}
}

void CImguiManager::DrawChannels()
{
	auto Anims = m_pRenderModel->Get_Animations();
	auto& Channels = Anims[m_iAnimIndex]->Get_Channels();

	m_ChannelNameList.clear();
	m_ChannelNameList.resize(Channels.size());

	_uint i = 0;

	vector<const char*> items;

	for (auto pChannel : Channels)
	{
		string strName = pChannel->Get_Name();
		//string strName = m_pGameInstance->Extract_String(pBone->Get_Name(), '[', ']');

		m_ChannelNameList[i] = strName;
		items.push_back(m_ChannelNameList[i].c_str());

		i++;
	}

	if (ImGui::ListBox("##", &m_iChannelSelectedIndex, items.data(), items.size()))
	{
		m_pRenderModel->Select_Bone(Channels[m_iChannelSelectedIndex]->Get_BoneIndex());
	}
}

void CImguiManager::LoadAnimationCharacterList()
{
	wstring strModelPath = TEXT("../Bin/Resources/Model/Anim/");

	vector<wstring> vecDirectorys;
	m_pGameInstance->Get_DirectoryName(strModelPath, vecDirectorys);

	for (auto& strName : vecDirectorys)
	{
		string strBinPath = m_pGameInstance->WstringToString(strModelPath) + "Bin/";

		if (!fs::exists(strBinPath))
		{

		}
	}
}

void CImguiManager::Connect_Model_Ref()
{
	m_pRenderModel = static_cast<CAnimModel*>(m_pGameInstance->Get_GameObject(LEVEL_EDIT, TEXT("Layer_Object"), 0));
}

void CImguiManager::Update_Model_RotationX()
{
	m_pRenderModel->Set_Rotation(0, XMConvertToRadians(m_ModelRotation[0]), m_pGameInstance->Get_TimeDelta(TEXT("Timer_60")));
}

void CImguiManager::Update_Model_RotationY()
{
	m_pRenderModel->Set_Rotation(1, XMConvertToRadians(m_ModelRotation[1]), m_pGameInstance->Get_TimeDelta(TEXT("Timer_60")));
}

void CImguiManager::Update_Model_RotationZ()
{
	m_pRenderModel->Set_Rotation(2, XMConvertToRadians(m_ModelRotation[2]), m_pGameInstance->Get_TimeDelta(TEXT("Timer_60")));
}

void CImguiManager::Update_Model_Scaled()
{
	m_pRenderModel->Set_Scaled(m_ModelScale, m_ModelScale, m_ModelScale);
}

CImguiManager* CImguiManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImguiManager* pInstance = new CImguiManager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Engine::Safe_Release(pInstance);
		MSG_BOX("CImguiManager Create Failed");
		return nullptr;
	}

	//ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);

	return pInstance;
}

void CImguiManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
