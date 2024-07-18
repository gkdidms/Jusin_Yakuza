#include "ImguiManager.h"
#include "GameInstance.h"

#include "AnimModel.h"
#include "ModelBoneSphere.h"
#include "TRailEffect.h"

#pragma region "Model"
#include "Animation.h"
#include "Anim.h"
#include "Bone.h"
#include "Channel.h"
#include "Mesh.h"
#pragma endregion

wstring g_wstrModelName;

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
	Setting_InitialData();

	//ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &(ImGui::GetIO()); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io->Fonts->AddFontFromFileTTF("C:\\Windows\\fonts\\malgun.ttf", 18.0f, NULL, io->Fonts->GetGlyphRangesKorean());

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);
	return S_OK;
}

void CImguiManager::Tick(const _float& fTimeDelta)
{
	if (LEVEL_EDIT == m_pGameInstance->Get_CurrentLevel())
		Connect_Model_Ref();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

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

		if (m_isEffectListWindow)
			EffectListWindow();
		if (m_isSoundListWindow)
			SoundListWindow();
		if (m_isRimLightWindow)
			RimLightWindow();
		if (m_isTrailWindow)
			TrailWindow();
	}

	// �ִϸ��̼� �̺�Ʈ�� �ִ� �ݶ��̴��� ������ �þȻ����� �ٲ��ִ� ���
	if (m_pGameInstance->Get_CurrentLevel() != LEVEL_LOADING)
	{
		if (0 < m_AnimNameList.size())
		{
			auto lower_bound_iter = m_AnimationEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

			// ��ȯ�� iter�� Ű���� �ٸ��ٸ� �� ���� �ش� Ű���� �������� �ʴ´ٴ� ��
			if (lower_bound_iter != m_AnimationEvents.end() && (*lower_bound_iter).first != m_AnimNameList[m_iAnimIndex])
				return;

			auto upper_bound_iter = m_AnimationEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

			auto pBoneSpheres = m_pRenderModel->Get_BoneSpheres();

			for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
			{
				Animation_Event Value = (*lower_bound_iter).second;

				CModel* pModel = static_cast<CModel*>(m_pRenderModel->Get_Model());
				if (nullptr != pModel)
				{
					_double Position = 0.0;
					if(m_iModelType == PLAYER)
						Position = *pModel->Get_AnimationCurrentPosition();
					else
						Position = *pModel->Get_AnimationCurrentPosition(m_pRenderModel->Get_AnimComponent());

					if (Value.iType == COLLIDER_ACTIVATION && Value.fAinmPosition < Position)
					{
						pBoneSpheres[Value.iBoneIndex]->Change_ColliderColor(true);
					}
					if (Value.iType == COLLIDER_DISABLE && Value.fAinmPosition < Position)
					{
						pBoneSpheres[Value.iBoneIndex]->Change_ColliderColor(false);
					}
				}
			}
		}

	}

}



HRESULT CImguiManager::Render()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	return S_OK;
}

void CImguiManager::ModelList()
{
	ImGui::Text(u8"�� üũ �� AnimComponent�� �ִϸ��̼� ������� �����Ѵ�");
	if (ImGui::RadioButton(u8"�÷��̾�", m_iModelType == PLAYER))
	{
		m_iModelType = PLAYER;
		m_Anims = m_pRenderModel->Get_Animations();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"��", m_iModelType == ENEMY))
	{
		m_iModelType = ENEMY;

		auto pAnimCom = m_pRenderModel->Get_AnimComponent();
		m_Anims = pAnimCom->Get_Animations();
	}

	Setting_AnimationList();

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

	if (ImGui::DragFloat("All Scale", &m_ModelScale, 0.001f))
	{
		if (0.001f > m_ModelScale) m_ModelScale = 0.001f;

		Update_Model_Scaled();
	}

	ImGui::NewLine();
	ImGui::Text("Model List");

	vector<const char*> items;

	if (0 < m_ModelNameList.size()) {
		for (const string& str : m_ModelNameList) {
			items.push_back(str.c_str());
		}
	}

	if (ImGui::ListBox("##", &m_iModelSelectedIndex, items.data(), m_ModelNameList.size()))
	{
		m_iAnimIndex = 0;
		m_iSearchAnimIndex = 0;
		m_iAddedAnimSelectedIndex = 0;
		m_iBoneSelectedIndex = 0;
		m_iBoneSelectedIndex = 0;
		m_iColliderSelectedIndex = 0;
		m_iEventBoneIndex = 0;
		m_iChannelSelectedIndex = 0;
		m_iEventSelectedIndex = 0;
		m_iMeshSelectedIndex = 0;
		m_iAddedMeshSelectedIndex = 0;

		m_pRenderModel->Change_Model(m_pGameInstance->StringToWstring(m_ModelNameList[m_iModelSelectedIndex]));
		//All_Load();
	}

	if (ImGui::Button(u8"������ �����ϱ�"))
	{
		All_Save();
	}
	if (ImGui::Button(u8"������ �ε��ϱ�"))
	{
		All_Load();
	}

}

void CImguiManager::AnimListWindow()
{
	ImGui::Begin("Anim List", &m_isOnToolWindows);

	m_AnimNameList.clear();
	m_AnimNameList.resize(m_Anims.size());

	_uint i = 0;

	vector<const char*> items;

	for (auto pAnim : m_Anims)
	{
		string strChannelName = m_pGameInstance->Extract_String(pAnim->Get_AnimName(), '[', ']');

		m_AnimNameList[i] = strChannelName;
		items.push_back(m_AnimNameList[i].c_str());

		i++;
	}

	ImGui::Text("Animation List");
	if (ImGui::ListBox("##", &m_iAnimIndex, items.data(), items.size()))
	{
		m_fAnimationPosition = 0.f;
		m_pRenderModel->Change_Animation(m_iAnimIndex, m_iModelType == ENEMY ? true : false);
		//m_isAnimLoop = m_pRenderModel->Get_AnimLoop(m_iAnimIndex);
	}


	ImGui::InputInt("Search Anim Index", &m_iSearchAnimIndex);
	if (ImGui::Button(u8"�ִϸ��̼� �ε��� �˻��ϱ�"))
		m_iAnimIndex = m_iSearchAnimIndex;

	ImGui::SameLine();
	ImGui::Text("Anim Index: %d", m_iAnimIndex);

	if (ImGui::Button("Add"))
	{
		if (m_AddedAnims.find(m_iAnimIndex) == m_AddedAnims.end())
		{
			m_AddedAnims.emplace(m_iAnimIndex, m_AnimNameList[m_iAnimIndex]);
		}

	}

	vector<const char*> Addeditems;
	for (auto& AddedAnim : m_AddedAnims)
		Addeditems.push_back(AddedAnim.second.c_str());

	ImGui::ListBox("Loop Animations", &m_iAddedAnimSelectedIndex, Addeditems.data(), Addeditems.size());

	if (ImGui::Button(u8"���� �ִϸ��̼� ��� ����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

		AnimationLoop_Save(strDirectory);
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

	for (auto pBone : pBones)
	{
		string strChannelName = pBone->Get_Name();
		//string strName = m_pGameInstance->Extract_String(pBone->Get_Name(), '[', ']');

		m_BoneNameList[i] = strChannelName;
		items.push_back(m_BoneNameList[i].c_str());

		i++;
	}

	ImGui::InputText("Bone Name Search", m_szSearchBoneName, sizeof(m_szSearchBoneName) * _MAX_PATH);

	if (ImGui::Button(u8"�� �̸� �˻��ϱ�"))
	{
		for (size_t i = 0; i < m_BoneNameList.size(); i++)
		{
			if (m_BoneNameList[i] == string(m_szSearchBoneName))
				m_iBoneSelectedIndex = i;
		}

		Gui_Select_Bone(m_iBoneSelectedIndex);
	}

	//�� ����Ʈ
	if (ImGui::ListBox("##", &m_iBoneSelectedIndex, items.data(), items.size()))
	{
		Gui_Select_Bone(m_iBoneSelectedIndex);
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
		if (ImGui::DragFloat3("Extents", m_ColliderExtents, 0.1f))
			m_pRenderModel->Set_Collider_Value(m_iBoneSelectedIndex, m_ColliderExtents);
		break;
	}
	case OBB:
	{
		if (ImGui::DragFloat3("Extents", m_ColliderExtents, 0.1f))
			m_pRenderModel->Set_Collider_Value(m_iBoneSelectedIndex, m_ColliderExtents);
		break;
	}
	case SPHERE:
	{
		if (ImGui::DragFloat("Radius", &m_fColliderRadius, 0.1f))
			m_pRenderModel->Set_Collider_Value(m_iBoneSelectedIndex, &m_fColliderRadius);
		break;
	}
	}

	if (m_iColliderActionType == HIT)
		m_isAlwaysCollider = true;
	else
		m_isAlwaysCollider = false;

	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	ImGui::Checkbox(u8"���", &m_isAlwaysCollider);
	ImGui::PopItemFlag();
	ImGui::SameLine();

	if (ImGui::RadioButton("Attack", m_iColliderActionType == ATTACK))
	{
		m_iColliderActionType = ATTACK;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Hit", m_iColliderActionType == HIT))
	{
		m_iColliderActionType = HIT;
	}

	switch (m_iColliderActionType)
	{
	case ATTACK:
		ImGui::Text(u8"���ݿ� �ݶ��̴� Ÿ��");
		if (ImGui::RadioButton("Hand", m_iColliderPartType == HAND_A))
		{
			m_iColliderPartType = HAND_A;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Foot", m_iColliderPartType == FOOT_A))
		{
			m_iColliderPartType = FOOT_A;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Joint", m_iColliderPartType == JOINT_A))
		{
			m_iColliderPartType = JOINT_A;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Head", m_iColliderPartType == HEAD_A))
		{
			m_iColliderPartType = HEAD_A;
		}
		break;
	case HIT:

		ImGui::Text(u8"�ǰݿ� �ݶ��̴� Ÿ��");
		if (ImGui::RadioButton("Head(Neck)", m_iColliderPartType == HEAD_H))
		{
			m_iColliderPartType = HEAD_H;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Body", m_iColliderPartType == BODY_H))
		{
			m_iColliderPartType = BODY_H;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Leg", m_iColliderPartType == LEG_H))
		{
			m_iColliderPartType = LEG_H;
		}
		break;
	}

	if (ImGui::Button("Create Collier"))
	{
		void* pValue = nullptr;
		switch (m_iColliderType)
		{
		case AABB:
			pValue = &m_ColliderExtents;
			break;
		case OBB:
			pValue = &m_ColliderExtents;
			break;
		case SPHERE:
			pValue = &m_fColliderRadius;
			break;
		}

		_float3 vCenter;
		memcpy(&vCenter, &m_ColliderPosition, sizeof(_float3));
		if (!m_pRenderModel->Created_BoneCollider(m_iBoneSelectedIndex))
		{
			m_AddedColliders.emplace(m_iBoneSelectedIndex, Collider_State{ m_BoneNameList[m_iBoneSelectedIndex], m_iColliderActionType,  m_iColliderPartType, m_isAlwaysCollider });
			m_pRenderModel->Create_BoneCollider(m_iColliderType, m_iBoneSelectedIndex, vCenter, pValue);
		}
	}

	vector<const char*> Addeditems;

	for (auto& iter : m_AddedColliders)
	{
		Addeditems.push_back(iter.second.strBoneName.c_str());
	}

	if (ImGui::ListBox(u8"�߰��� �ݶ��̴� ����Ʈ", &m_iColliderSelectedIndex, Addeditems.data(), Addeditems.size()))
	{
		auto iter = m_AddedColliders.begin();

		for (size_t i = 0; i < m_iColliderSelectedIndex; i++)
		{
			iter++;
		}

		Gui_Select_Bone((*iter).first);
		m_isAlwaysCollider = (*iter).second.isAlways;
		m_iColliderActionType = (*iter).second.iColliderActionType;
		m_iColliderPartType = (*iter).second.iColliderPartType;
	}

	if (ImGui::Button(u8"�ݶ��̴� ����"))
	{
		auto iter = m_AddedColliders.begin();

		for (size_t i = 0; i < m_iColliderSelectedIndex; i++)
		{
			iter++;
		}

		m_pRenderModel->Release_BoneCollider((*iter).first);
		m_AddedColliders.erase((*iter).first);		// ���� ������ ���ε����� �������� �������ϱ� ������ m_iColliderSelectedIndex�� �ƴ϶� m_iBoneSelectedIndex�� �������Ѵ�.
	}

	if (ImGui::Button(u8"�ݶ��̴� ���� ����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

		ColliderState_Save(strDirectory);
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
		string strChannelName = pMesh->Get_Name();
		//string strName = m_pGameInstance->Extract_String(pMesh->Get_Name(), '[', ']');

		m_MeshNameList[i] = strChannelName;
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
	// ���� ��� �����ϴ� ��ư
	if (ImGui::Button("Apply"))
	{
		for (auto& AddedMesh : m_AddedMeshes)
		{
			pMeshes[AddedMesh.first]->Set_AlphaApply(true);
		}
	}

	//�߰��� �޽� ����Ʈ
	vector<const char*> Addeditems;
	for (auto& AddedMesh : m_AddedMeshes)
	{
		Addeditems.push_back(AddedMesh.second.c_str());
	}
	ImGui::ListBox("Added Meshes", &m_iAddedMeshSelectedIndex, Addeditems.data(), Addeditems.size());

	if (ImGui::Button(u8"�߰��� �޽� ����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

		AlphaMesh_Save(strDirectory);
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
	if (m_Anims.size() < 1) return;

	ImGui::Begin("KeyFrame", NULL);

	auto Anims = m_Anims;

	_float Duration = (_float)(*(Anims[m_iAnimIndex]->Get_Duration()));
	_float CurrentPosition = (_float)(*(Anims[m_iAnimIndex]->Get_CurrentPosition()));

	// �ִϸ��̼� ��� ��ġ�� �����ش�.
	ImGui::SliderFloat("##", &CurrentPosition, 0.f, Duration);

	Anims[m_iAnimIndex]->Set_CurrentPosition((_double)CurrentPosition);
	Anims[m_iAnimIndex]->Update_KeyframeIndex();

#pragma region �ִϸ��̼� ��� ���� ��ư
	if (ImGui::Button("Stop/Play"))
	{
		m_isPause = !m_isPause;

		if (m_isPause)
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.f);
		else
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fTimeDeltaScale);
	}
	ImGui::SameLine();

	if (ImGui::SliderFloat("Speed Scale", &m_fTimeDeltaScale, 0.f, 3.f))
	{
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fTimeDeltaScale);
	}
	ImGui::SameLine();

#pragma endregion

	//���� �ִϸ��̼ǿ� �´� ä�ε��� �����ش�
	DrawChannels();

	// �̺�Ʈ�� �߰��� �ִϸ��̼� ������
	ImGui::InputFloat("Animation Position", &m_fAnimationPosition);

#pragma region �ݶ��̴� �̺�Ʈ ��ư��
	ImGui::Text("Collider Event");
	if (ImGui::Button("Collider Activation"))
	{
		auto Anims = m_Anims;
		auto& Channels = Anims[m_iAnimIndex]->Get_Channels();

		if (m_pRenderModel->Created_BoneCollider(Channels[m_iChannelSelectedIndex]->Get_BoneIndex()))
		{
			Animation_Event Event{ COLLIDER_ACTIVATION, m_fAnimationPosition, m_ChannelNameList[m_iChannelSelectedIndex], m_iBoneSelectedIndex };

			m_AnimationEvents.emplace(m_AnimNameList[m_iAnimIndex], Event);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Collider Disable"))
	{
		auto Anims = m_Anims;
		auto& Channels = Anims[m_iAnimIndex]->Get_Channels();

		if (m_pRenderModel->Created_BoneCollider(Channels[m_iChannelSelectedIndex]->Get_BoneIndex()))
		{
			Animation_Event Event{ COLLIDER_DISABLE, m_fAnimationPosition, m_ChannelNameList[m_iChannelSelectedIndex], m_iBoneSelectedIndex };

			m_AnimationEvents.emplace(m_AnimNameList[m_iAnimIndex], Event);
		}
	}
#pragma endregion

#pragma region ����Ʈ �̺�Ʈ ��ư��
	ImGui::Text("Effect Event");
	if (ImGui::Button("Add Effect"))
	{
		m_isEffectListWindow = true;
	}
#pragma endregion

#pragma region ���� �̺�Ʈ ��ư
	ImGui::Text("Sound Event");
	if (ImGui::Button("Add Sound"))
	{
		m_isSoundListWindow = true;
	}
#pragma endregion

#pragma region ������Ʈ �̺�Ʈ ��ư
	ImGui::Text("RimLight Event");
	if (ImGui::Button("RimLight Op"))
	{
		m_isRimLightWindow = true;
	}
#pragma endregion

#pragma region Ʈ���� �̺�Ʈ ��ư
	ImGui::Text("Trail Event");
	if (ImGui::Button("Trail Op"))
	{
		m_isTrailWindow = true;
	}
#pragma endregion

	if (ImGui::Button(u8"�ִϸ��̼� �̺�Ʈ ����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

		AnimationEvent_Save(strDirectory);
	}

	ImGui::End();
}

void CImguiManager::AnimEventWindow()
{
	if (m_AnimNameList.size() < 1) return;

	ImGui::Begin("Animation Events", NULL);

	vector<_uint> EventTypes;
	vector<const char*> items;
	for (auto& Event : m_AnimationEvents)
	{
		if (Event.first == m_AnimNameList[m_iAnimIndex])
			EventTypes.push_back(Event.second.iType);
	}
	for (auto& iEventType : EventTypes)
	{
		switch (iEventType)
		{
		case 0:
			items.push_back("Collider On");
			break;
		case 1:
			items.push_back("Collider Off");
			break;
		case 2:
			items.push_back("Sound On");
			break;
		}
	}

	if (ImGui::ListBox("##", &m_iEventSelectedIndex, items.data(), items.size()))
	{
		// ��� �̺�Ʈ���� Selected�� ����
		for (auto& Event : m_AnimationEvents)
			Event.second.isSelected = false;

		// ���� ���õ� �ִϸ��̼� �̸��� key�� ���� ������ ù��°�� ã�Ƽ�
		auto lower_bound_iter = m_AnimationEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

		//���õ� �̺�Ʈ��ŭ ���� ������Ű��
		for (size_t i = 0; i < m_iEventSelectedIndex; i++)
			lower_bound_iter++;

		// ã�� ���Ҹ� ������ ������ �����Ѵ�
		(*lower_bound_iter).second.isSelected = true;
	}

	ImGui::SameLine();
	if (ImGui::Button("Event Delete"))
	{
		auto lower_bound_iter = m_AnimationEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

		for (size_t i = 0; i < m_iEventSelectedIndex; i++)
			lower_bound_iter++;

		m_AnimationEvents.erase(lower_bound_iter);
	}

	ImGui::Text("Animation Name : %s", m_AnimNameList[m_iAnimIndex].c_str());

	auto lower_bound_iter = m_AnimationEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_AnimationEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		Animation_Event Value = (*lower_bound_iter).second;
		if (Value.isSelected) m_iEventBoneIndex = Value.iBoneIndex;
	}

	if (-1 < m_iEventBoneIndex)
		ImGui::Text("[Bone Info]\nIndex: %i Name: %s", m_iEventBoneIndex, m_BoneNameList[m_iEventBoneIndex].c_str());
	else
		ImGui::Text("[Bone Info]\nNone");

	ImGui::Text(u8"Bone Info�� None�̶�� ���õ� �̺�Ʈ�� ���ų�\n���� ������ ���� �ʴ� �̺�Ʈ�� ������ ����");

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	DrawTimeline(draw_list);

	ImGui::End();
}

void CImguiManager::EffectListWindow()
{
	ImGui::Begin("Effect List", &m_isEffectListWindow);

	if (ImGui::Checkbox(u8"����Ʈ ����", &m_isEffectOff))
	{
		if (m_isEffectOff)
		{
			auto pGameObjects = m_pGameInstance->Get_GameObjects(LEVEL_EDIT, TEXT("Layer_Effect"));
			for (auto& pObject : pGameObjects)
			{
				static_cast<CEffect*>(pObject)->Set_Off(true);
			}
		}
		else
		{
			auto pGameObjects = m_pGameInstance->Get_GameObjects(LEVEL_EDIT, TEXT("Layer_Effect"));
			for (auto& pObject : pGameObjects)
			{
				static_cast<CEffect*>(pObject)->Set_Off(false);
			}
		}

	}

	for (size_t i = 0; i < m_EffectTypeList.size(); i++)
	{
		ImGui::SameLine();
		if (ImGui::RadioButton(m_EffectTypeList[i].c_str(), m_iEffectType == i))
		{
			m_iEffectType = i;
		}
	}

	//�߰��� �޽� ����Ʈ
	auto lower_bound_iter = m_EffectFiles.lower_bound(m_iEffectType);
	auto upper_bound_iter = m_EffectFiles.upper_bound(m_iEffectType);

	vector<const char*> items;
	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		items.push_back((*lower_bound_iter).second.c_str());
	}

	ImGui::ListBox("##", &m_iEffectSelectedIndex, items.data(), items.size());
	m_SelectedEffectName = m_pGameInstance->Get_FileName(items[m_iEffectSelectedIndex]);

	if (ImGui::Button(u8"����Ʈ ����"))
	{
		Create_Effect(m_BoneNameList[m_iBoneSelectedIndex], m_SelectedEffectName);
	}


	vector<const char*> Addeditems;
	for (auto& strEffect : m_EffectState)
		Addeditems.push_back(strEffect.second.data());

	auto iter = m_EffectState.begin();
	for (size_t i = 0; i < m_iAddedEffectSelectedIndex; i++)	iter++;

	if (0 < Addeditems.size())
	{
		if (iter != m_EffectState.end())
			ImGui::Text(u8"��� �� �̸�: %s", (*iter).first.c_str());
	}
	else
		ImGui::Text(u8"��� �� �̸�: None");

	ImGui::ListBox("Added Effects", &m_iAddedEffectSelectedIndex, Addeditems.data(), Addeditems.size());
	if (ImGui::Button(u8"����Ʈ ����"))
	{
		CEffect* pEffect = reinterpret_cast<CEffect*>(m_pGameInstance->Get_GameObject(LEVEL_EDIT, TEXT("Layer_Effect"), m_iAddedEffectSelectedIndex));
		pEffect->Set_Dead();

		m_EffectState.erase(iter);
	}

	if (ImGui::Button(u8"����Ʈ ���� ����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

		EffectState_Save(strDirectory);
	}

	ImGui::End();
}

void CImguiManager::RimLightWindow()
{
	ImGui::Begin(u8"������Ʈ ���� â", &m_isRimLightWindow);

	ImGui::Text(u8"���õ� �ִϸ��̼�: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::Text(u8"���õ� �޽�: %s", m_MeshNameList[m_iMeshSelectedIndex].c_str());

	ImGui::Text(u8"���� �Էµ� �ִϸ��̼� ������: %f", m_fAnimationPosition);

	if (ImGui::Button(u8"������Ʈ On"))
	{
		Animation_RimLightState RimLightState{};
		RimLightState.iType = 0;					
		RimLightState.fAinmPosition = m_fAnimationPosition;
		RimLightState.strMeshName = m_MeshNameList[m_iMeshSelectedIndex];

		m_RimLightEvents.emplace(m_AnimNameList[m_iAnimIndex], RimLightState);
	}

	if (ImGui::Button(u8"������Ʈ Off"))
	{
		Animation_RimLightState RimLightState{};
		RimLightState.iType = 1;
		RimLightState.fAinmPosition = m_fAnimationPosition;
		RimLightState.strMeshName = m_MeshNameList[m_iMeshSelectedIndex];

		m_RimLightEvents.emplace(m_AnimNameList[m_iAnimIndex], RimLightState);
	}

	//������Ʈ �̺�Ʈ ����Ʈ
	auto lower_bound_iter = m_RimLightEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_RimLightEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	ImGui::Text(u8"������Ʈ �̺�Ʈ ����Ʈ");
	vector<const char*> items;
	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		items.push_back((*lower_bound_iter).second.strMeshName.c_str());
	}

	ImGui::ListBox("##", &m_iRimLightEventIndex, items.data(), items.size());

	auto lower_iter = m_RimLightEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	for (size_t i = 0; i < m_iRimLightEventIndex; i++)
	{
		lower_iter++;
	}

	if (lower_iter != upper_bound_iter && lower_iter != m_RimLightEvents.end())
	{
		if ((*lower_iter).second.iType == 0)
		{
			ImGui::Text(u8"On �̺�Ʈ");

			ImGui::Text(u8"����� �ִϸ��̼�: %s", m_AnimNameList[m_iAnimIndex].c_str());
			ImGui::Text(u8"����� �޽�: %s", (*lower_iter).second.strMeshName.c_str());

			ImGui::Text(u8"����� �ִϸ��̼� ������: %f", (*lower_iter).second.fAinmPosition);
		}
		else
		{
			ImGui::Text(u8"Off �̺�Ʈ");

			ImGui::Text(u8"����� �ִϸ��̼�: %s", m_AnimNameList[m_iAnimIndex].c_str());
			ImGui::Text(u8"����� �޽�: %s", (*lower_iter).second.strMeshName.c_str());

			ImGui::Text(u8"����� �ִϸ��̼� ������: %f", (*lower_iter).second.fAinmPosition);
		}
	}

	// ������Ʈ ������ �޽� �̸� �����ϴ� �Լ�
	Setting_RimLight();

	if (ImGui::Button(u8"����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		RimEvent_Save(strDirectory);
	}
		

	ImGui::End();
}

void CImguiManager::TrailWindow()
{
	ImGui::Begin(u8"Ʈ���� ���� â", &m_isTrailWindow);

	ImGui::Text(u8"���õ� �ִϸ��̼�: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::Text(u8"���õ� ��(ä�θ���Ʈ���� ������ ��): %s", m_ChannelNameList[m_iChannelSelectedIndex].c_str());

	ImGui::Text(u8"���� �Էµ� �ִϸ��̼� ������: %f", m_fAnimationPosition);

	if (ImGui::Button(u8"Ʈ���� On"))
	{
		Animation_TrailState TrailState{};
		TrailState.iType = 0;
		TrailState.fAinmPosition = m_fAnimationPosition;
		TrailState.strBonelName = m_ChannelNameList[m_iChannelSelectedIndex];

		auto& Channels = m_Anims[m_iAnimIndex]->Get_Channels();
		TrailState.iBoneIndex = Channels[m_iChannelSelectedIndex]->Get_BoneIndex();

		m_TrailEvents.emplace(m_AnimNameList[m_iAnimIndex], TrailState);

		Create_Effect(m_BoneNameList[m_iBoneSelectedIndex], m_SelectedEffectName, TEXT("Layer_Trail"));
	}

	if (ImGui::Button(u8"Ʈ���� Off"))
	{
		Animation_TrailState TrailState{};
		TrailState.iType = 1;
		TrailState.fAinmPosition = m_fAnimationPosition;
		TrailState.strBonelName = m_ChannelNameList[m_iChannelSelectedIndex];

		auto& Channels = m_Anims[m_iAnimIndex]->Get_Channels();
		TrailState.iBoneIndex = Channels[m_iChannelSelectedIndex]->Get_BoneIndex();

		m_TrailEvents.emplace(m_AnimNameList[m_iAnimIndex], TrailState);
	}

	//������Ʈ �̺�Ʈ ����Ʈ
	auto lower_bound_iter = m_TrailEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_TrailEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	ImGui::Text(u8"Ʈ���� �̺�Ʈ ����Ʈ");
	vector<const char*> items;
	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		items.push_back((*lower_bound_iter).second.strBonelName.c_str());
	}

	ImGui::ListBox("##", &m_iTrailEventIndex, items.data(), items.size());

	auto lower_iter = m_TrailEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	for (size_t i = 0; i < m_iTrailEventIndex; i++)
	{
		lower_iter++;
	}

	if (lower_iter != upper_bound_iter && lower_iter != m_TrailEvents.end())
	{
		if ((*lower_iter).second.iType == 0)
		{
			ImGui::Text(u8"On �̺�Ʈ");

			ImGui::Text(u8"����� �ִϸ��̼�: %s", m_AnimNameList[m_iAnimIndex].c_str());
			ImGui::Text(u8"����� �� �̸�: %s", (*lower_iter).second.strBonelName.c_str());
			ImGui::Text(u8"����� �� �ε���: %i", (*lower_iter).second.iBoneIndex);

			ImGui::Text(u8"����� �ִϸ��̼� ������: %f", (*lower_iter).second.fAinmPosition);
		}
		else
		{
			ImGui::Text(u8"Off �̺�Ʈ");

			ImGui::Text(u8"����� �ִϸ��̼�: %s", m_AnimNameList[m_iAnimIndex].c_str());
			ImGui::Text(u8"����� �� �̸�: %s", (*lower_iter).second.strBonelName.c_str());
			ImGui::Text(u8"����� �� �ε���: %i", (*lower_iter).second.iBoneIndex);

			ImGui::Text(u8"����� �ִϸ��̼� ������: %f", (*lower_iter).second.fAinmPosition);
		}
	}

	// Ʈ���� ��/���� ���� �Լ�
	Setting_Trail();

	if (ImGui::Button(u8"����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		TrailEvent_Save(strDirectory);
	}

	ImGui::End();
}

void CImguiManager::SoundListWindow()
{
	ImGui::Begin("Sound List", &m_isSoundListWindow);

	ImGui::Text(u8"�׽�Ʈ������ ������ϴ�");

	ImGui::End();
}

void CImguiManager::DrawTimeline(ImDrawList* draw_list)
{
	ImVec2 vCanvas_Start = ImGui::GetCursorScreenPos();			// ���� ��ġ	
	ImVec2 vCanvas_Size = ImGui::GetContentRegionAvail();		// ũ��	
	ImVec2 vCanvas_End = vCanvas_Start + vCanvas_Size;

	draw_list->AddRectFilled(vCanvas_Start, vCanvas_End, IM_COL32(50, 50, 50, 255));

	_float fCircleRadius = 5.f;

	auto lower_bound_iter = m_AnimationEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_AnimationEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	_float vCircleStartPosY = vCanvas_Start.y + fCircleRadius * 2.f;
	_float vCircleStartPosX = vCanvas_Start.x + fCircleRadius * 4.f;
	_float vPosY = 0.f;

	//�ݶ��̴� Ȱ��ȭ(���), �ݶ��̴� ��Ȱ��ȭ(��Ȳ), ���� Ȱ��ȭ(�ʷ�), ����Ʈ Ȱ��ȭ(�Ķ�)
	ImU32 CircleColor = IM_COL32_WHITE;

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		Animation_Event Value = (*lower_bound_iter).second;

		//vCanvas_Size
		auto Anims = m_Anims;

		_float fDuration = _float(*(Anims[m_iAnimIndex]->Get_Duration()));

		_float fRatio = Value.fAinmPosition / fDuration;

		vCanvas_Size.x* fRatio;

		if (Value.isSelected)
		{
			CircleColor = IM_COL32(255, 0, 0, 255);
		}
		else
		{
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
			}
		}


		vPosY = vCircleStartPosY + (Value.iType * fCircleRadius * 2.5f);

		draw_list->AddCircleFilled(ImVec2(vCanvas_Start.x + vCanvas_Size.x * fRatio, vPosY), fCircleRadius, CircleColor);
	}
}

void CImguiManager::DrawChannels()
{
	auto Anims = m_Anims;
	auto& Channels = Anims[m_iAnimIndex]->Get_Channels();

	m_ChannelNameList.clear();
	m_ChannelNameList.resize(Channels.size());

	ImGui::InputText("Channel Name", m_szSearchChannelName, sizeof(m_szSearchChannelName) * _MAX_PATH);

	_uint i = 0;

	vector<const char*> items;

	for (auto pChannel : Channels)
	{
		string strChannelName = pChannel->Get_Name();
		//string strName = m_pGameInstance->Extract_String(pBone->Get_Name(), '[', ']');

		m_ChannelNameList[i] = strChannelName;
		items.push_back(m_ChannelNameList[i].c_str());

		i++;
	}

	ImGui::InputText("Channel Name", m_szSearchChannelName, sizeof(m_szSearchChannelName) * _MAX_PATH);

	if (ImGui::Button(u8"ä��(=��) �̸� �˻��ϱ�"))
	{
		for (size_t i = 0; i < m_ChannelNameList.size(); i++)
		{
			if (m_ChannelNameList[i] == string(m_szSearchChannelName))
			{
				m_iChannelSelectedIndex = i;

				m_iBoneSelectedIndex = Channels[m_iChannelSelectedIndex]->Get_BoneIndex();

				Gui_Select_Bone(m_iBoneSelectedIndex);
				break;
			}
		}
	}

	ImGui::ScrollToItem(ImGuiScrollFlags_AlwaysCenterY);
	if (ImGui::ListBox("##", &m_iChannelSelectedIndex, items.data(), items.size()))
	{
		m_iBoneSelectedIndex = Channels[m_iChannelSelectedIndex]->Get_BoneIndex();

		m_pRenderModel->Select_Bone(m_iBoneSelectedIndex);
	}
}

void CImguiManager::Connect_Model_Ref()
{
	m_pRenderModel = static_cast<CAnimModel*>(m_pGameInstance->Get_GameObject(LEVEL_EDIT, TEXT("Layer_Object"), 0));
}

string CImguiManager::Get_FirstModel_Name()
{
	return m_ModelNameList.front();
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
	ZeroMemory(m_ColliderPosition, sizeof(float) * 3);
	ZeroMemory(m_ColliderExtents, sizeof(float) * 3);
	m_fColliderRadius = 0.f;
}

void CImguiManager::Clear_EffectStateMap()
{
	auto pEffectLayer = m_pGameInstance->Get_GameObjects(LEVEL_EDIT, TEXT("Layer_Effect"));

	for (auto& pEffect : pEffectLayer)
		pEffect->Set_Dead();

	m_EffectState.clear();
}

void CImguiManager::Create_Effect(string& strBoneName, string& strEffectName, wstring wstrLayer)
{
	auto& pBones = m_pRenderModel->Get_Bones();

	const _float4x4* pBoneMatrix = { nullptr };

	for (auto& pBone : pBones)
	{
		if (pBone->Compare_Name(strBoneName.c_str()))
			pBoneMatrix = pBone->Get_CombinedTransformationMatrix();
	}

	CEffect::EFFECT_DESC Desc{};
	Desc.pWorldMatrix = pBoneMatrix;

	if(wstrLayer == TEXT("Layer_Effect"))
		m_pGameInstance->Add_GameObject(LEVEL_EDIT, m_pGameInstance->StringToWstring(strEffectName), wstrLayer, &Desc);
	else if (wstrLayer == TEXT("Layer_Trail"))
	{
		m_pRenderModel->Create_Trail(strBoneName, m_pGameInstance->Clone_Object(m_pGameInstance->StringToWstring(strEffectName), &Desc));
	}

	m_EffectState.emplace(strBoneName, strEffectName);
}

void CImguiManager::Setting_Collider_Value(_uint iBoneIndex)
{
	auto& pBoneSpheres = m_pRenderModel->Get_BoneSpheres();

	auto pSphere = pBoneSpheres[iBoneIndex];

	if (!pSphere->Created_Collider())
		return;

	// ������ �� �ε����� �� ����Ʈ�� ����ȭ
	m_iBoneSelectedIndex = iBoneIndex;

	int iType = pSphere->Get_Collider_Type();
	m_iColliderType = iType;

	_float3 vCenter = pSphere->Get_Collider_Center();
	memcpy(&m_ColliderPosition, &vCenter, sizeof(_float3));

	switch (iType)
	{
	case CCollider::COLLIDER_AABB:
	{
		_float3* vExtents = (_float3*)pSphere->Get_Collider_Value();
		memcpy(&m_ColliderExtents, vExtents, sizeof(_float3));
	}
	case CCollider::COLLIDER_OBB:
	{
		_float3* vExtents = (_float3*)pSphere->Get_Collider_Value();
		memcpy(&m_ColliderExtents, vExtents, sizeof(_float3));
	}
	case CCollider::COLLIDER_SPHERE:
	{
		_float* fRadius = (_float*)pSphere->Get_Collider_Value();
		memcpy(&m_fColliderRadius, fRadius, sizeof(_float));
	}
	}
}


void CImguiManager::All_Save()
{
	string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

	fs::path path(strDirectory);
	if (!exists(path))
		fs::create_directory(path);

	/* �� �Ž� ���� �����ϱ� */
	AlphaMesh_Save(strDirectory);

	/* �ִϸ��̼� ���� ���� �����ϱ� */
	AnimationLoop_Save(strDirectory);

	/* �ִϸ��̼� �̺�Ʈ �����ϱ� */
	AnimationEvent_Save(strDirectory);

	/* �浹ü ���� ���� �����ϱ� */
	ColliderState_Save(strDirectory);

	/* ����Ʈ ���� ���� �����ϱ� */
	EffectState_Save(strDirectory);

	/* ������Ʈ �̺�Ʈ ���� �����ϱ� */
	RimEvent_Save(strDirectory);

	/* Ʈ���� �̺�Ʈ �����ϱ� */
	TrailEvent_Save(strDirectory);
}

void CImguiManager::AlphaMesh_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_AlphaMeshes.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iMeshMapSize = m_AddedMeshes.size();

	out.write((char*)&iMeshMapSize, sizeof(_uint));

	// �Ž� �о���� �ε��� ������ �ٲ� ���� ����?
	// ���ٰ� �����ϰ� �ε����� �����ϱ�
	//string strModelName = m_ModelNameList[m_iModelSelectedIndex];

	for (auto& iter : m_AddedMeshes)
	{
		out.write((char*)&(iter.first), sizeof(_uint));
	}

	out.close();
}

void CImguiManager::AnimationLoop_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_LoopAnimations.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iAnimMapSize = m_AddedAnims.size();

	out.write((char*)&iAnimMapSize, sizeof(_uint));

	for (auto& iter : m_AddedAnims)
	{
		out.write((char*)&(iter.first), sizeof(_uint));
	}

	out.close();
}

void CImguiManager::AnimationEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_AnimationEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iEventMapSize = m_AnimationEvents.size();

	out << iEventMapSize;

	// first: �ִϸ��̼� �̸�, second: �̺�Ʈ����
	// � �ִϸ��̼ǿ� ���� ���������� ������ (�ִϸ��̼� �̸��� key�� ����)
	for (auto& iter : m_AnimationEvents)
	{
		string strAnimName = iter.first;
		out << strAnimName << endl;

		Animation_Event Desc{};
		Desc.iType = iter.second.iType;
		Desc.fAinmPosition = iter.second.fAinmPosition;
		Desc.strChannelName = iter.second.strChannelName;
		Desc.iBoneIndex = iter.second.iBoneIndex;

		out << Desc.iType << endl;
		out << Desc.fAinmPosition << endl;
		out << Desc.strChannelName << endl;
		out << Desc.iBoneIndex << endl;
	}

	out.close();
}

void CImguiManager::ColliderState_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_Colliders.dat";

	ofstream out(strDirectory, ios::binary);

	auto pBoneSpheres = m_pRenderModel->Get_BoneSpheres();

	_uint iColliderCount = { 0 };

	// ���⼭ i�� ���� �ε����� key������ ������ ���ϴ� ��
	_uint i = { 0 };
	map<_uint, CModelBoneSphere*> Spheres;

	for (auto& pSphere : pBoneSpheres)
	{
		if (pSphere->Created_Collider())
		{
			Spheres.emplace(i, pSphere);
		}

		i++;
	}

	iColliderCount = Spheres.size();

	// �� ��� �ݶ��̴��� �о�� ������ �ۼ��Ѵ�
	out.write((char*)&iColliderCount, sizeof(_uint));

	// ������ �����ص� �����Ǿ�� �ݶ��̴� ������ �����´�.
	for (auto& pSphere : Spheres)
	{
		CCollider* pCollider = static_cast<CCollider*>(pSphere.second->Get_Component(TEXT("Com_Collider")));

		//�о�� �ݶ��̴��� Ÿ�Կ� ���� ������ �о �����Ѵ� (���� Ÿ���̴� ������ 2�پ� �� ���̱��ϳ�, 
		// Sphere �� ��� _float �ܵ��̰�, �������� _float3�̹Ƿ� �ε� �� ���� �ʿ�
		CCollider::TYPE eType = pCollider->Get_Type();

		out.write((char*)&pSphere.first, sizeof(_uint));
		// �̳��� ��Ʈó�� 4����Ʈ�� �Ҹ��ϹǷ�, ��Ʈ ũ��� �����Ѵ�.
		out.write((char*)&eType, sizeof(_uint));

		switch (eType)
		{
		case Engine::CCollider::COLLIDER_AABB:
		{
			BoundingBox* pBounding = static_cast<BoundingBox*>(pCollider->Get_OriginDesc());
			out.write((char*)&pBounding->Center, sizeof(_float3));
			out.write((char*)&pBounding->Extents, sizeof(_float3));
		}
		break;
		case Engine::CCollider::COLLIDER_OBB:
		{
			//BoundingOrientedBox ����ü���� Orientation(_float4) ���� �ֱ� �ϳ�, 
			// ���� �������� ���� ���̹Ƿ� �������� �ʴ´�.
			BoundingOrientedBox* pBounding = static_cast<BoundingOrientedBox*>(pCollider->Get_OriginDesc());
			out.write((char*)&pBounding->Center, sizeof(_float3));
			out.write((char*)&pBounding->Extents, sizeof(_float3));
		}
		break;
		case Engine::CCollider::COLLIDER_SPHERE:
		{
			BoundingSphere* pBounding = static_cast<BoundingSphere*>(pCollider->Get_OriginDesc());
			out.write((char*)&pBounding->Center, sizeof(_float3));
			out.write((char*)&pBounding->Radius, sizeof(_float));
		}
		break;
		}

		int iColliderActionType = m_AddedColliders.at(pSphere.first).iColliderActionType;
		int iColliderPartType = m_AddedColliders.at(pSphere.first).iColliderPartType;

		out.write((char*)&iColliderActionType, sizeof(int));
		out.write((char*)&iColliderPartType, sizeof(int));
		out.write((char*)&m_AddedColliders.at(pSphere.first).isAlways, sizeof(_bool));
	}

	out.close();
}

void CImguiManager::EffectState_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_EffectState.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumEffectState = m_EffectState.size();
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	out << iNumEffectState;

	for (auto& Effect : m_EffectState)
	{
		out << Effect.first << endl;
		out << Effect.second << endl;
	}

	out.close();
}

void CImguiManager::RimEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_RimEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumRimEvents = m_RimLightEvents.size();
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	out << iNumRimEvents;

	for (auto& pair : m_RimLightEvents)
	{
		out << pair.first << endl;
		out << pair.second.iType << endl;
		out << pair.second.strMeshName << endl;
		out << pair.second.fAinmPosition << endl;
	}

	out.close();
}

void CImguiManager::TrailEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_TrailEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumTrailEvents = m_TrailEvents.size();
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	out << iNumTrailEvents;

	for (auto& pair : m_TrailEvents)
	{
		out << pair.first << endl;
		out << pair.second.iType << endl;
		out << pair.second.strBonelName << endl;
		out << pair.second.iBoneIndex << endl;
		out << pair.second.fAinmPosition << endl;
	}

	out.close();
}

void CImguiManager::All_Load()
{
	if (!m_isOnToolWindows)
	{
		MSG_BOX("Please Open the Windows");
		return;
	}

	string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

	AlphaMesh_Load(strDirectory);
	AnimationLoop_Load(strDirectory);
	AnimationEvent_Load(strDirectory);
	ColliderState_Load(strDirectory);
	EffectState_Load(strDirectory);
	RimEvent_Load(strDirectory);
	TrailEvent_Load(strDirectory);

	Setting_AnimationList();
}

void CImguiManager::AlphaMesh_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_AlphaMeshes.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("AlphaMeshes.dat ���� ����");
		return;
	}

	_uint iMeshMapSize;

	in.read((char*)&iMeshMapSize, sizeof(_uint));

	m_AddedMeshes.clear();

	for (size_t i = 0; i < iMeshMapSize; i++)
	{
		_uint iMeshIndex = { 0 };
		in.read((char*)&iMeshIndex, sizeof(_uint));
		m_AddedMeshes.emplace(i, m_MeshNameList[iMeshIndex]);
	}
	in.close();
}

void CImguiManager::AnimationLoop_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_LoopAnimations.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("LoopAnimations.dat ���� ����");
		return;
	}

	_uint iAnimMapSize;

	in.read((char*)&iAnimMapSize, sizeof(_uint));

	m_AddedAnims.clear();

	m_AnimNameList.clear();
	m_AnimNameList.resize(m_Anims.size());

	_uint i = 0;

	vector<const char*> items;

	for (auto pAnim : m_Anims)
	{
		string strChannelName = m_pGameInstance->Extract_String(pAnim->Get_AnimName(), '[', ']');

		m_AnimNameList[i] = strChannelName;
		items.push_back(m_AnimNameList[i].c_str());

		i++;
	}


	for (size_t i = 0; i < iAnimMapSize; i++)
	{
		_uint iAnimIndex = { 0 };
		in.read((char*)&iAnimIndex, sizeof(_uint));
		m_AddedAnims.emplace(iAnimIndex, m_AnimNameList[iAnimIndex]);
	}

	in.close();
}

void CImguiManager::AnimationEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_AnimationEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("AnimationEvents ���� ���� ����");
		return;
	}

	m_AnimationEvents.clear();

	_uint iEventMapSize = { 0 };

	in >> iEventMapSize;

	for (size_t i = 0; i < iEventMapSize; i++)
	{
		string strAnimName = "";
		in >> strAnimName;

		Animation_Event Desc{};
		in >> Desc.iType;
		in >> Desc.fAinmPosition;
		in >> Desc.strChannelName;
		in >> Desc.iBoneIndex;

		m_AnimationEvents.emplace(strAnimName, Desc);
	}

	in.close();
}

void CImguiManager::ColliderState_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_Colliders.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("Colliders ���� ����");
		return;
	}

	m_AddedColliders.clear();

	_uint iColliderCount = { 0 };

	in.read((char*)&iColliderCount, sizeof(_uint));

	for (size_t i = 0; i < iColliderCount; i++)
	{
		_uint iBoneIndex = { 0 };
		_uint iColliderType = { 0 };
		in.read((char*)&iBoneIndex, sizeof(_uint));
		in.read((char*)&iColliderType, sizeof(_uint));

		_float3 vCenter;
		in.read((char*)&vCenter, sizeof(_float3));

		_float3 vExtents;
		_float fRadius;

		void* pValue = nullptr;

		switch (iColliderType)
		{
		case Engine::CCollider::COLLIDER_AABB:
		{
			in.read((char*)&vExtents, sizeof(_float3));

			pValue = &vExtents;
		}
		break;
		case Engine::CCollider::COLLIDER_OBB:
		{
			in.read((char*)&vExtents, sizeof(_float3));
			pValue = &vExtents;
		}
		break;
		case Engine::CCollider::COLLIDER_SPHERE:
		{
			in.read((char*)&fRadius, sizeof(_float));
			pValue = &fRadius;
		}
		break;
		}

		_bool isAlways = { false };
		int iColliderPartType, iColliderActionType;
		iColliderPartType = iColliderActionType = 0;

		in.read((char*)&iColliderActionType, sizeof(int));
		in.read((char*)&iColliderPartType, sizeof(int));
		in.read((char*)&isAlways, sizeof(_bool));

		if (!m_pRenderModel->Created_BoneCollider(iBoneIndex))
		{
			m_AddedColliders.emplace(iBoneIndex, Collider_State{ m_BoneNameList[iBoneIndex], iColliderActionType, iColliderPartType, isAlways });
			m_pRenderModel->Create_BoneCollider(iColliderType, iBoneIndex, vCenter, pValue);
		}

	}

	in.close();
}

void CImguiManager::EffectState_Load(string strPath)
{
	// ������ �����ص� ���� Ŭ����
	Clear_EffectStateMap();

	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_EffectState.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("EffectState ���� ���� ����");
		return;
	}

	_uint iNumEffectState = { 0 };
	in >> iNumEffectState;

	for (size_t i = 0; i < iNumEffectState; i++)
	{
		string strBoneName = "";
		in >> strBoneName;

		string strEffectName = "";
		in >> strEffectName;

		Create_Effect(strBoneName, strEffectName);
	}



	in.close();
}

void CImguiManager::RimEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_RimEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("RimEvents ���� ���� ����");
		return;
	}

	m_RimLightEvents.clear();

	_uint iNumRimEvents{ 0 };
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	in >> iNumRimEvents;

	Animation_RimLightState RimEvent{};
	string key;

	for (size_t i = 0; i < iNumRimEvents; i++)
	{
		in >> key;
		in >> RimEvent.iType;
		in >> RimEvent.strMeshName;
		in >> RimEvent.fAinmPosition;

		m_RimLightEvents.emplace(key, RimEvent);
	}

	in.close();
}

void CImguiManager::TrailEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_TrailEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("TrailEvents ���� ���� ����");
		return;
	}

	m_TrailEvents.clear();

	_uint iNumTrailEvents{ 0 };
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	in >> iNumTrailEvents;

	Animation_TrailState TrailEvent{};
	string key;

	for (size_t i = 0; i < iNumTrailEvents; i++)
	{
		in >> key;
		in >> TrailEvent.iType;
		in >> TrailEvent.strBonelName;
		in >> TrailEvent.iBoneIndex;
		in >> TrailEvent.fAinmPosition;
		
		m_TrailEvents.emplace(key, TrailEvent);

		if (TrailEvent.iType == 0)
		{
			Create_Effect(TrailEvent.strBonelName, m_SelectedEffectName, TEXT("Layer_Trail"));
		}

	}

	in.close();
}

void CImguiManager::Gui_Select_Bone(_uint iBoneIndex)
{
	Reset_Collider_Value();
	Setting_Collider_Value(iBoneIndex);
	m_pRenderModel->Select_Bone(iBoneIndex);
}

void CImguiManager::Setting_AnimationList()
{
	if (nullptr == m_pRenderModel) return;

	switch (m_iModelType)
	{
	case PLAYER:
	{
		m_Anims = m_Anims;
		break;
	}
	case ENEMY:
	{
		auto pAnimCom = m_pRenderModel->Get_AnimComponent();
		m_Anims = pAnimCom->Get_Animations();
		break;
	}
	}
}

void CImguiManager::Setting_RimLight()
{
	auto lower_bound_iter = m_RimLightEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_RimLightEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	// ��ȯ�� iter�� Ű���� �ٸ��ٸ� �� ���� �ش� Ű���� �������� �ʴ´ٴ� ��
	if (lower_bound_iter != m_RimLightEvents.end() && (*lower_bound_iter).first != m_AnimNameList[m_iAnimIndex])
		return;

	auto Anims = m_Anims;
	_float CurrentPosition = (_float)(*(Anims[m_iAnimIndex]->Get_CurrentPosition()));

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		if ((*lower_bound_iter).second.fAinmPosition < CurrentPosition)
		{
			if ((*lower_bound_iter).second.iType == 0)
				m_pRenderModel->Set_RimMeshName((*lower_bound_iter).second.strMeshName);
			else
				m_pRenderModel->Set_RimMeshName("");
		}
	}
}

void CImguiManager::Setting_Trail()
{
	auto lower_bound_iter = m_TrailEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_TrailEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	// ��ȯ�� iter�� Ű���� �ٸ��ٸ� �� ���� �ش� Ű���� �������� �ʴ´ٴ� ��
	if (lower_bound_iter != m_TrailEvents.end() && (*lower_bound_iter).first != m_AnimNameList[m_iAnimIndex])
		return;

	auto Anims = m_Anims;
	_float CurrentPosition = (_float)(*(Anims[m_iAnimIndex]->Get_CurrentPosition()));

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		if ((*lower_bound_iter).second.fAinmPosition < CurrentPosition)
		{
			if ((*lower_bound_iter).second.iType == 0)
				m_pRenderModel->Trail_On((*lower_bound_iter).second.strBonelName, true);
			else
				m_pRenderModel->Trail_On((*lower_bound_iter).second.strBonelName, false);
		}
	}
}

void CImguiManager::Setting_InitialData()
{
	string strDirPath = "../../Client/Bin/Resources/Models/Anim/";

	m_pGameInstance->Get_DirectoryName(strDirPath, m_ModelNameList);
	g_wstrModelName = m_pGameInstance->StringToWstring(m_ModelNameList.front());

	strDirPath = "../../Client/Bin/DataFiles/Particle/";
	m_pGameInstance->Get_DirectoryName(strDirPath, m_EffectTypeList);


	for (size_t i = 0; i < m_EffectTypeList.size(); i++)
	{
		vector<string> tempFileNames;
		m_pGameInstance->Get_FileNames(strDirPath + m_EffectTypeList[i], tempFileNames);

		for (size_t j = 0; j < tempFileNames.size(); j++)
		{
			m_EffectFiles.emplace(i, tempFileNames[j]);
		}
	}
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