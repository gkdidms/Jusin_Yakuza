#include "Trigger_Manager.h"
#include "GameInstance.h"

#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"
#include "SystemManager.h"


#include <iostream>
#include <io.h>

bool useWindowT = false;
int gizmoCountT = 1;
static ImGuizmo::OPERATION mCurrentGizmoOperationT(ImGuizmo::TRANSLATE);


// TRANSFORM의 MATRIX
void EditTransformT(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
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
			mCurrentGizmoOperationT = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGizmoOperationT = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
			mCurrentGizmoOperationT = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperationT == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperationT = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperationT == ImGuizmo::ROTATE))
			mCurrentGizmoOperationT = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperationT == ImGuizmo::SCALE))
			mCurrentGizmoOperationT = ImGuizmo::SCALE;
		/* if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
			 mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;*/
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

		if (mCurrentGizmoOperationT != ImGuizmo::SCALE)
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

		switch (mCurrentGizmoOperationT)
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
	if (useWindowT)
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
	}
	else
	{
		// 이거만 신경쓰기 얼마만큼 기즈모 화면값
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperationT, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	if (useWindowT)
	{
		ImGui::End();
		// 이거해보기 터지면
		ImGui::PopStyleColor(1);
	}
}

IMPLEMENT_SINGLETON(CTrigger_Manager)

CTrigger_Manager::CTrigger_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTrigger_Manager::Initialize()
{
    return S_OK;
}

void CTrigger_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CTrigger_Manager::Tick(_float fTimeDelta)
{
}

void CTrigger_Manager::Late_Tick(_float fTimeDelta)
{
	if (true == m_bRender)
	{
		for (auto& iter : m_TriggerObjects)
			iter->Late_Tick(fTimeDelta);
	}
}

void CTrigger_Manager::Render()
{
}

void CTrigger_Manager::Show_FileName()
{
}

void CTrigger_Manager::Show_Add_Trigger_IMGUI()
{
	ImGui::Text(u8"트리거 종류");
	static int triggerType = TRIGGER_TYPE::TRIGGER_MOVE_LEVEL;
	if (ImGui::RadioButton(u8"Trigger MoveLevel", triggerType == TRIGGER_TYPE::TRIGGER_MOVE_LEVEL))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_MOVE_LEVEL;
	}
	ImGui::NewLine();
	if (ImGui::RadioButton(u8"시네머신", triggerType == TRIGGER_TYPE::TRIGGER_CINEMACHINE))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_CINEMACHINE;
	}
	ImGui::NewLine();
	if (ImGui::RadioButton(u8"요네다", triggerType == TRIGGER_TYPE::TRIGGER_YONEDA))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_YONEDA;
	}

	ImGui::NewLine();

	ImGui::Text(u8"씬 이동 유무");
	static bool bscenemove = false;
	ImGui::Checkbox(u8"체크-씬이동o, 체크x-씬이동x", &bscenemove);


	ImGui::NewLine();

	static int			iNum = 0;
	ImGui::InputInt(u8"씬 번호", &iNum);


	ImGui::Text(u8"트리거 추가 - 추가 버튼 누르고 pick ");

	if (ImGui::Button(u8"트리거 추가"))
	{
		m_bAddTrigger = true;
	}

	Trigger_IO		triggerAddDesc;
	triggerAddDesc.iTriggerType = triggerType;
	triggerAddDesc.bMoveScene = bscenemove;
	triggerAddDesc.iSceneNum = iNum;

	/* 왼쪽 클릭할 시 트리거 추가 */
	if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB) && true == m_bAddTrigger)
	{
		_matrix			startPos;
		startPos = XMMatrixIdentity();


		CTriggerObj::TRIGGEROBJ_DESC		triggerObjDesc;
		triggerObjDesc.vStartPos = startPos;
		triggerObjDesc.tTriggerDesc = triggerAddDesc;

		m_TriggerObjects.push_back(dynamic_cast<CTriggerObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_TriggerObj"), &triggerObjDesc)));

		m_bAddTrigger = false;

		/* 추가되면 추가된 index로 이동 */
		m_iCurrentLightIndex = m_TriggerObjects.size() - 1;

		Update_TriggerObjName();

	}
}

void CTrigger_Manager::Update_TriggerObjName()
{
}

void CTrigger_Manager::Edit_Trigger()
{
}

void CTrigger_Manager::Edit_GameObject_Transform(int iNumObject)
{
}

void CTrigger_Manager::Edit_Installed_Trigger()
{
}

void CTrigger_Manager::Click_To_Select_Trigger(int& iObjectNum)
{
}

void CTrigger_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}
