#include "../Public/Imgui_Manager.h"
#include "GameInstance.h"
#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include <Windows.h>
#include <string.h>

#include "PipeLine.h"

#include "ImGuiFileDialog.h"

#include "Object_Manager.h"
#include "UI_Texture.h"


IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager() :
    m_pGameInstance { CGameInstance::GetInstance()},
    m_pObjectManager { CObject_Manager::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pObjectManager);
}

HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    m_pObjectManager->Initialize(pDevice, pContext);

    // Win32 초기화 호출
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\fonts\\malgun.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());

    /* IMGUI 초기화 */
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.0f));

    return S_OK;
}

void CImgui_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //매 업데이트마다 돌아감
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame(); //new와 같은것 선언해주기

    ImGuiIO& io = ImGui::GetIO();

    Window_Image();

    Guizmo_Test();
}

void CImgui_Manager::Window_Image()
{
    ImGui::Begin("Image");

    if (ImGui::CollapsingHeader(u8"그룹 / 렌더타겟 이미지 생성"))
    {
        //1.그룹을 만들기
        auto Objects = m_pObjectManager->Get_Objects();

        ImGui::InputText(u8"생성할 Group 이름", m_szGroupName, MAX_PATH);
        if (ImGui::Button(u8"Group 생성"))
        {
            if (FAILED(m_pObjectManager->Add_Group(m_pGameInstance->StringToWstring(string(m_szGroupName)))))
            {
                MSG_BOX("Group 생성 실패 / 이미 존재하는 그룹입니다.");
            }
        }


        if (Objects.size() != 0)
        {
            auto iter = Objects.begin();
            string strName = "";
            if (ImGui::BeginListBox("GroupList"))
            {
                for (int n = 0; n < Objects.size(); n++)
                {
                    const bool is_selected = (m_iGroupIndex == n);
                    strName = m_pGameInstance->WstringToString(iter->first);
                    if (ImGui::Selectable(strName.c_str(), is_selected))
                    {
                        m_iGroupIndex = n;
                        m_strSelectTag = strName;
                        m_iGroupObjectIndex = -1; // 초기화 시키기
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();

                    ++iter;
                }
                ImGui::EndListBox();
            }


            if (ImGui::Button(u8"이미지 파일 생성"))
            {
                IGFD::FileDialogConfig config;
                config.path = "../../Client/Bin/Resources/Textures/UI";
                ImGuiFileDialog::Instance()->OpenDialog("ChooseTextureKey", "Choose File", ".dds, .png", config);
            }
            if (ImGuiFileDialog::Instance()->Display("ChooseTextureKey")) {
                if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                    // action

                    CObject_Manager::OBJ_MNG_DESC Desc{};
                    Desc.strFileName = m_pGameInstance->StringToWstring(fileName);
                    Desc.strFilePath = m_pGameInstance->StringToWstring(filePathName);
                    Desc.iTextureType = CObject_Manager::IMG;

                    if (FAILED(m_pObjectManager->Add_Object(m_pGameInstance->StringToWstring(strName), &Desc)))
                        MSG_BOX("이미지 생성 실패");
                }
                 
                // close
                ImGuiFileDialog::Instance()->Close();
            }

            ImGui::SameLine();

            if (ImGui::Button("Group Remove"))
            {
                m_pObjectManager->Remove_Group(m_pGameInstance->StringToWstring(m_strSelectTag));
                m_iGroupIndex--;

                if (m_iGroupIndex < 0 || Objects.size() <= 1) // 기존에 받아왔던 Objects 사이즈가 1보다 같거나 작다면
                    m_iGroupIndex = -1;

                m_iGroupObjectIndex = -1; // 초기화
            }

            ImGui::SameLine();

            if (ImGui::Button("Group Copy"))
            {
                m_pObjectManager->Copy_Group(m_pGameInstance->StringToWstring(m_strSelectTag));
            }

            if (m_iGroupIndex != -1)
            {
                vector<CUI_Texture*> Textures = m_pObjectManager->Get_GroupObject(m_pGameInstance->StringToWstring(m_strSelectTag));

                string strObjectName = "";

                if (Textures.size() > 0)
                {
                    auto iter = Textures.begin();
                    if (ImGui::BeginListBox("ObjectList"))
                    {
                        for (int n = 0; n < Textures.size(); n++)
                        {
                            const bool is_selected = (m_iGroupObjectIndex == n);
                            strObjectName = m_pGameInstance->WstringToString(Textures[n]->Get_FileName());
                            if (ImGui::Selectable(strObjectName.c_str(), is_selected))
                            {
                                m_iGroupObjectIndex = n;

                                m_WorldMatirx = *Textures[n]->Get_TransformCom()->Get_WorldFloat4x4();
                            }
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();

                            if (iter != Textures.end())
                                ++iter;
                        }
                        ImGui::EndListBox();
                    }

                    if (ImGui::Button("Object Remove"))
                    {
                        m_pObjectManager->Remove_Object(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex);
                        m_iGroupObjectIndex = -1; // 초기화 시키기
                    }
                }

                if (m_iGroupObjectIndex != -1)
                {
                    ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");

                    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
                        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
                    ImGui::SameLine();
                    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
                        mCurrentGizmoOperation = ImGuizmo::ROTATE;
                    ImGui::SameLine();
                    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
                        mCurrentGizmoOperation = ImGuizmo::SCALE;

                    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
                    ImGuizmo::DecomposeMatrixToComponents((_float*)&m_WorldMatirx, matrixTranslation, matrixRotation, matrixScale);
                    ImGui::InputFloat3("Tr", matrixTranslation);
                    ImGui::InputFloat3("Rt", matrixRotation);
                    ImGui::InputFloat3("Sc", matrixScale);
                    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (_float*)&m_WorldMatirx);

                    Textures[m_iGroupObjectIndex]->Get_TransformCom()->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatirx));


                    //쉐이더 선택
                    _int iPass = Textures[m_iGroupObjectIndex]->Get_ShaderPass();

                    if (ImGui::RadioButton("NonBlend", &iPass, 0))
                        Textures[m_iGroupObjectIndex]->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Blend", &iPass, 1))
                        Textures[m_iGroupObjectIndex]->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Alpha Blend", &iPass, 2))
                        Textures[m_iGroupObjectIndex]->Set_ShaderPass(iPass);

                }
            }
        }

        ImGui::NewLine();
        if (ImGui::Button(u8"렌더타겟 이미지 생성"))
        {
            /*if (FAILED(m_pGameInstance->Create_Texture(TEXT("Target_Diffuse"), TEXT("../../Client/Bin/Resources/Textures/UI/CreateFile/test.dds"))))
            {
                MSG_BOX("렌더타겟 이미지 생성 실패");
            }*/
        }
    }

    if (ImGui::CollapsingHeader(u8"Button 생성"))
    {
        if (ImGui::Button(u8"Button 파일 열기"))
        {
            IGFD::FileDialogConfig config;
            config.path = "../../Client/Bin/Resources/Textures/UI";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseButtonKey", "Choose File", ".dds", config);
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseButtonKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                // action

                CObject_Manager::OBJ_MNG_DESC Desc{};
                Desc.strFileName = m_pGameInstance->StringToWstring(fileName);
                Desc.strFilePath = m_pGameInstance->StringToWstring(filePathName);
                Desc.iTextureType = CObject_Manager::BTN;

                m_pObjectManager->Add_Object(TEXT("Layer_Image"), &Desc);
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
    }



    ImGui::End();
}

void CImgui_Manager::Guizmo_Test()
{
    ImGuizmo::BeginFrame();

    static bool useSnap = true;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    float viewManipulateRight = g_iWinSizeX;
    float viewManipulateTop = 0;

    ImGuizmo::SetRect(0, 0, g_iWinSizeX, g_iWinSizeY);

    _matrix cameraView = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
    _matrix viewInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW); // 카메라 월드 
    _matrix cameraProjection = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
    _float viewMatrix[16], projMatrix[16], viewInverseMatrix[16];


    memcpy(&viewMatrix, &cameraView, sizeof(_matrix));
    memcpy(&viewInverseMatrix, &viewInverse, sizeof(_matrix));
    memcpy(&projMatrix, &cameraProjection, sizeof(_matrix));

   // ImGuizmo::DrawGrid(viewMatrix, projMatrix, (_float*)&identityMatrix, 100.f);

    if (m_iGroupObjectIndex != -1)
        ImGuizmo::Manipulate((_float*)&m_ViewMatrix, (_float*)&m_ProjMatrix, mCurrentGizmoOperation, mCurrentGizmoMode, (_float*)&m_WorldMatirx, NULL, useSnap ? &snap[0] : NULL);

    //memcpy(&cameraView, &viewInverseMatrix, sizeof(_float) * 16);
    //m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, cameraView));
}

HRESULT CImgui_Manager::Create_Texture()
{
    //if (FAILED(m_pGameInstance->Copy_Resource(TEXT("Target_BackBuffer"), m_pTexture2D)))
    //    return E_FAIL;

    //if (FAILED(SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../../Create/Bin/Resources/Textures/UI/CreateFile/test.dds"))))
    //    return E_FAIL;

    return S_OK;
}

void CImgui_Manager::Late_Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

void CImgui_Manager::Release_Imgui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CImgui_Manager::GetInstance()->DestroyInstance();
}

void CImgui_Manager::Free()
{
    Safe_Release(m_pGameInstance);

    Safe_Release(m_pObjectManager);
    

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}