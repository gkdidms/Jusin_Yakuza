#include "ImguiManager.h"
#include "GameInstance.h"
#include "AnimModel.h"
#include "Animation.h"
#include "Bone.h"

#pragma region "Imgui"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"
#pragma endregion


static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
bool useWindow = false;

static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f };

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
	if (!m_isPause)
		m_fTimeDelta = fTimeDelta;
	else
		m_fTimeDelta = 0.f;


	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

	if (ImGui::Button("Model Connecting"))
		Connect_Model_Ref();

	ModelList();

	if (ImGui::Button("Open"))
		m_isAnimListWindow = !m_isAnimListWindow;

	if (m_isAnimListWindow)
	{
		AnimListWindow();
		BoneListWindow();
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

	if (ImGui::Button("Anim Pause"))
		m_isPause = !m_isPause;

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
	m_pRenderModel->Set_Rotation(0, XMConvertToRadians(m_ModelRotation[0]), m_fTimeDelta);
}

void CImguiManager::Update_Model_RotationY()
{
	m_pRenderModel->Set_Rotation(1, XMConvertToRadians(m_ModelRotation[1]), m_fTimeDelta);
}

void CImguiManager::Update_Model_RotationZ()
{
	m_pRenderModel->Set_Rotation(2, XMConvertToRadians(m_ModelRotation[2]), m_fTimeDelta);
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
