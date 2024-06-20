#include "ImguiManager.h"
#include "GameInstance.h"


#pragma region "Imgui"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"
#pragma endregion

#pragma region "객체 원형"
#include "Particle_Point.h"
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
	if (nullptr != pArg)
	{

	}
	m_EffectDesc.eType = 0;
	m_EffectDesc.vStartPos = { 0.f, 0.f, 0.f, 1.f };

	m_EffectDesc.BufferInstance.iNumInstance = 1;
	m_EffectDesc.BufferInstance.isLoop = true;
	m_EffectDesc.BufferInstance.vLifeTime = _float2(1.f, 1.f);
	m_EffectDesc.BufferInstance.vOffsetPos = _float3(0.f, 0.f, 0.f);
	m_EffectDesc.BufferInstance.vPivotPos = _float3(0.f, 0.f, 0.f);
	m_EffectDesc.BufferInstance.vRange = _float3(0.f, 0.f, 0.f);
	m_EffectDesc.BufferInstance.vSize = _float2(1.f, 1.f);
	m_EffectDesc.BufferInstance.vSpeed = _float2(0.f, 0.f);
	m_EffectDesc.BufferInstance.vRectSize = _float2(1.0f, 1.0f);
	m_EffectDesc.BufferInstance.fRadius = 1.f;

	return S_OK;
}



void CImguiManager::Tick(const _float& fTimeDelta)
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	//bool bDemo = true;
	//ImGui::ShowDemoWindow(&bDemo);

	Create_Tick(fTimeDelta);
	Editor_Tick(fTimeDelta);
	Guizmo_Tick(fTimeDelta);


	if (nullptr != m_EditParticle)
	{
		m_EditParticle->Tick(fTimeDelta);
		m_EditParticle->Late_Tick(fTimeDelta);
	}
}



HRESULT CImguiManager::Render()
{
	ImGui::Render();

	return S_OK;
}

void CImguiManager::EditTransform(_float* cameraView, _float* cameraProjection, _float* matrix)
{
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

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
	ImGui::SameLine();

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;

	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
	if (useWindow)
	{
		ImGui::PopStyleColor(1);
	}
}

void CImguiManager::Guizmo(_float fTimeDelta)
{


	if(m_bGuizmo)
	{
		_float* cameraView = (_float*)m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
		_float* cameraProjection = (_float*)m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

		if (nullptr != m_EditParticle)
		{

			_float* objectWorld = (_float*)dynamic_cast<CTransform*>(m_EditParticle->Get_TransformCom())->Get_WorldFloat4x4();
			EditTransform(cameraView, cameraProjection, objectWorld);
			ImGuizmo::IsUsing();
		}
	}

}

HRESULT CImguiManager::Create_Particle(_uint iType)
{
	if (nullptr != m_EditParticle)
	{
		Safe_Release(m_EditParticle);
	}

	CEffect::EFFECT_DESC EffectDesc{};
	EffectDesc.BufferInstance.iNumInstance = m_EffectDesc.BufferInstance.iNumInstance;
	EffectDesc.BufferInstance.isLoop = m_EffectDesc.BufferInstance.isLoop;
	EffectDesc.BufferInstance.vLifeTime = m_EffectDesc.BufferInstance.vLifeTime;
	EffectDesc.BufferInstance.vOffsetPos = m_EffectDesc.BufferInstance.vOffsetPos;
	EffectDesc.BufferInstance.vPivotPos = m_EffectDesc.BufferInstance.vPivotPos;
	EffectDesc.BufferInstance.vRange = m_EffectDesc.BufferInstance.vRange;
	EffectDesc.BufferInstance.vSize = m_EffectDesc.BufferInstance.vSize;
	EffectDesc.BufferInstance.vSpeed = m_EffectDesc.BufferInstance.vSpeed;
	EffectDesc.BufferInstance.vRectSize = m_EffectDesc.BufferInstance.vRectSize;
	EffectDesc.BufferInstance.fRadius = m_EffectDesc.BufferInstance.fRadius;
	EffectDesc.vStartPos = m_EffectDesc.vStartPos;
	EffectDesc.eType = m_EffectDesc.eType;


	m_EditParticle = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Particle_Point"), &EffectDesc);
	if (nullptr == m_EditParticle)
		return E_FAIL;

	if(m_bSpread)
		dynamic_cast<CParticle_Point*>(m_EditParticle)->Edit_Action(CParticle_Point::ACTION_SPREAD);
	if(m_bDrop)
		dynamic_cast<CParticle_Point*>(m_EditParticle)->Edit_Action(CParticle_Point::ACTION_DROP);


	return S_OK;
}

HRESULT CImguiManager::Edit_Particle()
{
	if (nullptr == m_EditParticle)
		return E_FAIL;
	else
		Safe_Release(m_EditParticle);


	CEffect::EFFECT_DESC EffectDesc{};
	EffectDesc.BufferInstance.iNumInstance = m_EffectDesc.BufferInstance.iNumInstance;
	EffectDesc.BufferInstance.isLoop = m_EffectDesc.BufferInstance.isLoop;
	EffectDesc.BufferInstance.vLifeTime = m_EffectDesc.BufferInstance.vLifeTime;
	EffectDesc.BufferInstance.vOffsetPos = m_EffectDesc.BufferInstance.vOffsetPos;
	EffectDesc.BufferInstance.vPivotPos = m_EffectDesc.BufferInstance.vPivotPos;
	EffectDesc.BufferInstance.vRange = m_EffectDesc.BufferInstance.vRange;
	EffectDesc.BufferInstance.vSize = m_EffectDesc.BufferInstance.vSize;
	EffectDesc.BufferInstance.vSpeed = m_EffectDesc.BufferInstance.vSpeed;
	EffectDesc.BufferInstance.vRectSize = m_EffectDesc.BufferInstance.vRectSize;
	EffectDesc.BufferInstance.fRadius = m_EffectDesc.BufferInstance.fRadius;
	EffectDesc.vStartPos = m_EffectDesc.vStartPos;
	EffectDesc.eType = m_EffectDesc.eType;


	m_EditParticle = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Particle_Point"), &EffectDesc);
	if (nullptr == m_EditParticle)
		return E_FAIL;

	if (m_bSpread)
		dynamic_cast<CParticle_Point*>(m_EditParticle)->Edit_Action(CParticle_Point::ACTION_SPREAD);
	if (m_bDrop)
		dynamic_cast<CParticle_Point*>(m_EditParticle)->Edit_Action(CParticle_Point::ACTION_DROP);


	return S_OK;
}

void CImguiManager::Create_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(1030, 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(250, 200), ImGuiCond_None);

	ImGui::Begin("Particle_Create");


	if (ImGui::TreeNode("Particle"))
	{
		const char* Particle[] = { "Point","Rect","Animation" };
		static int Particle_current = 0;
		ImGui::ListBox("Particle List", &Particle_current, Particle, IM_ARRAYSIZE(Particle));

		if (ImGui::Button("Create Particle"))
		{
			Create_Particle(Particle_current);
		}
		if (ImGui::Button("Delete Particle"))
		{
			Safe_Release(m_EditParticle);
		}
		if (ImGui::Button("Save_binary"))
		{
			//바이너리화
		}
		if (ImGui::Button("Load_binary"))
		{
			//바이너리 로드
		}
		ImGui::TreePop();
	}


	ImGui::End();
}

void CImguiManager::Editor_Tick(_float fTimeDelta)
{
	CParticle_Point* pParticle = dynamic_cast<CParticle_Point*>(m_EditParticle);

	ImGui::SetNextWindowPos(ImVec2(1030, 200), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(250, 350), ImGuiCond_None);

	ImGui::Begin("Particle_Edit");

	if (ImGui::Checkbox("Spread", &m_bSpread))
	{
		pParticle->Edit_Action(CParticle_Point::ACTION_SPREAD);
	}
	ImGui::SameLine();

	if (ImGui::Checkbox("Drop", &m_bDrop))
	{
		pParticle->Edit_Action(CParticle_Point::ACTION_DROP);
	}	
	ImGui::SameLine();

	if (ImGui::Checkbox("Loop", &m_EffectDesc.BufferInstance.isLoop))
	{
		Edit_Particle();
	}
	ImGui::NewLine();

	if (ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &m_EffectDesc.BufferInstance.iNumInstance))
	{
		Edit_Particle();
	}

	_float* Temp = (_float*)&m_EffectDesc.BufferInstance.vOffsetPos;
	if (ImGui::InputFloat3("OffsetPos", Temp))
	{
		memcpy(&m_EffectDesc.BufferInstance.vOffsetPos, Temp, sizeof(_float3));
		Edit_Particle();
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vPivotPos;
	if (ImGui::InputFloat3("PivotPos", Temp))
	{
		memcpy(&m_EffectDesc.BufferInstance.vPivotPos, Temp, sizeof(_float3));
		Edit_Particle();
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vRange;
	if (ImGui::InputFloat3("Range", Temp))
	{
		memcpy(&m_EffectDesc.BufferInstance.vRange, Temp, sizeof(_float3));
		Edit_Particle();
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.fRadius;
	if (ImGui::InputFloat("Radius", Temp))
	{
		memcpy(&m_EffectDesc.BufferInstance.fRadius, Temp, sizeof(_float));
		Edit_Particle();	
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vSize;
	if (ImGui::InputFloat2("Size", Temp))
	{
		memcpy(&m_EffectDesc.BufferInstance.vSize, Temp, sizeof(_float2));
		Edit_Particle();
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vRectSize;
	if (ImGui::InputFloat2("RectSize", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_EffectDesc.BufferInstance.vRectSize, Temp, sizeof(_float2));
		Edit_Particle();
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vSpeed;
	if (ImGui::InputFloat2("Speed", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_EffectDesc.BufferInstance.vSpeed, Temp, sizeof(_float2));
		Edit_Particle();
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vLifeTime;
	if (ImGui::InputFloat2("LifeTime", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_EffectDesc.BufferInstance.vLifeTime, Temp, sizeof(_float2));
		Edit_Particle();
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vPower;
	if (ImGui::InputFloat2("Power", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_EffectDesc.BufferInstance.vPower, Temp, sizeof(_float2));
		Edit_Particle();
	}



	ImGui::End();


}

void CImguiManager::Guizmo_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(1030, 550), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(250, 170), ImGuiCond_None);


	ImGui::Begin("Editor");
	
	ImGui::Checkbox("Edit Guizmo", &m_bGuizmo);

	Guizmo(fTimeDelta);

	ImGui::End();
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
	Safe_Release(m_EditParticle);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
