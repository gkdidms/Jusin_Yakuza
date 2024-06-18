#include "../Public/Imgui_Manager.h"
#include "GameInstance.h"
#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include <Windows.h>
#include <string.h>
#include "ObjPlace_Manager.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"



IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
    :m_pNavigationMgr(CNavigation_Manager::GetInstance())
{
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

    ImGui::End();

#pragma region IMGUI UI
    if (m_bTerrain_IMGUI)
    {
        Set_Terrain_IMGUI();
    }

    if (m_bObject_Place_IMGUI)
    {
        Set_Map_Object();
        Show_Object_List();
    }

    if (m_bNaviTool_IMGUI)
    {
        Set_NaviTool_IMGUI();
    }

#pragma endregion

    ImGui::EndFrame();
}

void CImgui_Manager::Late_Tick(_float fTimeDelta)
{
    CObjPlace_Manager::GetInstance()->Late_Tick(fTimeDelta);
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
    //ImGui::Text(u8"Terrain ũ�� ����");

    //static int inumx, inumz;
    //ImGui::InputInt(u8"ũ��x", &inumx);
    //ImGui::InputInt(u8"ũ��z", &inumz);

    //if (ImGui::Button(u8"Terrain ����"))
    //{
    //    CTerrain_Manager::GetInstance()->Change_LandScale(inumx, inumz);
    //}

    //if (ImGui::Button("Close"))
    //    m_bTerrain_IMGUI = false;
    ImGui::End();
}


/* ������Ʈ ��ġ */
void CImgui_Manager::Set_Map_Object()
{
    ImGui::Begin(u8"���̾� ����");
    //ImGui::Text(u8"LayerTag �̸�");

    //const char* pLayerArray[] = { "Layer_Land","Layer_Palm","Layer_Cliff" ,"Layer_Monster", "Layer_GameObject" , "Layer_GroundGameObject" };
    //static int layer_current_idx = 0;
    //if (ImGui::BeginListBox("listbox 1"))
    //{
    //    for (int n = 0; n < IM_ARRAYSIZE(pLayerArray); n++)
    //    {
    //        const bool is_selected = (layer_current_idx == n);
    //        if (ImGui::Selectable(pLayerArray[n], is_selected))
    //            layer_current_idx = n;

    //        if (is_selected)
    //            ImGui::SetItemDefaultFocus();
    //    }
    //    ImGui::EndListBox();
    //}

    //static int object_current_idx = 0;

    //if (0 == layer_current_idx)
    //{
    //    ImGui::Text(u8"���� ��ġ");

    //    const char* items[] = { "Prototype_Component_Model_CrabMapFlat2", "Prototype_Component_Model_CrabMapFlat3", "Prototype_Component_Model_CrabMapFlat4",
    //   "Prototype_Component_Model_CrabMapFlat5", "Prototype_Component_Model_CrabMapFlat6", "Prototype_Component_Model_CrabMapFlat7", "Prototype_Component_Model_CrabMapFlat8" };

    //    if (ImGui::BeginListBox("listbox 2"))
    //    {
    //        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
    //        {
    //            const bool is_selected = (object_current_idx == n);
    //            if (ImGui::Selectable(items[n], is_selected))
    //                object_current_idx = n;

    //            if (is_selected)
    //                ImGui::SetItemDefaultFocus();
    //        }
    //        ImGui::EndListBox();
    //    }
    //}
    //if (1 == layer_current_idx)
    //{
    //    ImGui::Text(u8"���ڼ� ��ġ");

    //    const char* items[] = { "Prototype_Component_Model_Palm_1", "Prototype_Component_Model_Palm_2", "Prototype_Component_Model_Palm_3", "Prototype_Component_Model_Palm_4" };

    //    if (ImGui::BeginListBox("listbox 3"))
    //    {
    //        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
    //        {
    //            const bool is_selected = (object_current_idx == n);
    //            if (ImGui::Selectable(items[n], is_selected))
    //                object_current_idx = n;

    //            if (is_selected)
    //                ImGui::SetItemDefaultFocus();
    //        }
    //        ImGui::EndListBox();
    //    }
    //}
    //if (2 == layer_current_idx)
    //{
    //    ImGui::Text(u8"���� ��ġ");

    //    const char* items[] = { "Prototype_Component_Model_CliffFlat1" , "Prototype_Component_Model_CliffFlat2", "Prototype_Component_Model_CliffFlat3" , "Prototype_Component_Model_CliffFlat4" , "Prototype_Component_Model_CliffLarge1", "Prototype_Component_Model_CliffLarge2" , "Prototype_Component_Model_CliffLarge3"
    //    "Prototype_Component_Model_CliffLarge4" , "Prototype_Component_Model_CliffLarge5" , "Prototype_Component_Model_CliffLarge6" , "Prototype_Component_Model_CliffMedium1" ,"Prototype_Component_Model_CliffMedium2" };

    //    if (ImGui::BeginListBox("listbox 4"))
    //    {
    //        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
    //        {
    //            const bool is_selected = (object_current_idx == n);
    //            if (ImGui::Selectable(items[n], is_selected))
    //                object_current_idx = n;

    //            if (is_selected)
    //                ImGui::SetItemDefaultFocus();
    //        }
    //        ImGui::EndListBox();
    //    }
    //}
    //else if (3 == layer_current_idx)
    //{
    //    ImGui::Text(u8"���� ��ġ");

    //    const char* items[] = { "Prototype_GameObject_Ant", "Prototype_GameObject_Skull", "Prototype_GameObject_Slug" ,"Prototype_GameObject_Scorpion", "Prototype_GameObject_Pumpkin", "Prototype_Component_Model_StarFish", "Prototype_GameObject_EnemyCrab", "Prototype_GameObject_Pufferfish" };

    //    if (ImGui::BeginListBox("listbox 5"))
    //    {
    //        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
    //        {
    //            const bool is_selected = (object_current_idx == n);
    //            if (ImGui::Selectable(items[n], is_selected))
    //                object_current_idx = n;

    //            if (is_selected)
    //                ImGui::SetItemDefaultFocus();
    //        }
    //        ImGui::EndListBox();
    //    }
    //}
    //else if (4 == layer_current_idx)
    //{
    //    ImGui::Text(u8"������Ʈ ��ġ");

    //    const char* items[] = { "Prototype_GameObject_Shrub", "Prototype_GameObject_BrightStone" , "Prototype_GameObject_BombStone" };
    //    if (ImGui::BeginListBox("listbox 6"))
    //    {
    //        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
    //        {
    //            const bool is_selected = (object_current_idx == n);
    //            if (ImGui::Selectable(items[n], is_selected))
    //                object_current_idx = n;

    //            if (is_selected)
    //                ImGui::SetItemDefaultFocus();
    //        }
    //        ImGui::EndListBox();
    //    }
    //}
    //else if (5 == layer_current_idx)
    //{
    //    ImGui::Text(u8"������Ʈ ��ġ");

    //    const char* items[] = { "Prototype_Component_Model_Rock1" ,"Prototype_Component_Model_RockScatter", "Prototype_Component_Model_Acacia", "Prototype_Component_Model_Leaf" ,
    //        "Prototype_Component_Model_coral1", "Prototype_Component_Model_coral2", "Prototype_Component_Model_coral3"
    //    , "Prototype_Component_Model_seaweed" , "Prototype_Component_Model_Shrub" , "Prototype_Component_Model_seaweedshort"
    //    , "Prototype_Component_Model_CoralAsset1", "Prototype_Component_Model_CoralAsset2", "Prototype_Component_Model_CoralAsset3", "Prototype_Component_Model_CoralAsset4"
    //    , "Prototype_Component_Model_Marimo" , "Prototype_Component_Model_Clam" , "Prototype_Component_Model_CoralB1", "Prototype_Component_Model_Gorgonian", "Prototype_Component_Model_SeaFan"
    //    };

    //    if (ImGui::BeginListBox("listbox 6"))
    //    {
    //        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
    //        {
    //            const bool is_selected = (object_current_idx == n);
    //            if (ImGui::Selectable(items[n], is_selected))
    //                object_current_idx = n;

    //            if (is_selected)
    //                ImGui::SetItemDefaultFocus();
    //        }
    //        ImGui::EndListBox();
    //    }
    //}

    //if (ImGui::Button(u8"Object �߰�"))
    //{
    //    m_bDoing_Place_Object = true;
    //}

    //if (true == m_bDoing_Place_Object)
    //{
    //    bool    bcheckFinished = false;

    //    bcheckFinished = CObjPlace_Manager::GetInstance()->Add_CloneObject_Imgui(layer_current_idx, object_current_idx);

    //    if (true == bcheckFinished)
    //    {
    //        m_bDoing_Place_Object = false;
    //    }
    //}


    //if (ImGui::Button("Close"))
    //{
    //    m_bObject_Place_IMGUI = false;
    //}

    ImGui::End();
}



void CImgui_Manager::Show_Object_List()
{
    //CObjPlace_Manager::GetInstance()->Set_GameObjectsList();
}

void CImgui_Manager::Set_NaviTool_IMGUI()
{
    ImGui::Begin(u8"Navigation ����");

    //m_pNavigationMgr->Load_Cell_IMGUI();

    //m_pNavigationMgr->Show_Cells_IMGUI();

    //static bool    bCanAdd;

    //if (CGameInstance::GetInstance()->Key_Pressing(DIK_X))
    //{
    //    bCanAdd = true;
    //}


    //if (CGameInstance::GetInstance()->Mouse_Pressing(DIM_LB) && true == bCanAdd)
    //{
    //    _bool		isPick;
    //    _vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);

    //    m_pNavigationMgr->Make_Point(vTargetPos);
    //    bCanAdd = false;
    //}

    //if (ImGui::Button("Close"))
    //    m_bNaviTool_IMGUI = false;
    ImGui::End();
}


void CImgui_Manager::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CObjPlace_Manager::GetInstance()->DestroyInstance();

    m_pNavigationMgr->DestroyInstance();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}