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
#include "Image_Texture.h"
#include "Text.h"
#include "Btn.h"
#include "Group.h"

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
    Window_Binary();
    Window_Binary_Group();

    Guizmo_Test();
}

void CImgui_Manager::Window_Image()
{
    ImGui::Begin(u8"렌더타겟 이미지 생성");

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

            ImGui::RadioButton(u8"이미지 생성", &m_iObjectType, 0); ImGui::SameLine();
            ImGui::RadioButton(u8"텍스트 생성", &m_iObjectType, 2);

            if (m_iObjectType == 0)
            {
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

                        if (FAILED(m_pObjectManager->Add_RenderTextureObject(m_pGameInstance->StringToWstring(strName), &Desc)))
                            MSG_BOX("이미지 생성 실패");
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }
            }
            else if (m_iObjectType == 2)
            {
                ImGui::InputText(u8"텍스트 작성 : ", m_szText, MAX_PATH);

                static ImVec4 vAmbientColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
                ImGui::ColorEdit4(u8"Color 선택", (float*)&m_vColor, ImGuiColorEditFlags_Float);

                if (ImGui::Button("Create"))
                {
                    //텍스쳐 생성
                    CObject_Manager::OBJ_MNG_DESC Desc{};
                    Desc.strText = m_pGameInstance->StringToWstring(string(m_szText));
                    Desc.iTextureType = m_iObjectType;
                    Desc.vColor = XMLoadFloat4(&m_vColor);

                    m_pObjectManager->Add_RenderTextureObject(m_pGameInstance->StringToWstring(strName), &Desc);
                }
            }
            
            if (m_iGroupIndex != -1)
            {
                vector<CUI_Object*> Textures = m_pObjectManager->Get_GroupObject(m_pGameInstance->StringToWstring(m_strSelectTag));

                string strObjectName = "";

                if (Textures.size() > 0)
                {
                    auto iter = Textures.begin();
                    if (ImGui::BeginListBox("ObjectList"))
                    {
                        for (int n = 0; n < Textures.size(); n++)
                        {
                            const bool is_selected = (m_iGroupObjectIndex == n);
                            strObjectName = m_pGameInstance->WstringToString(dynamic_cast<CUI_Texture*>(Textures[n])->Get_FileName());
                            if (ImGui::Selectable(strObjectName.c_str(), is_selected))
                            {
                                m_iGroupObjectIndex = n;

                                m_WorldMatirx = *Textures[n]->Get_TransformCom()->Get_WorldFloat4x4();
                                m_iBinaryPick = RENDER_TARGET;
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
                    ImGui::SameLine();
                    if (ImGui::Button("Up"))
                    {
                        if (FAILED(m_pObjectManager->Move_ObjectIndex(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex, CObject_Manager::UP)))
                            MSG_BOX("이동 실패");
                        else
                            m_iGroupObjectIndex++;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Down"))
                    {
                        if (FAILED(m_pObjectManager->Move_ObjectIndex(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex, CObject_Manager::DOWN)))
                            MSG_BOX("이동 실패");
                        else
                            m_iGroupObjectIndex--;
                    }

                    Textures = m_pObjectManager->Get_GroupObject(m_pGameInstance->StringToWstring(m_strSelectTag)); // 파일 다시 불러오기
                }

                if (m_iGroupObjectIndex != -1 && m_iBinaryPick == RENDER_TARGET)
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
                    _int iPass =  dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Get_ShaderPass();

                    if (ImGui::RadioButton("NonBlend", &iPass, 0))
                        dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Blend", &iPass, 1))
                        dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Alpha Blend", &iPass, 2))
                        dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_ShaderPass(iPass);

                }
            }
        }

        ImGui::NewLine();
        if (ImGui::Button(u8"렌더타겟 이미지 생성"))
        {
            wstring strFilePath = TEXT("../../Client/Bin/Resources/Textures/UI/CreateFile/") + m_pGameInstance->StringToWstring(m_strSelectTag) + TEXT(".dds");
            if (FAILED(m_pObjectManager->Create_Texture(m_iGroupIndex, strFilePath)))
            {
                MSG_BOX("렌더타겟 이미지 생성 실패");
            }
        }
        ImGui::NewLine();
    }

    ImGui::End();
}

void CImgui_Manager::Window_Binary()
{
    ImGui::Begin(u8"바이너리 그룹");

    auto Objects = m_pObjectManager->Get_BinaryObjects();

    //그룹 선택은 렌더타겟 윈도우와 연동되어있다.
    if (Objects.size() != 0)
    {
        auto iter = Objects.begin();
        string strName = "";
        if (ImGui::BeginListBox("GroupList"))
        {
            for (int n = 0; n < Objects.size(); n++)
            {
                const bool is_selected = (m_iBinaryGroupIndex == n);
                strName = m_pGameInstance->WstringToString(iter->first);
                if (ImGui::Selectable(strName.c_str(), is_selected))
                {
                    m_iBinaryGroupIndex = n;
                    m_strBinarySelectTag = strName;
                    m_iBinaryObjectIndex = -1; // 초기화 시키기
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();

                ++iter;
            }
            ImGui::EndListBox();
        }

        ImGui::NewLine();
        ImGui::Checkbox(u8"바이너리 오브젝트 생성", &m_isCreateBinaryObject);
        if (m_isCreateBinaryObject)
        {
            ImGui::SeparatorText(u8"오브젝트 생성");
            ImGui::RadioButton(u8"이미지", &m_iBinaryObjectType, 0); ImGui::SameLine();
            ImGui::RadioButton(u8"버튼", &m_iBinaryObjectType, 1); ImGui::SameLine();
            ImGui::RadioButton(u8"텍스트", &m_iBinaryObjectType, 2); ImGui::SameLine();
            ImGui::RadioButton(u8"텍스트 포멧", &m_iBinaryObjectType, 3); ImGui::SameLine();
            ImGui::RadioButton(u8"그룹", &m_iBinaryObjectType, 4);

            ImGui::InputText(u8"저장할 오브젝트 이름 : ", m_szObjectName, MAX_PATH);

            //오브젝트 생성
            if (m_iBinaryObjectType == 0)
            {
                //일반 이미지
                if (ImGui::Button(u8"이미지 파일 생성"))
                {
                    IGFD::FileDialogConfig config;
                    config.path = "../../Client/Bin/Resources/Textures/UI";
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseBinaryTextureKey", "Choose File", ".dds, .png", config);
                }
                if (ImGuiFileDialog::Instance()->Display("ChooseBinaryTextureKey")) {
                    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                        std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                        // action

                        CObject_Manager::OBJ_MNG_DESC Desc{};
                        Desc.strFileName = m_pGameInstance->StringToWstring(fileName);
                        Desc.strFilePath = m_pGameInstance->StringToWstring(filePathName);
                        Desc.iTextureType = CObject_Manager::IMG;
                        Desc.strName = m_szObjectName;

                        if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(strName), &Desc)))
                            MSG_BOX("이미지 생성 실패");
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }
            }
            else if (m_iBinaryObjectType == 1)
            {
                //버튼 생성
                if (ImGui::Button(u8"이미지 파일"))
                {
                    IGFD::FileDialogConfig config;
                    config.path = "../../Client/Bin/Resources/Textures/UI";
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseBtnTextureKey", "ChooseBtnFile", ".dds, .png", config);
                }
                if (ImGuiFileDialog::Instance()->Display("ChooseBtnTextureKey")) {
                    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                        m_strBtnFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
                        m_strBtnFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }

                if (ImGui::Button(u8"클릭 이미지 파일"))
                {
                    IGFD::FileDialogConfig config;
                    config.path = "../../Client/Bin/Resources/Textures/UI";
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseBtnClickTextureKey", "Choose File", ".dds, .png", config);
                }
                if (ImGuiFileDialog::Instance()->Display("ChooseBtnClickTextureKey")) {
                    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                        m_strBtnClickFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
                        m_strBtnClickFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }

                if (ImGui::Button(u8"버튼 생성"))
                {
                    CObject_Manager::OBJ_MNG_DESC Desc{};
                    Desc.strFilePath = m_pGameInstance->StringToWstring(m_strBtnFilePath);
                    Desc.strFileName = m_pGameInstance->StringToWstring(m_strBtnFileName);
                    Desc.strBtnClickFilePath = m_pGameInstance->StringToWstring(m_strBtnClickFilePath);
                    Desc.strBtnClickFileName = m_pGameInstance->StringToWstring(m_strBtnClickFileName);
                    Desc.strName = m_szObjectName;
                    Desc.iTextureType = m_iBinaryObjectType;

                    if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                        MSG_BOX("버튼 생성 실패");
                }
            }
            else if (m_iBinaryObjectType == 2 || m_iBinaryObjectType == 3)
            {
                //텍스트 생성
                ImGui::InputText(u8"텍스트 작성 : ", m_szText, MAX_PATH);

                static ImVec4 vAmbientColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
                ImGui::ColorEdit4(u8"vColor 선택", (float*)&m_vColor, ImGuiColorEditFlags_Float);

                if (ImGui::Button(u8"텍스트 생성"))
                {
                    //텍스쳐 생성
                    CObject_Manager::OBJ_MNG_DESC Desc{};
                    Desc.strText = m_pGameInstance->StringToWstring(string(m_szText));
                    Desc.iTextureType = m_iBinaryObjectType;
                    Desc.vColor = XMLoadFloat4(&m_vColor);
                    Desc.strName = m_szObjectName;

                    m_pObjectManager->Add_RenderTextureObject(TEXT("Layer_Image"), &Desc);
                }
            }
            else if (m_iBinaryObjectType == 4)
            {
                //그룹 생성
                if (ImGui::Button(u8"그룹 생성"))
                {
                    CObject_Manager::OBJ_MNG_DESC Desc{};
                    Desc.strName = m_szObjectName;
                    Desc.iTextureType = m_iBinaryObjectType;

                    if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                        MSG_BOX("바이너리 오브젝트 생성 실패");
                }
            }
        }

        //생성된 오브젝트 리스트
        if (m_iBinaryGroupIndex != -1)
        {
            auto BinaryObject = m_pObjectManager->Get_GroupBinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag));

            if (ImGui::BeginListBox("BinaryObjectList"))
            {
                for (int n = 0; n < BinaryObject.size(); n++)
                {
                    const bool is_selected = (m_iBinaryObjectIndex == n);
                    string strBinaryName = BinaryObject[n]->Get_Name();
                    if (ImGui::Selectable(strBinaryName.c_str(), is_selected))
                    {
                        m_iBinaryObjectIndex = n;
                        m_WorldMatirx = *BinaryObject[n]->Get_TransformCom()->Get_WorldFloat4x4();
                        m_iBinaryPick = BINARY_OBJECT;

                        m_isOpenBinaryGroup = BinaryObject[m_iBinaryObjectIndex]->Get_TypeIndex() == CObject_Manager::GROUP;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            if (ImGui::Button("Remove"))
            {
                if (FAILED(m_pObjectManager->Remove_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex)))
                    MSG_BOX("바이너리 오브젝트 삭제 불가");

                if (BinaryObject.size() <= 1)
                    m_iBinaryObjectIndex = -1;

                auto BinaryObject = m_pObjectManager->Get_GroupBinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag));
            }

            //생성된 바이너리에 따라 수정 옵션 추가 
            if (m_iBinaryObjectIndex != -1 && m_iBinaryPick == BINARY_OBJECT)
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

                BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatirx));


                //쉐이더 선택
                if (!m_isOpenBinaryGroup)
                {
                    _int iPass = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_ShaderPass();

                    if (ImGui::RadioButton("NonBlend", &iPass, 0))
                        dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Blend", &iPass, 1))
                        dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Alpha Blend", &iPass, 2))
                        dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(iPass);
                }
            }
        }
        else m_isOpenBinaryGroup = false;
    }
    
    ImGui::End();
}

void CImgui_Manager::Window_Binary_Group()
{
    if (!m_isOpenBinaryGroup) return;

    ImGui::Begin(u8"바이너리 오브젝트 그룹들");

    //생성
    ImGui::Checkbox(u8"오브젝트 생성", &m_isCreateBinaryGroupObject);

    if (m_isCreateBinaryGroupObject)
    {
        auto BinaryObject = m_pObjectManager->Get_GroupBinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag));
        //생성 가능하다면
        ImGui::SeparatorText(u8"오브젝트 생성");
        ImGui::RadioButton(u8"이미지", &m_iBinaryGroupObjectType, 0); ImGui::SameLine();
        ImGui::RadioButton(u8"버튼", &m_iBinaryGroupObjectType, 1); ImGui::SameLine();
        ImGui::RadioButton(u8"텍스트", &m_iBinaryGroupObjectType, 2); ImGui::SameLine();
        ImGui::RadioButton(u8"텍스트 포멧", &m_iBinaryGroupObjectType, 3); ImGui::SameLine();

        ImGui::InputText(u8"저장할 오브젝트 이름 : ", m_szBinaryGroupObjectName, MAX_PATH);

        //오브젝트 생성
        if (m_iBinaryGroupObjectType == 0)
        {
            //일반 이미지
            if (ImGui::Button(u8"이미지 파일 생성"))
            {
                IGFD::FileDialogConfig config;
                config.path = "../../Client/Bin/Resources/Textures/UI";
                ImGuiFileDialog::Instance()->OpenDialog("ChooseBinaryTextureKey", "Choose File", ".dds, .png", config);
            }
            if (ImGuiFileDialog::Instance()->Display("ChooseBinaryTextureKey")) {
                if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                    // action

                    CImage_Texture::UI_TEXTURE_DESC Desc{};
                    Desc.strTextureFileName = m_pGameInstance->StringToWstring(fileName);
                    Desc.strTextureFilePath = m_pGameInstance->StringToWstring(filePathName);
                    Desc.iTypeIndex = CObject_Manager::IMG;
                    Desc.strName = m_szObjectName;
                    Desc.isParent = true;
                    Desc.pParentMatrix = BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Get_WorldFloat4x4();
                    CImage_Texture* pImage = dynamic_cast<CImage_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &Desc));
                    dynamic_cast<CGroup*>(BinaryObject[m_iBinaryObjectIndex])->Set_PartObject(pImage);
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }
        }
        else if (m_iBinaryGroupObjectType == 1)
        {
            //버튼 생성
            if (ImGui::Button(u8"이미지 파일"))
            {
                IGFD::FileDialogConfig config;
                config.path = "../../Client/Bin/Resources/Textures/UI";
                ImGuiFileDialog::Instance()->OpenDialog("ChooseBtnTextureKey", "Choose File", ".dds, .png", config);
            }
            if (ImGuiFileDialog::Instance()->Display("ChooseBtnTextureKey")) {
                if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                    m_strBtnFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
                    m_strBtnFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGui::Button(u8"클릭 이미지 파일"))
            {
                IGFD::FileDialogConfig config;
                config.path = "../../Client/Bin/Resources/Textures/UI";
                ImGuiFileDialog::Instance()->OpenDialog("ChooseBtnClickTextureKey", "Choose File", ".dds, .png", config);
            }
            if (ImGuiFileDialog::Instance()->Display("ChooseBtnClickTextureKey")) {
                if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                    m_strBtnClickFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
                    m_strBtnClickFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGui::Button(u8"버튼 생성"))
            {
                CObject_Manager::OBJ_MNG_DESC Desc{};
                Desc.strFilePath = m_pGameInstance->StringToWstring(m_strBtnFilePath);
                Desc.strFileName = m_pGameInstance->StringToWstring(m_strBtnFileName);
                Desc.strBtnClickFilePath = m_pGameInstance->StringToWstring(m_strBtnClickFilePath);
                Desc.strBtnClickFileName = m_pGameInstance->StringToWstring(m_strBtnClickFileName);
                Desc.strName = m_szObjectName;
                Desc.iTextureType = m_iBinaryGroupObjectType;

            }
        }
        else if (m_iBinaryGroupObjectType == 2 || m_iBinaryGroupObjectType == 3)
        {
            //텍스트 생성
            ImGui::InputText(u8"텍스트 작성 : ", m_szText, MAX_PATH);

            static ImVec4 vAmbientColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
            ImGui::ColorEdit4(u8"vColor 선택", (float*)&m_vColor, ImGuiColorEditFlags_Float);

            if (ImGui::Button(u8"텍스트 생성"))
            {
                //텍스쳐 생성
                CObject_Manager::OBJ_MNG_DESC Desc{};
                Desc.strText = m_pGameInstance->StringToWstring(string(m_szText));
                Desc.iTextureType = m_iBinaryGroupObjectType;
                Desc.vColor = XMLoadFloat4(&m_vColor);
                Desc.strName = m_szObjectName;
            }
        }
        else if (m_iBinaryGroupObjectType == 4)
        {
            //그룹 생성
            if (ImGui::Button(u8"그룹 생성"))
            {
                CObject_Manager::OBJ_MNG_DESC Desc{};
                Desc.strName = m_szObjectName;
                Desc.iTextureType = m_iBinaryGroupObjectType;

                if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                    MSG_BOX("바이너리 오브젝트 생성 실패");
            }
        }
    }

    //생성된 오브젝트 확인 가능
    vector<CUI_Object*> Objects = m_pObjectManager->Get_BinaryGroupObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex);
    if (ImGui::BeginListBox("BinaryGroupObjectList"))
    {
        for (int n = 0; n < Objects.size(); n++)
        {
            const bool is_selected = (m_iBinaryGroupObjectIndex == n);
            string strName = Objects[n]->Get_Name();
            if (ImGui::Selectable(strName.c_str(), is_selected))
            {
                m_iBinaryGroupObjectIndex = n;
                m_WorldMatirx = *Objects[n]->Get_TransformCom()->Get_WorldFloat4x4();
                m_iBinaryPick = GROUP_OBJECT;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    if (m_iBinaryGroupObjectIndex != -1 && m_iBinaryPick == GROUP_OBJECT)
    {
        //바이너리 선택했을때
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

        Objects[m_iBinaryGroupObjectIndex]->Get_TransformCom()->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatirx));
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

    if (m_iGroupObjectIndex != -1 || m_iBinaryObjectIndex != -1)
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