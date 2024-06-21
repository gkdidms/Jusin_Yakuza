#include "../Public/Imgui_Manager.h"
#include "GameInstance.h"
#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include <Windows.h>
#include <string.h>
#include "ObjPlace_Manager.h"
#include "Terrain_Manager.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"



IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
    :m_pNavigationMgr{ CNavigation_Manager::GetInstance() }
    , m_pObjPlace_Manager { CObjPlace_Manager::GetInstance() }
    , m_pGameInstance { CGameInstance::GetInstance()}
    , m_pLightTool_Mgr { CLightTool_Mgr::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    // Win32 �ʱ�ȭ ȣ��
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\fonts\\malgun.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());

    /* IMGUI �ʱ�ȭ */
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    /* Navigation �ʱ�ȭ */
    m_pNavigationMgr->Initialize(m_pDevice, m_pContext);
    m_pObjPlace_Manager->Initialize();

    return S_OK;
}

void CImgui_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
    CObjPlace_Manager::GetInstance()->Tick(fTimeDelta);

    m_pNavigationMgr->Tick(fTimeDelta);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //�� ������Ʈ���� ���ư�
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame(); //new�� ������ �������ֱ�

    ImGuiIO& io = ImGui::GetIO();

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin(u8"IMGUI ����ȭ��");

    if (ImGui::Button(u8"��ũ�����"))
        m_bTerrain_IMGUI = true;

    if (ImGui::Button(u8"������Ʈ��ġ"))
        m_bObject_Place_IMGUI = true;

    if (ImGui::Button(u8"�׺���̼� TOOL"))
        m_bNaviTool_IMGUI = true;

    if (ImGui::Button(u8"����Ʈ TOOL"))
        m_bLightMgr_IMGUI = true;

    ImGui::End();

#pragma region IMGUI UI
    if (m_bTerrain_IMGUI)
    {
        Set_Terrain_IMGUI();
    }

    if (m_bObject_Place_IMGUI)
    {
        Show_MapObj_Place_IMGUI();
        Show_Object_List();
    }

    if (m_bNaviTool_IMGUI)
    {
        Set_NaviTool_IMGUI();
    }

    if (m_bLightMgr_IMGUI)
    {
        Show_LightTool_IMGUI();
    }

#pragma endregion
    ImGui::EndFrame();
}

void CImgui_Manager::Late_Tick(_float fTimeDelta)
{
    CObjPlace_Manager::GetInstance()->Late_Tick(fTimeDelta);

    CLightTool_Mgr::GetInstance()->Late_Tick(fTimeDelta);
}

void CImgui_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    m_pNavigationMgr->Render();
}


void CImgui_Manager::Set_Terrain_IMGUI()
{
    ImGui::Begin(u8"Terrain");
    ImGui::Text(u8"Terrain ũ�� ����");

    static int inumx, inumz;
    ImGui::InputInt(u8"ũ��x", &inumx);
    ImGui::InputInt(u8"ũ��z", &inumz);

    if (ImGui::Button(u8"Terrain ����"))
    {
        CTerrain_Manager::GetInstance()->Change_LandScale(inumx, inumz);
    }

    if (ImGui::Button("Close"))
        m_bTerrain_IMGUI = false;
    ImGui::End();
}



void CImgui_Manager::Show_Object_List()
{
    CObjPlace_Manager::GetInstance()->Show_Installed_GameObjectsList();
}

void CImgui_Manager::Set_NaviTool_IMGUI()
{
    ImGui::Begin(u8"Navigation ����");

    m_pNavigationMgr->Load_Cell_IMGUI();

    m_pNavigationMgr->Show_Cells_IMGUI();

    static bool    bCanAdd;

    
    if (m_pGameInstance->GetKeyState(DIK_X) == AWAY)
    {
        bCanAdd = true;
    }


    if (CGameInstance::GetInstance()->GetMouseState(DIM_LB) == AWAY && true == bCanAdd)
    {
        _bool		isPick;
        _vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);

        m_pNavigationMgr->Make_Point(vTargetPos);
        bCanAdd = false;
    }

    if (ImGui::Button("Close"))
        m_bNaviTool_IMGUI = false;
    ImGui::End();
}

void CImgui_Manager::Show_MapObj_Place_IMGUI()
{
    ImGui::Begin(u8"���̾� ����");

    m_pObjPlace_Manager->Set_Map_Object();

    if (ImGui::Button("Close"))
    {
        m_bObject_Place_IMGUI = false;
    }

    ImGui::End();
}

void CImgui_Manager::Show_LightTool_IMGUI()
{
    /* ���� ����Ʈ �����ֱ� */
    m_pLightTool_Mgr->Show_FileName();

    ImGui::Begin(u8"����Ʈ ��ġ");

    m_pLightTool_Mgr->Show_LightMgr_IMGUI();

    if (ImGui::Button("Close"))
    {
        m_bLightMgr_IMGUI = false;
    }

    ImGui::End();
}


void CImgui_Manager::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CObjPlace_Manager::GetInstance()->DestroyInstance();
    CLightTool_Mgr::GetInstance()->DestroyInstance();

    //m_pNavigationMgr->DestroyInstance();
    Safe_Release(m_pNavigationMgr);
    Safe_Release(m_pObjPlace_Manager);
    Safe_Release(m_pLightTool_Mgr);

    Safe_Release(m_pGameInstance);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}