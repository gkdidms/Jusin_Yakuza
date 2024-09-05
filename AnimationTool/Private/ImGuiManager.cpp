#include "ImguiManager.h"
#include "GameInstance.h"

#include "AnimModel.h"
#include "ModelBoneSphere.h"
#include "TRailEffect.h"

#include "ImGuiFileDialog.h"

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
	Setting_InitialData("Player/");

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

	fs::path projectRootDir = fs::current_path();
	fs::path ParentDir = projectRootDir.parent_path();
	m_RootDir = ParentDir.parent_path();
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
		if (m_isFaceWindow)
			FaceEventWindow();
		if (m_isBloodWindow)
			BloodEventWindow();
		if (m_isRadialEventWindow)
			RadialEventWindow();
	}

	Play_SoundEvent();

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
	if (ImGui::RadioButton(u8"�÷��̾� ��", m_iModelPathType == PLAYER))
	{
		m_iModelPathType = PLAYER;
		Setting_ModelList("Player/");
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"���� ��", m_iModelPathType == ENEMY))
	{
		m_iModelPathType = ENEMY;
		Setting_ModelList("Monster/");
	}	
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"NPC ��", m_iModelPathType == NPC))
	{
		m_iModelPathType = NPC;
		Setting_ModelList("NPC/");
	}


	ImGui::Text(u8"�� üũ �� AnimComponent�� �ִϸ��̼� ������� �����Ѵ�");
	if (ImGui::RadioButton(u8"�� �� �ִϸ��̼�", m_iModelType == MODEL_ANIMS))
	{
		m_iModelType = MODEL_ANIMS;

		Change_Model();

		m_Anims = m_pRenderModel->Get_Animations();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"�ִ� ��", m_iModelType == ANIM_COM))
	{
		m_iModelType = ANIM_COM;

		Change_Model();

		auto pAnimCom = m_pRenderModel->Get_AnimComponent();
		m_Anims = pAnimCom->Get_Animations();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"��ũ�׼� �ִ���", m_iModelType == SYNC_COM))
	{
		m_iModelType = SYNC_COM;

		Change_Model();

		auto pAnimCom = m_pRenderModel->Get_AnimComponent();
		m_Anims = pAnimCom->Get_Animations();
	}

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
		Change_Model();
	}

	//if (ImGui::Button(u8"������ �����ϱ�"))
	//{
	//	All_Save();
	//}
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
		m_pRenderModel->Change_Animation(m_iAnimIndex, m_iModelType == PLAYER ? false : true);
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
	ImGui::Text(u8"���� �̺�Ʈ��");
	if (ImGui::Button("RimLight Op"))
	{
		m_isRimLightWindow = true;
	}
#pragma endregion

#pragma region Ʈ���� �̺�Ʈ ��ư
	ImGui::SameLine();
	if (ImGui::Button("Trail Op"))
	{
		m_isTrailWindow = true;
	}
#pragma endregion

#pragma region ���̽� �̺�Ʈ ��ư
	if (ImGui::Button("Face Op"))
	{
		m_isFaceWindow = true;
	}
#pragma endregion

#pragma region ���� ����Ʈ �̺�Ʈ ��ư
	ImGui::SameLine();
	if (ImGui::Button("Blood Op"))
	{
		m_isBloodWindow = true;
	}
#pragma endregion


#pragma region ����� ����Ʈ �̺�Ʈ ��ư
	ImGui::SameLine();
	if (ImGui::Button("Radiul Op"))
	{
		m_isRadialEventWindow = true;
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
		auto iter = m_EffectState.begin();
		for (size_t i = 0; i < m_iAddedEffectSelectedIndex; i++)
		{
			iter++;
		}

		auto pEffectList = m_pGameInstance->Get_GameObjects(LEVEL_EDIT, TEXT("Layer_Effect"));
		
		for (auto pEffect : pEffectList)
		{
			//pEffect == (*iter).first
		}

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
		auto iterator = m_RimLightEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

		for (size_t i = 0; i < m_iTrailEventIndex; i++)
		{
			iterator++;
		}
		m_RimLightEvents.erase(iterator);
	}

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
		TrailState.strTrailProtoName = m_SelectedEffectName;

		auto& Channels = m_Anims[m_iAnimIndex]->Get_Channels();
		TrailState.iBoneIndex = Channels[m_iChannelSelectedIndex]->Get_BoneIndex();

		m_TrailEvents.emplace(m_AnimNameList[m_iAnimIndex], TrailState);

		Create_Effect(m_BoneNameList[m_iBoneSelectedIndex], m_SelectedEffectName, TEXT("Layer_Effect"));
	}

	if (ImGui::Button(u8"Ʈ���� Off"))
	{
		Animation_TrailState TrailState{};
		TrailState.iType = 1;
		TrailState.fAinmPosition = m_fAnimationPosition;
		TrailState.strBonelName = m_ChannelNameList[m_iChannelSelectedIndex];
		TrailState.strTrailProtoName = m_SelectedEffectName;

		auto& Channels = m_Anims[m_iAnimIndex]->Get_Channels();
		TrailState.iBoneIndex = Channels[m_iChannelSelectedIndex]->Get_BoneIndex();

		m_TrailEvents.emplace(m_AnimNameList[m_iAnimIndex], TrailState);
	}

	//������Ʈ �̺�Ʈ ����Ʈ
	auto lower_bound_iter = m_TrailEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	// ��ȯ�� iter�� Ű���� �ٸ��ٸ� �� ���� �ش� Ű���� �������� �ʴ´ٴ� ��
	if (!(lower_bound_iter != m_TrailEvents.end() && (*lower_bound_iter).first != m_AnimNameList[m_iAnimIndex]))
	{
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
			if(lower_iter != m_TrailEvents.end())
				lower_iter++;
			else
			{
				MSG_BOX("Ʈ���� �̺�Ʈ ��Ͽ��� iterator�� end�Դϴ�. �ٽ� �������ּ���.");
				return;
			}
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
			auto iterator = m_TrailEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

			for (size_t i = 0; i < m_iTrailEventIndex; i++)
			{
				iterator++;
			}
			m_TrailEvents.erase(iterator);
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"����"))
		{
			string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
			TrailEvent_Save(strDirectory);
		}
	}

	ImGui::End();
}

void CImguiManager::SoundListWindow()
{
	ImGui::Begin("Sound List", &m_isSoundListWindow);

	if (ImGui::Button(u8"���� ���� �ҷ�����"))
	{
		IGFD::FileDialogConfig config;
		config.path = (m_RootDir / "Client" / "Bin" / "Resources" / "Sounds" ).string();
		ImGuiFileDialog::Instance()->OpenDialog("ChooseBinaryGroupSoundFiles", "Choose File", ".wav, .mp3", config);
	}

	if (ImGuiFileDialog::Instance()->Display("ChooseBinaryGroupSoundFiles"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			m_strSelectSoundFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

			ImGuiFileDialog::Instance()->Close();
		}
	}

	if (ImGui::Button(u8"���� ���"))
		m_pGameInstance->PlaySound_W(m_pGameInstance->StringToWstring(ImGuiFileDialog::Instance()->GetCurrentFileName()), m_eSoundCannel, m_fSoundVolume);
	ImGui::SameLine();
	if (ImGui::Button(u8"���� ����"))
		m_pGameInstance->StopAll();

	ImGui::Checkbox(u8"�ƽſ�", &m_isCutSceneEvents);

	ImGui::InputFloat(u8"���� ����", &m_fSoundVolume);

	/*
	* 		Animation_SoundEventState Desc{};
			Desc.eChannel = m_eSoundCannel;
			Desc.fAinmPosition = m_fAnimationPosition;
			Desc.eChannel = m_eSoundCannel;


			m_SoundEvents.emplace()
	*/
	ImGui::Text(u8"���õ� �ִϸ��̼�: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::InputFloat(u8"�ִϸ��̼� ������", &m_fAnimationPosition);
	ImGui::Text(u8"���õ� ���� ����: %s", m_strSelectSoundFileName.c_str());

	/*
	* 	enum CHANNELID {
		SOUND_EFFECT,
		SOUND_BGM,
		SOUND_AMBIENT,
		SOUND_PLAYER_VOICE,
		SOUND_PLAYER_EFFECT,
		SOUND_ENEMY_VOICE,
		SOUND_ENEMY_EFFECT,
		SOUND_UI,
		SOUND_END
	};
	*/

	ImGui::Text(u8"���� ä�� ����");
	if (ImGui::RadioButton(u8"EFFECT", m_eSoundCannel == SOUND_EFFECT))
	{
		m_eSoundCannel = SOUND_EFFECT;
	}
	if (ImGui::RadioButton(u8"BGM", m_eSoundCannel == SOUND_BGM))
	{
		m_eSoundCannel = SOUND_BGM;
	}
	if (ImGui::RadioButton(u8"AMBIENT", m_eSoundCannel == SOUND_AMBIENT))
	{
		m_eSoundCannel = SOUND_AMBIENT;
	}
	if (ImGui::RadioButton(u8"PLAYER_VOICE", m_eSoundCannel == SOUND_PLAYER_VOICE))
	{
		m_eSoundCannel = SOUND_PLAYER_VOICE;
	}
	if (ImGui::RadioButton(u8"PLAYER_EFFECT", m_eSoundCannel == SOUND_PLAYER_EFFECT))
	{
		m_eSoundCannel = SOUND_PLAYER_EFFECT;
	}
	if (ImGui::RadioButton(u8"ENEMY_VOICE", m_eSoundCannel == SOUND_ENEMY_VOICE))
	{ 
		m_eSoundCannel = SOUND_ENEMY_VOICE;
	}
	if (ImGui::RadioButton(u8"ENEMY_EFFECT", m_eSoundCannel == SOUND_ENEMY_EFFECT))
	{
		m_eSoundCannel = SOUND_ENEMY_EFFECT;
	}

	if (ImGui::Button(u8"���� �߰��ϱ�"))
	{
		Animation_SoundEventState Desc{};

		Desc.iChannel = static_cast<_uint>(m_eSoundCannel);
		Desc.fAinmPosition = m_fAnimationPosition;
		Desc.fSoundVolume = m_fSoundVolume;
		Desc.strSoundFileName = m_strSelectSoundFileName;

		m_SoundEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}

	ImGui::Text(u8"���� �̺�Ʈ ����Ʈ");

	auto upper_bound_iter = m_SoundEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);
	auto lower_bound_iter = m_SoundEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	vector<const char*> items;
	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		items.push_back((*lower_bound_iter).second.strSoundFileName.c_str());
	}

	if (ImGui::ListBox("##", &m_iSoundEventIndex, items.data(), items.size()))
	{
		auto lower_bound_iter2 = m_SoundEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iSoundEventIndex; i++)
		{
			lower_bound_iter2++;
		}

		m_strSelectSoundFileName = lower_bound_iter2->second.strSoundFileName;
		m_fAnimationPosition = lower_bound_iter2->second.fAinmPosition;
		m_fSoundVolume = lower_bound_iter2->second.fSoundVolume;
	}

	if (ImGui::Button(u8"�ҷ�����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		SoundEvent_Load(strDirectory);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"����"))
	{
		auto iterator = m_SoundEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iSoundEventIndex; i++)
		{
			iterator++;
		}
		m_SoundEvents.erase(iterator);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		SoundEvent_Save(strDirectory);
	}

	ImGui::End();
}

void CImguiManager::FaceEventWindow()
{
	ImGui::Begin("Face Event", &m_isFaceWindow);

	ImGui::Text(u8"[0] f201_special_01 �Ѱ������� ���� �����ϴ� �ƽſ��� ����");
	ImGui::Text(u8"[1] f202_special_02 �λ�(ȭ��)");
	ImGui::Text(u8"[2] f203_special_03 �λ�(�������� �̷�ǥ��) Ȳ�����ϴµ�");
	ImGui::Text(u8"[3] f204_special_04 �λ󾲰��ִµ� �����ϰ� ����");
	ImGui::Text(u8"[4] f205_special_05 �����𸣰ڴ� ������ ǥ��");
	ImGui::Text(u8"[5] f206_special_06 �̽�½�� ����");
	ImGui::Text(u8"[6] f207_special_07 ������ �� ���ϵ�?");
	ImGui::Text(u8"[7] f208_special_08 ���� �Ⱦ����Ժ��°Ű��ƺ���");
	ImGui::Text(u8"[8] f209_special_09 ũ�� ȭ�� ��");
	ImGui::Text(u8"[9] f210_special_10 �� �θ��ߴ�");
	ImGui::Text(u8"[10] f211_special_11 �λ󾲰� �Ǵ��߰�");
	ImGui::Text(u8"[11] f212_special_12 �� ũ�Զ� (�����)\n");
		

	ImGui::Text(u8"���õ� �ִϸ��̼�: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::InputFloat(u8"�ִϸ��̼� ������", &m_fAnimationPosition);

	ImGui::InputInt(u8"ǥ�� �ε���", &m_iFaceAnimIndex);

	if (ImGui::Button(u8"ǥ�� On"))
	{
		Animation_FaceEventState Desc{};
		Desc.iType = 0;
		Desc.fAinmPosition = m_fAnimationPosition;
		Desc.iFaceAnimIndex = m_iFaceAnimIndex;

		m_FaceEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}
	if (ImGui::Button(u8"ǥ�� Off"))
	{
		Animation_FaceEventState Desc{};
		Desc.iType = 1;
		Desc.fAinmPosition = m_fAnimationPosition;
		Desc.iFaceAnimIndex = m_iFaceAnimIndex;

		m_FaceEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}
	if (ImGui::Button(u8"ǥ�� Change"))
	{
		Animation_FaceEventState Desc{};
		Desc.iType = 2;
		Desc.fAinmPosition = m_fAnimationPosition;
		Desc.iFaceAnimIndex = m_iFaceAnimIndex;

		m_FaceEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}

	ImGui::Text(u8"ǥ�� �̺�Ʈ ����Ʈ");

	auto upper_bound_iter = m_FaceEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);
	auto lower_bound_iter = m_FaceEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	vector<const char*> items;
	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		items.push_back((*lower_bound_iter).second.iType == 0 ? "On" : ((*lower_bound_iter).second.iType == 1 ? "Off" : "Change"));
	}

	if (ImGui::ListBox("##", &m_iFaceEventIndex, items.data(), items.size()))
	{
		auto lower_bound_iter2 = m_FaceEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iFaceEventIndex; i++)
		{
			lower_bound_iter2++;
		}

		m_iFaceAnimIndex = lower_bound_iter2->second.iFaceAnimIndex;
		m_fAnimationPosition = lower_bound_iter2->second.fAinmPosition;
	}

	if (ImGui::Button(u8"�ҷ�����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		FaceEvent_Load(strDirectory);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"����"))
	{
		auto iterator = m_FaceEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iFaceEventIndex; i++)
		{
			iterator++;
		}
		m_FaceEvents.erase(iterator);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		FaceEvent_Save(strDirectory);
	}

	ImGui::End();
}

void CImguiManager::BloodEventWindow()
{
	ImGui::Begin("Blood Particle Event", &m_isBloodWindow);

	ImGui::Text(u8"���õ� �ִϸ��̼�: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::Text(u8"���õ� ��(�� ����Ʈ���� ������ ��): %s", m_BoneNameList[m_iBoneSelectedIndex].c_str());

	ImGui::InputFloat(u8"�ִϸ��̼� ������", &m_fAnimationPosition);

	ImGui::Text(u8"�Ľ��� �� ���ص� Ÿ���� int������ �Է��Ѵ�.");
	ImGui::Text(u8"0: ��, 1: ��, 2: ...");
	ImGui::InputInt(u8"����� ����Ʈ Ÿ��: ", &m_eBloodEffectType);

	if(m_isBloodEffectOn)
		ImGui::Text(u8"�����Ѱ� On �̺�Ʈ");
	else
		ImGui::Text(u8"�����Ѱ� Off �̺�Ʈ");

	if (ImGui::RadioButton(u8"���� O", m_isBloodEffectLoop))
	{
		m_isBloodEffectLoop = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"���� X", !m_isBloodEffectLoop))
	{
		m_isBloodEffectLoop = false;
	}

	if (ImGui::Button(u8"��"))
	{
		m_iBoneSelectedIndex = 99;
		Gui_Select_Bone(99);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"��"))
	{
		m_iBoneSelectedIndex = 89;
		Gui_Select_Bone(89);
	}

	if (ImGui::Button(u8"�� ����Ʈ On"))
	{
		Animation_BloodEventState Desc{};
		Desc.fAinmPosition = m_fAnimationPosition;
		Desc.iBoneIndex = m_iBoneSelectedIndex;
		Desc.strBonelName = m_BoneNameList[m_iBoneSelectedIndex];
		Desc.iBloodEffectType = m_eBloodEffectType;
		Desc.isLoop = m_isBloodEffectLoop;
		Desc.isOn = true;
		m_isBloodEffectOn = true;

		m_BloodEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}
	if (m_isBloodEffectLoop)
	{
		if (ImGui::Button(u8"�� ����Ʈ Off"))
		{
			Animation_BloodEventState Desc{};
			Desc.fAinmPosition = m_fAnimationPosition;
			Desc.iBoneIndex = m_iBoneSelectedIndex;
			Desc.strBonelName = m_BoneNameList[m_iBoneSelectedIndex];
			Desc.iBloodEffectType = m_eBloodEffectType;
			Desc.isLoop = m_isBloodEffectLoop;
			Desc.isOn = false;
			m_isBloodEffectOn = false;

			m_BloodEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
		}
	}

	auto upper_bound_iter = m_BloodEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);
	auto lower_bound_iter = m_BloodEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	ImGui::Text(u8"�� ����Ʈ �̺�Ʈ ����Ʈ");
	vector<string> itemStrings; // std::string�� ����Ͽ� ���ڿ� ����
	vector<const char*> items; // const char* ����Ʈ

	itemStrings.reserve(distance(lower_bound_iter, upper_bound_iter));
	items.reserve(distance(lower_bound_iter, upper_bound_iter));

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		itemStrings.push_back(to_string((*lower_bound_iter).second.iBloodEffectType)); // ������ ���ڿ��� ��ȯ�Ͽ� ����
		items.push_back(itemStrings.back().c_str()); // ��ȯ�� ���ڿ��� ����Ʈ�� �߰�
	}

	if (ImGui::ListBox(u8"##", &m_iBloodEventIndex, items.data(), items.size()))
	{
		auto lower_bound_iter2 = m_BloodEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iBloodEventIndex; i++)
		{
			lower_bound_iter2++;
		}

		m_fAnimationPosition = lower_bound_iter2->second.fAinmPosition;
		Gui_Select_Bone(lower_bound_iter2->second.iBoneIndex);
		m_isBloodEffectLoop = lower_bound_iter2->second.isLoop;
		m_eBloodEffectType = lower_bound_iter2->second.iBloodEffectType;
		m_isBloodEffectOn = lower_bound_iter2->second.isOn;
	}

	if (ImGui::Button(u8"�ҷ�����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		BloodEvent_Load(strDirectory);
	}
	if (ImGui::Button(u8"����"))
	{
		auto iterator = m_BloodEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iBloodEventIndex; i++)
		{
			iterator++;
		}
		m_BloodEvents.erase(iterator);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		BloodEvent_Save(strDirectory);
	}


	ImGui::End();
}

void CImguiManager::RadialEventWindow()
{
	ImGui::Begin("Radial Event", &m_isRadialEventWindow);

	ImGui::Text(u8"���õ� �ִϸ��̼�: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::Text(u8"���� �Էµ� �ִϸ��̼� ������: %f", m_fAnimationPosition);

	auto upper_bound_iter = m_RadialEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);
	auto lower_bound_iter = m_RadialEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	ImGui::Text(u8"������ �̺�Ʈ ����Ʈ");
	vector<const char*> items;
	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		items.push_back((*lower_bound_iter).second.iType == 0 ? "On" : "Off");
	}

	if (ImGui::ListBox("##", &m_iRadialEventIndex, items.data(), items.size()))
	{
		auto lower_bound_iter2 = m_RadialEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iRadialEventIndex; i++)
		{
			lower_bound_iter2++;
		}

		m_fRadialForce = lower_bound_iter2->second.fForce;
		m_fAnimationPosition = lower_bound_iter2->second.fAinmPosition;
	}

	ImGui::InputFloat(u8"������ ����", &m_fRadialForce);
	ImGui::InputFloat(u8"�ִϸ��̼� ������", &m_fAnimationPosition);

	if (ImGui::Button(u8"����� On"))
	{
		Animation_RadialEventState Desc{};
		Desc.iType = 0;
		Desc.fForce = m_fRadialForce;
		Desc.fAinmPosition = m_fAnimationPosition;

		m_RadialEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}
	if (ImGui::Button(u8"����� Off"))
	{
		Animation_RadialEventState Desc{};
		Desc.iType = 1;
		Desc.fForce = 0.f;
		Desc.fAinmPosition = m_fAnimationPosition;

		m_RadialEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}

	if (ImGui::Button(u8"�ҷ�����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		RadialEvent_Load(strDirectory);
	}
	if (ImGui::Button(u8"����"))
	{
		auto iterator = m_RadialEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iRadialEventIndex; i++)
		{
			iterator++;
		}
		m_RadialEvents.erase(iterator);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"����"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		RadialEvent_Save(strDirectory);
	}

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

void CImguiManager::Play_SoundEvent()
{
	if (m_Anims.size() < 1) return;
	if (m_AnimNameList.size() < 1) return;

	auto Anims = m_Anims;
	_float Duration = (_float)(*(Anims[m_iAnimIndex]->Get_Duration()));
	_float CurrentPosition = (_float)(*(Anims[m_iAnimIndex]->Get_CurrentPosition()));

	auto lower_bound_iter = m_SoundEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_SoundEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		if (!lower_bound_iter->second.isPlay && CurrentPosition > lower_bound_iter->second.fAinmPosition)
		{
			lower_bound_iter->second.isPlay = true;
			m_pGameInstance->PlaySound_W(m_pGameInstance->StringToWstring(lower_bound_iter->second.strSoundFileName), static_cast<CHANNELID>(lower_bound_iter->second.iChannel), lower_bound_iter->second.fSoundVolume);
		}
	}

	if (Anims[m_iAnimIndex]->Get_Restrat())
		Reset_SoundEvent_Played();
}

void CImguiManager::Reset_SoundEvent_Played()
{
	auto lower_bound_iter = m_SoundEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_SoundEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		lower_bound_iter->second.isPlay = false;
	}
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

	/* ����� �̺�Ʈ �����ϱ� */
	RadialEvent_Save(strDirectory);
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

	_uint iMapSize = m_AddedAnims.size();

	// ���� �ڵ� ���� �ʿ�
	for (const auto& iter : m_AddedAnims)
	{
		_uint iAnimIndex = iter.first;

		auto Anims = m_pRenderModel->Get_Animations();


		// �̸��� ���� �ִϸ��̼��� ã�Ƽ� key������ �������ش�.
		_uint i = 0;
		for (auto& pAnim : Anims)
		{
			string animationName = m_pGameInstance->Extract_String(pAnim->Get_AnimName(), '[', ']');

			if (animationName == iter.second)
			{
				iAnimIndex = i;
				break;
			}

			i++;
		}

		// ���� �ִϸ��̼ǿ� ���°� �����ϴ� �ڵ� (�׸�ŭ ��������)
		if (i >= Anims.size())
			iMapSize--;
	}

	// iMapSize�� ���� �������� ����
	out.write(reinterpret_cast<const char*>(&iMapSize), sizeof(iMapSize));

	for (const auto& iter : m_AddedAnims)
	{
		// Ű�� ���� �������� ����
		_uint iAnimIndex = iter.first;

		auto Anims = m_pRenderModel->Get_Animations();


		// �̸��� ���� �ִϸ��̼��� ã�Ƽ� key������ �������ش�.
		_uint i = 0;
		for (auto& pAnim : Anims)
		{
			string animationName = m_pGameInstance->Extract_String(pAnim->Get_AnimName(), '[', ']');

			if (animationName == iter.second)
			{
				iAnimIndex = i;
				break;
			}

			i++;
		}

		if (i >= Anims.size()) continue;
		

		out.write(reinterpret_cast<const char*>(&iAnimIndex), sizeof(iAnimIndex));

		// ���ڿ� ���̿� ���ڿ��� ���� �������� ����
		const string& strAnimName = iter.second;
		_uint length = strAnimName.size();
		out.write(reinterpret_cast<const char*>(&length), sizeof(length));
		out.write(strAnimName.c_str(), length);
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
		out << pair.second.strTrailProtoName << endl;
		out << pair.second.iBoneIndex << endl;
		out << pair.second.fAinmPosition << endl;
	}

	out.close();
}

void CImguiManager::SoundEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + (m_isCutSceneEvents ? "_CutSoundEvents.dat" : "_SoundEvents.dat");

	ofstream out(strDirectory, ios::binary);

	_uint iNumEvents = m_SoundEvents.size();
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	out << iNumEvents;

	/*
		_uint iChannel;
		_float fAinmPosition;
		string strSoundFileName;
	*/

	for (auto& pair : m_SoundEvents)
	{
		out << pair.first << endl;
		out << pair.second.iChannel << endl;
		out << pair.second.fAinmPosition << endl;
		out << pair.second.fSoundVolume << endl;
		out << pair.second.strSoundFileName << endl;
	}

	out.close();
}

void CImguiManager::FaceEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_FaceEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumEvents = m_FaceEvents.size();
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	out << iNumEvents;

	/*
		_uint iType;					//0���� on 1���� off 2���� change
		_float fAinmPosition;
		_uint iFaceAnimIndex;
	*/

	for (auto& pair : m_FaceEvents)
	{
		out << pair.first << endl;
		out << pair.second.iType << endl;
		out << pair.second.fAinmPosition << endl;
		out << pair.second.iFaceAnimIndex << endl;
	}

	out.close();
}

void CImguiManager::BloodEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_BloodEffectEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumEvents = m_BloodEvents.size();
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	out << iNumEvents;

	/*
		_float fAinmPosition;
		_uint iBoneIndex;
		string strBonelName;
		_uint iBloodEffectType;		// ��½�ų ����Ʈ�� Ÿ��
		_bool isLoop;		// ��½�ų ����Ʈ�� Ÿ��
	*/

	for (auto& pair : m_BloodEvents)
	{
		out << pair.first << endl;
		out << pair.second.fAinmPosition << endl;
		out << pair.second.iBoneIndex << endl;
		out << pair.second.strBonelName << endl;
		out << pair.second.iBloodEffectType << endl;
		out << pair.second.isLoop << endl;
		out << pair.second.isOn << endl;
	}

	out.close();
}

void CImguiManager::RadialEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_RadialEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumEvents = m_RadialEvents.size();
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	out << iNumEvents;

	for (auto& pair : m_RadialEvents)
	{
		out << pair.first << endl;
		out << pair.second.iType << endl;
		out << pair.second.fAinmPosition << endl;
		out << pair.second.fForce << endl;
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
	RadialEvent_Load(strDirectory);
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

	m_AddedAnims.clear();

	m_AnimNameList.clear();
	m_AnimNameList.resize(m_Anims.size());

	_uint iAnimMapSize;
	in.read(reinterpret_cast<char*>(&iAnimMapSize), sizeof(iAnimMapSize));
	
	for (size_t i = 0; i < iAnimMapSize; ++i)
	{
		// Ű�� ���� �������� �о��
		_uint iAnimIndex = 0;
		in.read(reinterpret_cast<char*>(&iAnimIndex), sizeof(iAnimIndex));

		// ���ڿ� ���̿� ���ڿ��� ���� �������� �о��
		_uint length = 0;
		in.read(reinterpret_cast<char*>(&length), sizeof(length));

		string strAnimName(length, ' ');
		in.read(&strAnimName[0], length);

		m_AddedAnims.emplace(iAnimIndex, strAnimName);
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
		in >> TrailEvent.strTrailProtoName;
		in >> TrailEvent.iBoneIndex;
		in >> TrailEvent.fAinmPosition;
		
		m_TrailEvents.emplace(key, TrailEvent);

		if (TrailEvent.iType == 0)
		{
			if(m_EffectState.find(TrailEvent.strBonelName) == m_EffectState.end())
				Create_Effect(TrailEvent.strBonelName, m_SelectedEffectName, TEXT("Layer_Effect"));
		}

	}

	in.close();
}

void CImguiManager::SoundEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + (m_isCutSceneEvents ? "_CutSoundEvents.dat" : "_SoundEvents.dat");

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("SoundEvents ���� ���� ����");
		return;
	}

	m_SoundEvents.clear();

	_uint iNumEvents{ 0 };
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	in >> iNumEvents;

	Animation_SoundEventState Event{};
	string key;

	/*
		CHANNELID eChannel;
		_float fAinmPosition;
		string strSoundFileName;
	*/
	for (size_t i = 0; i < iNumEvents; i++)
	{
		in >> key;
		in >> Event.iChannel;
		in >> Event.fAinmPosition;
		in >> Event.fSoundVolume;
		in >> Event.strSoundFileName;

		m_SoundEvents.emplace(key, Event);
	}

	in.close();
}

void CImguiManager::FaceEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_FaceEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("FaceEvents ���� ���� ����");
		return;
	}

	m_FaceEvents.clear();

	_uint iNumEvents{ 0 };
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	in >> iNumEvents;

	Animation_FaceEventState Event{};
	string key;

	/*
		_uint iType;					//0���� on 1���� off 2���� change
		_float fAinmPosition;
		_uint iFaceAnimIndex;
	*/
	for (size_t i = 0; i < iNumEvents; i++)
	{
		in >> key;
		in >> Event.iType;
		in >> Event.fAinmPosition;
		in >> Event.iFaceAnimIndex;

		m_FaceEvents.emplace(key, Event);
	}

	in.close();
}

void CImguiManager::BloodEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_BloodEffectEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("BloodEffectEvents ���� ���� ����");
		return;
	}

	m_BloodEvents.clear();

	_uint iNumEvents{ 0 };
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	in >> iNumEvents;

	Animation_BloodEventState Event{};
	string key;

	/*
		_float fAinmPosition;
		_uint iBoneIndex;
		string strBonelName;
		_uint iBloodEffectType;		// ��½�ų ����Ʈ�� Ÿ��
		_bool isLoop;				// ����Ʈ ��������
	*/
	for (size_t i = 0; i < iNumEvents; i++)
	{
		in >> key;
		in >> Event.fAinmPosition;
		in >> Event.iBoneIndex;
		in >> Event.strBonelName;
		in >> Event.iBloodEffectType;
		in >> Event.isLoop;
		in >> Event.isOn;

		m_BloodEvents.emplace(key, Event);
	}

	in.close();
}

void CImguiManager::RadialEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_RadialEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("RadialEvents ���� ���� ����");
		return;
	}

	m_RadialEvents.clear();

	_uint iNumEvents{ 0 };
	// �� ��� ����Ʈ�� �о�� ������ �ۼ��Ѵ�
	in >> iNumEvents;

	Animation_RadialEventState Event{};
	string key;

	for (size_t i = 0; i < iNumEvents; i++)
	{
		in >> key;
		in >> Event.iType;
		in >> Event.fAinmPosition;
		in >> Event.fForce;

		m_RadialEvents.emplace(key, Event);
	}

	in.close();
}

void CImguiManager::Gui_Select_Bone(_uint iBoneIndex)
{
	Reset_Collider_Value();
	Setting_Collider_Value(iBoneIndex);
	m_pRenderModel->Select_Bone(iBoneIndex);
	m_iBoneSelectedIndex = iBoneIndex;
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

void CImguiManager::Change_Model()
{
	m_iAnimIndex = 0;
	m_iSearchAnimIndex = 0;
	m_iAddedAnimSelectedIndex = 0;
	m_iBoneSelectedIndex = 0;
	m_iColliderSelectedIndex = 0;
	m_iEventBoneIndex = 0;
	m_iChannelSelectedIndex = 0;
	m_iEventSelectedIndex = 0;
	m_iMeshSelectedIndex = 0;
	m_iAddedMeshSelectedIndex = 0;

	wstring strModelName = m_pGameInstance->StringToWstring(m_ModelNameList[m_iModelSelectedIndex]);
	wstring strAnimName = TEXT("");

	if (TEXT("Kiryu") == strModelName)
	{
		if (SYNC_COM == m_iModelType)
		{
			strAnimName = TEXT("CutSceneAnim_ForPlayer");
		}
		else
		{
			strAnimName = strModelName;
		}

	}
	else if (TEXT("Jimu") == strModelName || TEXT("Kuze") == strModelName || TEXT("Kuze_Fight") == strModelName
		|| TEXT("YakuzaA") == strModelName || TEXT("YakuzaB") == strModelName || TEXT("YakuzaC") == strModelName || TEXT("YakuzaD") == strModelName
		|| TEXT("Yoneda") == strModelName)
	{
		if (SYNC_COM == m_iModelType)
		{
			strAnimName = TEXT("SyncAnim");
		}
		else
		{

			strAnimName = TEXT("Common");
		}
	}
	else if (TEXT("Bike") == strModelName || TEXT("Heli") == strModelName || TEXT("Van") == strModelName || TEXT("Sedan") == strModelName)
	{
		strAnimName = TEXT("ReactorHighwayAnim");
	}

	m_pRenderModel->Change_Model(strModelName, strAnimName);
}

void CImguiManager::Setting_InitialData(string strFolderType)
{
	Setting_ModelList(strFolderType);

	string strDirPath = "../../Client/Bin/DataFiles/Particle/";
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

void CImguiManager::Setting_ModelList(string strFolderType)
{
	m_ModelNameList.clear();

	string strDirPath = "../../Client/Bin/Resources/Models/Anim/" + strFolderType;

	m_pGameInstance->Get_DirectoryName(strDirPath, m_ModelNameList);
	g_wstrModelName = m_pGameInstance->StringToWstring(m_ModelNameList.front());
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