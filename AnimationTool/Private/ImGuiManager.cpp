#include "ImguiManager.h"
#include "GameInstance.h"

#include "AnimModel.h"

#pragma region "Model"
#include "Animation.h"
#include "Bone.h"
#include "Channel.h"
#include "Mesh.h"
#pragma endregion

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
	if (LEVEL_EDIT == m_pGameInstance->Get_CurrentLevel())
		Connect_Model_Ref();

	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

	ModelList();

	ImGui::SameLine();
	if (ImGui::Button("Open"))
		m_isOnToolWindows = !m_isOnToolWindows;

	if (m_isOnToolWindows)
	{
		AnimListWindow();
		BoneListWindow();
		KeyFrameWindow();
		MeshListWindow();
		AnimEventWindow();
	}
}



HRESULT CImguiManager::Render()
{
	ImGui::Render();

	return S_OK;
}

void CImguiManager::ModelList()
{
	//ImGui::DragFloat("drag float", &f1, 0.005f);
	if (ImGui::DragFloat3("Position", m_ModelPosition, 0.1f))
	{
		Update_Model_Position();
	}

	if (ImGui::DragFloat("Rotation X", &m_ModelRotation[0], 10.f))
	{
		Update_Model_RotationX();

		m_ModelRotation[0] = 0.f;
	}

	if (ImGui::DragFloat("Rotation Y", &m_ModelRotation[1], 10.f))
	{
		Update_Model_RotationY();

		m_ModelRotation[1] = 0.f;
	}

	if (ImGui::DragFloat("Rotation Z", &m_ModelRotation[2], 10.f))
	{
		Update_Model_RotationZ();

		m_ModelRotation[2] = 0.f;
	}

	//if (ImGui::DragFloat("All Scale", &m_ModelScale, 0.001f))
	//{
	//	if (0.001f > m_ModelScale) m_ModelScale = 0.001f;

	//	Update_Model_Scaled();
	//}

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
	ImGui::Begin("Anim List", &m_isOnToolWindows);

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

	ImGui::Text("Animation List");
	if (ImGui::ListBox("##", &m_iAnimIndex, items.data(), items.size()))
	{
		m_fAnimationPosition = 0.f;
		m_pRenderModel->Change_Animation(m_iAnimIndex);
	}

	ImGui::SameLine();
	if (ImGui::Button("Add"))
	{
		m_AddedAnims.emplace(m_iAnimIndex, m_AnimNameList[m_iAnimIndex]);
	}


	vector<const char*> Addeditems;

	for (auto& AddedAnim : m_AddedAnims)
	{
		Addeditems.push_back(AddedAnim.second.c_str());
	}

	ImGui::ListBox("Loop Animations", &m_iAddedAnimSelectedIndex, Addeditems.data(), Addeditems.size());

	if (ImGui::Button("Save"))
	{
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete"))
	{
		auto iter = m_AddedAnims.begin();

		for (size_t i = 0; i < m_iAddedAnimSelectedIndex; i++)
		{
			iter++;
		}
		
		m_AddedAnims.erase(iter);
	}


	ImGui::End();

}

void CImguiManager::BoneListWindow()
{
	ImGui::Begin("Bone List", &m_isOnToolWindows);

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

	if (ImGui::RadioButton("AABB", m_iColliderType == AABB))
	{
		m_iColliderType = AABB;
		Reset_Collider_Value();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("OBB", m_iColliderType == OBB))
	{
		m_iColliderType = OBB;
		Reset_Collider_Value();
	}	
	ImGui::SameLine();
	if (ImGui::RadioButton("Sphere", m_iColliderType == SPHERE))
	{
		m_iColliderType = SPHERE;
		Reset_Collider_Value();
	}

	if (ImGui::DragFloat3("Position", m_ColliderPosition, 0.1f))
	{
		_float3 vPosition = _float3(m_ColliderPosition[0], m_ColliderPosition[1], m_ColliderPosition[2]);
		m_pRenderModel->Set_Collider_Center(m_iBoneSelectedIndex, vPosition);
	}


	switch (m_iColliderType)
	{
	case AABB:
	{
		if(ImGui::DragFloat3("Extents", m_ColliderExtents, 0.1f))
			m_pRenderModel->Set_Collider_Value(m_iBoneSelectedIndex, m_ColliderExtents);
		break;
	}
	case OBB:
	{
		if(ImGui::DragFloat3("Extents", m_ColliderExtents, 0.1f))
			m_pRenderModel->Set_Collider_Value(m_iBoneSelectedIndex, m_ColliderExtents);
		break;
	}
	case SPHERE:
	{
		if(ImGui::DragFloat("Radius", &m_fColliderRadius, 0.1f))
			m_pRenderModel->Set_Collider_Value(m_iBoneSelectedIndex, &m_fColliderRadius);
		break;
	}
	}

	if (ImGui::Button("Create Collier"))
	{
		m_pRenderModel->Create_BoneCollider(m_iColliderType, m_iBoneSelectedIndex);
	}

	ImGui::End();
}


void CImguiManager::MeshListWindow()
{
	ImGui::Begin("Mesh List", &m_isOnToolWindows);

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

	ImGui::Text("Mesh List");
	if (ImGui::ListBox("##", &m_iMeshSelectedIndex, items.data(), items.size()))
	{
		m_pRenderModel->Select_Mesh(m_iMeshSelectedIndex);
	}

	if (ImGui::Button("Add"))
	{
		m_AddedMeshes.emplace(m_iMeshSelectedIndex, m_MeshNameList[m_iMeshSelectedIndex]);
	}
	ImGui::SameLine();
	// 블랜드 기능 적용하는 버튼
	if (ImGui::Button("Apply"))
	{
		for (auto& AddedMesh : m_AddedMeshes)
		{
			pMeshes[AddedMesh.first]->Set_AlphaApply(true);
		}
	}

	//추가한 메시 리스트
	vector<const char*> Addeditems;
	for (auto& AddedMesh : m_AddedMeshes)
	{
		Addeditems.push_back(AddedMesh.second.c_str());
	}
	ImGui::ListBox("Added Meshes", &m_iAddedMeshSelectedIndex, Addeditems.data(), Addeditems.size());

	if (ImGui::Button("Save"))
	{
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		auto iter = m_AddedMeshes.begin();

		for (size_t i = 0; i < m_iAddedMeshSelectedIndex; i++)
		{
			iter++;
		}

		m_AddedMeshes.erase(iter);
	}

	ImGui::End();
}

void CImguiManager::KeyFrameWindow()
{
	ImGui::Begin("KeyFrame", NULL);

	auto Anims = m_pRenderModel->Get_Animations();

	_float Duration = (_float)(*(Anims[m_iAnimIndex]->Get_Duration()));
	_float CurrentPosition = (_float)(*(Anims[m_iAnimIndex]->Get_CurrentPosition()));
	
	// 애니메이션 재생 위치를 보여준다.
	ImGui::SliderFloat("##", &CurrentPosition, 0.f, Duration);

	Anims[m_iAnimIndex]->Set_CurrentPosition((_double)CurrentPosition);
	Anims[m_iAnimIndex]->Update_KeyframeIndex();

#pragma region 애니메이션 재생 관련 버튼
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
#pragma endregion

	//현재 애니메이션에 맞는 채널들을 보여준다
	DrawChannels();

	// 이벤트를 추가할 애니메이션 포지션
	ImGui::InputFloat("Animation Position", &m_fAnimationPosition);

#pragma region 콜라이더 이벤트 버튼들
	ImGui::Text("Collider Event");
	if (ImGui::Button("Collider Activation"))
	{
		auto Anims = m_pRenderModel->Get_Animations();
		auto& Channels = Anims[m_iAnimIndex]->Get_Channels();

		if (m_pRenderModel->Created_BoneCollider(Channels[m_iChannelSelectedIndex]->Get_BoneIndex()))
		{
			Animation_Event Event{ COLLIDER_ACTIVATION, m_fAnimationPosition, m_ChannelNameList[m_iChannelSelectedIndex] };

			m_AnimationEvents.emplace(m_AnimNameList[m_iAnimIndex], Event);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Collider Disable"))
	{
		auto Anims = m_pRenderModel->Get_Animations();
		auto& Channels = Anims[m_iAnimIndex]->Get_Channels();

		if (m_pRenderModel->Created_BoneCollider(Channels[m_iChannelSelectedIndex]->Get_BoneIndex()))
		{
			Animation_Event Event{ COLLIDER_DISABLE, m_fAnimationPosition, m_ChannelNameList[m_iChannelSelectedIndex] };

			m_AnimationEvents.emplace(m_AnimNameList[m_iAnimIndex], Event);
		}
	}
#pragma endregion

#pragma region 사운드 이벤트 버튼
	ImGui::Text("Sound Event");
	if (ImGui::Button("Add Sound"))
	{

	}
#pragma endregion

#pragma region 이펙트 이벤트 버튼들
	ImGui::Text("Effect Event");
	if (ImGui::Button("Add Effect"))
	{

	}
#pragma endregion



	ImGui::End();
}

void CImguiManager::AnimEventWindow()
{
	ImGui::Begin("Animation Events", NULL);
	//ImGui::Begin("FX", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_HorizontalScrollbar);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	DrawTimeline(draw_list);

	ImGui::End();
}

void CImguiManager::DrawTimeline(ImDrawList* draw_list)
{
	ImGui::Text("Animation Name : %s", m_AnimNameList[m_iAnimIndex].c_str());

	ImVec2 vCanvas_Start = ImGui::GetCursorScreenPos(); // 시작 위치	
	ImVec2 vCanvas_Size = ImGui::GetContentRegionAvail(); // 크기	
	ImVec2 vCanvas_End = vCanvas_Start + vCanvas_Size;

	draw_list->AddRectFilled(vCanvas_Start, vCanvas_End, IM_COL32(50, 50, 50, 255));

	_float fCircleRadius = 5.f;

	auto lower_bound_iter = m_AnimationEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_AnimationEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	_float vCircleStartPosY = vCanvas_Start.y + fCircleRadius * 2.f;
	_float vCircleStartPosX = vCanvas_Start.x + fCircleRadius * 4.f;
	_float vPosY = 0.f;

	//콜라이더 활성화(노랑), 콜라이더 비활성화(주황), 사운드 활성화(초록), 이펙트 활성화(파랑)
	ImU32 CircleColor = IM_COL32_WHITE;

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		Animation_Event Value = (*lower_bound_iter).second;

		//vCanvas_Size
		auto Anims = m_pRenderModel->Get_Animations();

		_float fDuration = _float(*(Anims[m_iAnimIndex]->Get_Duration()));

		_float fRatio = Value.fAinmPosition / fDuration;

		vCanvas_Size.x * fRatio;

		switch (Value.iType)
		{
		case COLLIDER_ACTIVATION:
			CircleColor = IM_COL32(255, 255, 0, 255);
			break;
		case COLLIDER_DISABLE:
			CircleColor = IM_COL32(255, 127, 0, 255);
			break;
		case SOUND_ACTIVATION:
			CircleColor = IM_COL32(0, 255, 0, 255);
			break;
		case EFFECT_ACTIVATION:
			CircleColor = IM_COL32(0, 0, 255, 255);
			break;
		}

		vPosY = vCircleStartPosY + (Value.iType * fCircleRadius * 2.5f);

		draw_list->AddCircleFilled(ImVec2(vCanvas_Start.x + vCanvas_Size.x * fRatio, vPosY), fCircleRadius, CircleColor);
	}
}

void CImguiManager::DrawChannels()
{
	auto Anims = m_pRenderModel->Get_Animations();
	auto& Channels = Anims[m_iAnimIndex]->Get_Channels();

	m_ChannelNameList.clear();
	m_ChannelNameList.resize(Channels.size());

	ImGui::InputText("Model Name", m_szSearchChannelName, sizeof(m_szSearchChannelName) * _MAX_PATH);

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

	if(ImGui::Button("test"))
	{
		for (size_t i = 0; i < m_ChannelNameList.size(); i++)
		{
			if(m_ChannelNameList[i] == string(m_szSearchChannelName))
				m_iChannelSelectedIndex = i;
		}
	}

	ImGui::ScrollToItem(ImGuiScrollFlags_AlwaysCenterY);
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

void CImguiManager::Update_Model_Position()
{
	_vector vPosition = XMVectorSet(m_ModelPosition[0], m_ModelPosition[1], m_ModelPosition[2], 1.f);
	m_pRenderModel->Set_Position(vPosition);
}

void CImguiManager::Update_Model_RotationX()
{
	m_pRenderModel->Set_Rotation(0, XMConvertToRadians(m_ModelRotation[0]), m_pGameInstance->Get_TimeDelta(TEXT("Timer_Edit")));
}

void CImguiManager::Update_Model_RotationY()
{
	m_pRenderModel->Set_Rotation(1, XMConvertToRadians(m_ModelRotation[1]), m_pGameInstance->Get_TimeDelta(TEXT("Timer_Edit")));
}

void CImguiManager::Update_Model_RotationZ()
{
	m_pRenderModel->Set_Rotation(2, XMConvertToRadians(m_ModelRotation[2]), m_pGameInstance->Get_TimeDelta(TEXT("Timer_Edit")));
}

void CImguiManager::Update_Model_Scaled()
{
	m_pRenderModel->Set_Scaled(m_ModelScale, m_ModelScale, m_ModelScale);
}

void CImguiManager::Reset_Collider_Value()
{
	ZeroMemory(m_ColliderExtents, sizeof(float) * 3);
	m_fColliderRadius = 0.f;
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
