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
#include "UI_Effect.h"

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

    // Win32 �ʱ�ȭ ȣ��
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF("C:\\Windows\\fonts\\malgun.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());

    /* IMGUI �ʱ�ȭ */
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
    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 0.00f);

    //�� ������Ʈ���� ���ư�
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame(); //new�� ������ �������ֱ�

    ImGuiIO& io = ImGui::GetIO();

    Window_Image();
    Window_Binary();
    Window_Binary_Group();

    Guizmo_Test();
}

void CImgui_Manager::Window_Image()
{
    ImGui::Begin(u8"����Ÿ�� �̹��� ����",nullptr , ImGuiWindowFlags_NoCollapse);

    if (ImGui::CollapsingHeader(u8"�׷� / ����Ÿ�� �̹��� ����"))
    {
        //1.�׷��� �����
        auto Objects = m_pObjectManager->Get_Objects();
        ImGui::Text(u8"UI Ʋ �̸�");
        ImGui::InputText(u8" ", m_szGroupName, MAX_PATH);
        if (ImGui::Button(u8"UI Ʋ ����"))
        {
            if ('\0' != m_szGroupName[0])
            {
                if (FAILED(m_pObjectManager->Add_Group(m_pGameInstance->StringToWstring(string(m_szGroupName)))))
                {
                    MSG_BOX("Group ���� ���� / �̹� �����ϴ� �׷��Դϴ�.");
                }
            }
            else
                MSG_BOX("�̸� ���� ������!");
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
                        m_iGroupObjectIndex = -1; // �ʱ�ȭ ��Ű��
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

                if (m_iGroupIndex < 0 || Objects.size() <= 1) // ������ �޾ƿԴ� Objects ����� 1���� ���ų� �۴ٸ�
                    m_iGroupIndex = -1;

                m_iGroupObjectIndex = -1; // �ʱ�ȭ
            }

            ImGui::SameLine();

            if (ImGui::Button("Group Copy"))
            {
                m_pObjectManager->Copy_Group(m_pGameInstance->StringToWstring(m_strSelectTag));
            }

            ImGui::RadioButton(u8"�̹��� ����", &m_iObjectType, CObject_Manager::IMG); ImGui::SameLine();
            ImGui::RadioButton(u8"�ؽ�Ʈ ����", &m_iObjectType, CObject_Manager::TEXT);

            ImGui::InputText(u8"�̹����̸�", m_szImageName, MAX_PATH);

            if (m_iObjectType == CObject_Manager::IMG)
            {
                if (ImGui::Button(u8"�̹��� ���� ����"))
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
                        if('\0'!= m_szImageName[0])
                        {
                            CObject_Manager::OBJ_MNG_DESC Desc{};
                            Desc.strName = string(m_szImageName);
                            Desc.strFileName = m_pGameInstance->StringToWstring(fileName);
                            Desc.strFilePath = m_pGameInstance->StringToWstring(filePathName);
                            Desc.iTextureType = CObject_Manager::IMG;

                            if (FAILED(m_pObjectManager->Add_RenderTextureObject(m_pGameInstance->StringToWstring(strName), &Desc)))
                                MSG_BOX("�̹��� ���� ����");
                        }
                        else
                        {
                            MSG_BOX("�̸� �ۼ�");
                        }
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }
            }
            else if (m_iObjectType == CObject_Manager::TEXT)
            {
                ImGui::InputText(u8"�ؽ�Ʈ �ۼ� : ", m_szText, MAX_PATH);

                //static ImVec4 vAmbientColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
                //ImGui::ColorEdit4(u8"Color ����", (float*)&m_vColor, ImGuiColorEditFlags_Float);

                if (ImGui::Button("Create"))
                {
                    //�ؽ��� ����
                    CObject_Manager::OBJ_MNG_DESC Desc{};
                    Desc.strText = m_pGameInstance->StringToWstring(string(m_szText));
                    Desc.iTextureType = m_iObjectType;
                    Desc.vColor = m_vColor;
                    Desc.strName = string(m_szImageName);

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
                            strObjectName = dynamic_cast<CUI_Texture*>(Textures[n])->Get_Name();
                            if (ImGui::Selectable(strObjectName.c_str(), is_selected))
                            {
                                m_iGroupObjectIndex = n;

                                m_WorldMatirx = *Textures[n]->Get_TransformCom()->Get_WorldFloat4x4();
                                m_StartUV = dynamic_cast<CUI_Texture*>(Textures[n])->Get_StartUV();
                                m_EndUV = dynamic_cast<CUI_Texture*>(Textures[n])->Get_EndUV();
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
                        m_iGroupObjectIndex = -1; // �ʱ�ȭ ��Ű��
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"������"))
                    {
                        if (FAILED(m_pObjectManager->Move_ObjectIndex(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex, CObject_Manager::UP)))
                            MSG_BOX("�̵� ����");
                        else
                            m_iGroupObjectIndex++;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"�ڷ�"))
                    {
                        if (FAILED(m_pObjectManager->Move_ObjectIndex(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex, CObject_Manager::DOWN)))
                            MSG_BOX("�̵� ����");
                        else
                            m_iGroupObjectIndex--;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"����"))
                    {
                        if (FAILED(m_pObjectManager->Copy_RenderTextureObject(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex)))
                            MSG_BOX("�̹��� ���� ����");
                    }
                    Textures = m_pObjectManager->Get_GroupObject(m_pGameInstance->StringToWstring(m_strSelectTag)); // ���� �ٽ� �ҷ�����
                }

                ImGui::Text(to_string(m_iGroupObjectIndex).c_str());
                if (m_iGroupObjectIndex != -1 && m_iBinaryPick == RENDER_TARGET)
                {
                    ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");

                    if (ImGui::IsKeyPressed(ImGuiKey_W))
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
                    ImGuizmo::DecomposeMatrixToComponents((_float*)&m_WorldMatirx, matrixTranslation, matrixRotation, matrixScale);
                    ImGui::InputFloat3("Tr", matrixTranslation);
                    ImGui::InputFloat3("Rt", matrixRotation);
                    ImGui::InputFloat3("Sc", matrixScale);
                    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (_float*)&m_WorldMatirx);

                    Textures[m_iGroupObjectIndex]->Get_TransformCom()->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatirx));


                    //���̴� ����
                    _int iPass =  dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Get_ShaderPass();

                    if (ImGui::RadioButton("NonBlend", &iPass, 0))
                        dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Blend", &iPass, 1))
                        dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Alpha Blend", &iPass, 2))
                        dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Color Alpha Blend", &iPass, 4))
                        dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Color Alpha Blend Effect", &iPass, 5))
                        dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_ShaderPass(iPass);


                    if(4==iPass)
                    {
                        _float4 vColor = dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Get_Color();
                        static bool alpha_preview = true;
                        static bool alpha_half_preview = false;
                        static bool drag_and_drop = true;
                        static bool options_menu = true;
                        static bool hdr = false;
                        ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

                        if (ImGui::ColorEdit4("MyColor##2f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
                        {
                            dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_Color(vColor);
                        }
                    }
                    else if (5 == iPass)
                    {
                        _float4 vColor = dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Get_Color();
                        static bool alpha_preview = true;
                        static bool alpha_half_preview = false;
                        static bool drag_and_drop = true;
                        static bool options_menu = true;
                        static bool hdr = false;
                        ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

                        if (ImGui::ColorEdit4("MyColor##2f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
                        {
                            dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_Color(vColor);
                        }

                        
                    }
                    
                    if(CObject_Manager::TEXT != dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Get_TypeIndex())
                    {
                        ImGui::DragFloat2("StartUV", (float*)&m_StartUV, 0.001f, 0.0f, 1.f);
                        ImGui::DragFloat2("EndUV", (float*)&m_EndUV, 0.001f, 0.0f, 1.f);

                        if (FAILED(dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Change_UV(m_StartUV, m_EndUV)))
                            MSG_BOX("UV ���� ����");
                    }
                    
                }
            }
        }

        ImGui::NewLine();
        if (ImGui::Button(u8"UI Ʋ ����"))
        {
            if("" != m_strSelectTag)
            {
                wstring strFilePath = TEXT("../../Client/Bin/Resources/Textures/UI/CreateFile/") + m_pGameInstance->StringToWstring(m_strSelectTag) + TEXT(".dds");
                if (FAILED(m_pObjectManager->Create_Texture(m_iGroupIndex, strFilePath)))
                {
                    MSG_BOX("UI �̹��� ���� ����");
                }
            }
            else
            {
                MSG_BOX("UI ����� ��������");
            }
        }
        ImGui::NewLine();
    }

    ImGui::End();
}

void CImgui_Manager::Window_Binary()
{
    ImGui::Begin(u8"���̳ʸ� �׷�");

    auto Objects = m_pObjectManager->Get_BinaryObjects();

    //�׷� ������ ����Ÿ�� ������� �����Ǿ��ִ�.
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
                    m_iBinaryObjectIndex = -1; // �ʱ�ȭ ��Ű��
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();

                ++iter;
            }
            ImGui::EndListBox();
        }

        ImGui::NewLine();
        ImGui::Checkbox(u8"���̳ʸ� ������Ʈ ����", &m_isCreateBinaryObject);

        if(ImGui::Button(u8"���̳ʸ� ����"))
        {
            m_pObjectManager->Save_binary();
        }

        if (m_isCreateBinaryObject)
        {
            
            ImGui::SeparatorText(u8"������Ʈ ����");
            ImGui::RadioButton(u8"�̹���", &m_iBinaryObjectType, CObject_Manager::IMG); ImGui::SameLine();
            ImGui::RadioButton(u8"��ư", &m_iBinaryObjectType, CObject_Manager::BTN); ImGui::SameLine();
            ImGui::RadioButton(u8"�ؽ�Ʈ", &m_iBinaryObjectType, CObject_Manager::TEXT); ImGui::SameLine();
            ImGui::RadioButton(u8"�׷�", &m_iBinaryObjectType, CObject_Manager::GROUP); ImGui::SameLine();
            ImGui::RadioButton(u8"UI����Ʈ", &m_iBinaryObjectType, CObject_Manager::EFFECT);
            ImGui::NewLine();
            ImGui::InputText(u8"������ ������Ʈ �̸� : ", m_szObjectName, MAX_PATH);

            //������Ʈ ����
            if (m_iBinaryObjectType == CObject_Manager::IMG)
            {
                //�Ϲ� �̹���
                if (ImGui::Button(u8"�̹��� ���� ����"))
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
                            MSG_BOX("�̹��� ���� ����");
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }
            }
            else if (m_iBinaryObjectType == CObject_Manager::BTN)
            {
                //��ư ����
                if (ImGui::Button(u8"�̹��� ����"))
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

                if (ImGui::Button(u8"Ŭ�� �̹��� ����"))
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

                if (ImGui::Button(u8"��ư ����"))
                {
                    CObject_Manager::OBJ_MNG_DESC Desc{};
                    Desc.strFilePath = m_pGameInstance->StringToWstring(m_strBtnFilePath);
                    Desc.strFileName = m_pGameInstance->StringToWstring(m_strBtnFileName);
                    Desc.strBtnClickFilePath = m_pGameInstance->StringToWstring(m_strBtnClickFilePath);
                    Desc.strBtnClickFileName = m_pGameInstance->StringToWstring(m_strBtnClickFileName);
                    Desc.strName = m_szObjectName;
                    Desc.iTextureType = m_iBinaryObjectType;

                    if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                        MSG_BOX("��ư ���� ����");
                }
            }
            else if (m_iBinaryObjectType == CObject_Manager::TEXT)
            {
                //�ؽ�Ʈ ����
                ImGui::InputText(u8"�ؽ�Ʈ �ۼ� : ", m_szText, MAX_PATH);

                if (ImGui::Button(u8"�ؽ�Ʈ ����"))
                {
                    //�ؽ��� ����
                    CObject_Manager::OBJ_MNG_DESC Desc{};
                    Desc.strText = m_pGameInstance->StringToWstring(string(m_szText));
                    Desc.iTextureType = m_iBinaryObjectType;
                    Desc.vColor = m_vColor;
                    Desc.strName = m_szObjectName;

                    if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                        MSG_BOX("�ؽ�Ʈ ������Ʈ ���� ����");
                }
            }
            else if (m_iBinaryObjectType == CObject_Manager::GROUP)
            {
                //�׷� ����
                if (ImGui::Button(u8"�׷� ����"))
                {
                    CObject_Manager::OBJ_MNG_DESC Desc{};
                    Desc.strName = m_szObjectName;
                    Desc.iTextureType = m_iBinaryObjectType;
                    Desc.vColor = m_vColor; 
                    Desc.strName = m_szObjectName;

                    if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                        MSG_BOX("���̳ʸ� ������Ʈ ���� ����");
                }
            }
            else if (m_iBinaryObjectType == CObject_Manager::EFFECT)
            {
                //����Ʈ ����
                if (ImGui::Button(u8"�̹��� ���� ����"))
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
                        Desc.iTextureType = m_iBinaryObjectType;    
                        Desc.strName = m_szObjectName;  

                        if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(strName), &Desc)))   
                            MSG_BOX("�̹��� ���� ����");
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }

            }
        }

        //������ ������Ʈ ����Ʈ
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


                        //m_StartUV = dynamic_cast<CUI_Texture*>(BinaryObject[n])->Get_StartUV();
                        //m_EndUV = dynamic_cast<CUI_Texture*>(BinaryObject[n])->Get_EndUV();

                        m_isOpenBinaryGroup = BinaryObject[m_iBinaryObjectIndex]->Get_TypeIndex() == CObject_Manager::GROUP;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            if (ImGui::Button("Remove"))
            {
                if (BinaryObject[m_iBinaryObjectIndex]->Get_TypeIndex() == CObject_Manager::GROUP)
                    m_isOpenBinaryGroup = false;

                if (FAILED(m_pObjectManager->Remove_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex)))
                    MSG_BOX("���̳ʸ� ������Ʈ ���� �Ұ�");

                if (BinaryObject.size() <= 1)
                    m_iBinaryObjectIndex = -1;

                auto BinaryObject = m_pObjectManager->Get_GroupBinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag));
            }

            //������ ���̳ʸ��� ���� ���� �ɼ� �߰� 
            if (m_iBinaryObjectIndex != -1 && m_iBinaryPick == BINARY_OBJECT)
            {

                ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");

                if (ImGui::IsKeyPressed(ImGuiKey_W))
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
                ImGuizmo::DecomposeMatrixToComponents((_float*)&m_WorldMatirx, matrixTranslation, matrixRotation, matrixScale);
                ImGui::InputFloat3("Tr", matrixTranslation);
                ImGui::InputFloat3("Rt", matrixRotation);
                ImGui::InputFloat3("Sc", matrixScale);
                ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (_float*)&m_WorldMatirx);

                BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatirx));

                //���̴� ����
                if (!m_isOpenBinaryGroup)
                {
                    _int iPass = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_ShaderPass();

                    if (ImGui::RadioButton("NonBlend", &iPass, 0))
                        dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Blend", &iPass, 1))
                        dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Alpha Blend", &iPass, 2))
                        dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Color Alpha Blend", &iPass, 4))
                        dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Color Alpha Blend Effect", &iPass, 5))
                        dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(iPass);

                    if (4 == iPass || 5 == iPass)
                    {
                        _float4 vColor = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_Color();
                        static bool alpha_preview = true;
                        static bool alpha_half_preview = false;
                        static bool drag_and_drop = true;
                        static bool options_menu = true;
                        static bool hdr = false;
                        ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

                        if (ImGui::ColorEdit4("MyColor##2f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
                        {
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_Color(vColor);
                        }
                    }

                    if (CObject_Manager::TEXT != dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_TypeIndex())
                    {
                        ImGui::DragFloat2("StartUV", (float*)&m_StartUV, 0.001f, 0.0f, 1.f);
                        ImGui::DragFloat2("EndUV", (float*)&m_EndUV, 0.001f, 0.0f, 1.f);

                        if (FAILED(dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Change_UV(m_StartUV, m_EndUV)))
                            MSG_BOX("UV ���� ����");
                    }

                    if (CObject_Manager::BTN == dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_TypeIndex())
                    {
                        ImGui::DragFloat2("ClickStartUV", (float*)&m_ClickStartUV, 0.001f, 0.0f, 1.f);
                        ImGui::DragFloat2("ClickEndUV", (float*)&m_ClickEndUV, 0.001f, 0.0f, 1.f);

                        if (FAILED(dynamic_cast<CBtn*>(BinaryObject[m_iBinaryObjectIndex])->Chage_ClickUV(m_ClickStartUV, m_ClickEndUV)))
                            MSG_BOX("ClickUV ���� ����");

                        _bool Click = dynamic_cast<CBtn*>(BinaryObject[m_iBinaryObjectIndex])->Get_Click(); 
                        if (ImGui::Checkbox("Click", &Click))
                        {
                            dynamic_cast<CBtn*>(BinaryObject[m_iBinaryObjectIndex])->Set_Click(Click);
                        }

                    }

                    if (CObject_Manager::EFFECT == dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_TypeIndex())
                    {
                        _float3 vLifeTime = dynamic_cast<CUI_Effect*>(BinaryObject[m_iBinaryObjectIndex])->Get_LifeTime();
                        _float fLife[2] = { vLifeTime.y, vLifeTime.z };
                        if (ImGui::DragFloat2("LifeTime", fLife, 0.001f, 0.0f, 10.f))
                        {
                            vLifeTime.y = fLife[0];
                            vLifeTime.z = fLife[1];
                            dynamic_cast<CUI_Effect*>(BinaryObject[m_iBinaryObjectIndex])->Set_LifeTime(vLifeTime);
                        }

                        _float fSpeed = dynamic_cast<CUI_Effect*>(BinaryObject[m_iBinaryObjectIndex])->Get_Speed();
                        if (ImGui::DragFloat("LifeSpeed", &fSpeed, 0.001f, 0.0f, 10.f))
                        {
                            dynamic_cast<CUI_Effect*>(BinaryObject[m_iBinaryObjectIndex])->Set_Speed(fSpeed);
                        }
                    }

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

    ImGui::Begin(u8"���̳ʸ� ������Ʈ �׷��");

    //����
    ImGui::Checkbox(u8"������Ʈ ����", &m_isCreateBinaryGroupObject);

    if (m_isCreateBinaryGroupObject)
    {
        auto BinaryObject = m_pObjectManager->Get_GroupBinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag));
        //���� �����ϴٸ�
        ImGui::SeparatorText(u8"������Ʈ ����");
        ImGui::RadioButton(u8"�̹���", &m_iBinaryGroupObjectType, CObject_Manager::IMG); ImGui::SameLine();
        ImGui::RadioButton(u8"��ư", &m_iBinaryGroupObjectType, CObject_Manager::BTN); ImGui::SameLine();
        ImGui::RadioButton(u8"�ؽ�Ʈ", &m_iBinaryGroupObjectType, CObject_Manager::TEXT); ImGui::SameLine();
        ImGui::RadioButton(u8"UI ����Ʈ", &m_iBinaryGroupObjectType, CObject_Manager::EFFECT);

        ImGui::NewLine();
        ImGui::InputText(u8"������ ������Ʈ �̸� : ", m_szBinaryGroupObjectName, MAX_PATH);

        //������Ʈ ����
        if (m_iBinaryGroupObjectType == CObject_Manager::IMG)
        {
            //�Ϲ� �̹���
            if (ImGui::Button(u8"�̹��� ���� ����"))
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
                    Desc.strName = m_szBinaryGroupObjectName;
                    Desc.isParent = true;
                    Desc.pParentMatrix = BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Get_WorldFloat4x4();
                    Desc.strParentName = BinaryObject[m_iBinaryObjectIndex]->Get_Name();

                    CImage_Texture* pImage = dynamic_cast<CImage_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &Desc));
                    dynamic_cast<CGroup*>(BinaryObject[m_iBinaryObjectIndex])->Set_PartObject(pImage);
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }
        }
        else if (m_iBinaryGroupObjectType == CObject_Manager::BTN)
        {
            //��ư ����
            if (ImGui::Button(u8"�̹��� ����"))
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

            if (ImGui::Button(u8"Ŭ�� �̹��� ����"))
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

            if (ImGui::Button(u8"��ư ����"))
            {
                CBtn::BTN_DESC Desc{};  
                Desc.strTextureFilePath = m_pGameInstance->StringToWstring(m_strBtnFilePath);
                Desc.strTextureFileName = m_pGameInstance->StringToWstring(m_strBtnFileName);
                Desc.strClickFilePath = m_pGameInstance->StringToWstring(m_strBtnClickFilePath);
                Desc.strClickFileName = m_pGameInstance->StringToWstring(m_strBtnClickFileName);
                Desc.strName = m_szBinaryGroupObjectName;   
                Desc.iTypeIndex = m_iBinaryGroupObjectType; 
                Desc.isParent = true;   
                Desc.pParentMatrix = BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Get_WorldFloat4x4();
                Desc.strParentName = BinaryObject[m_iBinaryObjectIndex]->Get_Name();
                CBtn* pBtn = dynamic_cast<CBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Btn"), &Desc));
                if (nullptr == pBtn)
                    MSG_BOX("��ư ���� ����");

                dynamic_cast<CGroup*>(BinaryObject[m_iBinaryObjectIndex])->Set_PartObject(pBtn);
            }
        }
        else if (m_iBinaryGroupObjectType == CObject_Manager::TEXT)
        {
            //�ؽ�Ʈ ����
            ImGui::InputText(u8"�ؽ�Ʈ �ۼ� : ", m_szText, MAX_PATH);

            //static ImVec4 vAmbientColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
            //ImGui::ColorEdit4(u8"vColor ����", (float*)&m_vColor, ImGuiColorEditFlags_Float);

            if (ImGui::Button(u8"�ؽ�Ʈ ����"))
            {
                //�ؽ��� ����
                CText::TEXT_DESC Desc{};
                Desc.strText = m_pGameInstance->StringToWstring(string(m_szText));
                Desc.iTypeIndex = m_iBinaryGroupObjectType;
                Desc.vColor = m_vColor; 
                Desc.strName = m_szBinaryGroupObjectName;
                Desc.isParent = true;
                Desc.pParentMatrix = BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Get_WorldFloat4x4();
                Desc.strParentName = BinaryObject[m_iBinaryObjectIndex]->Get_Name();
                CText* pText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &Desc));
                dynamic_cast<CGroup*>(BinaryObject[m_iBinaryObjectIndex])->Set_PartObject(pText);


            }
        }
        else if (m_iBinaryGroupObjectType == CObject_Manager::EFFECT)
        {
            //����Ʈ ����
            if (ImGui::Button(u8"����Ʈ ���� ����"))
            {
                IGFD::FileDialogConfig config;
                config.path = "../../Client/Bin/Resources/Textures/UI";
                ImGuiFileDialog::Instance()->OpenDialog("ChooseBinaryTextureKey", "Choose File", ".dds, .png", config);
            }
            if (ImGuiFileDialog::Instance()->Display("ChooseBinaryTextureKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                { // action if OK
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                    // action

                    CUI_Effect::UI_EFFECT_DESC Desc{};
                    Desc.strTextureFileName = m_pGameInstance->StringToWstring(fileName);
                    Desc.strTextureFilePath = m_pGameInstance->StringToWstring(filePathName);
                    Desc.iTypeIndex = m_iBinaryGroupObjectType;
                    Desc.strName = m_szBinaryGroupObjectName;
                    Desc.isParent = true;
                    Desc.pParentMatrix = BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Get_WorldFloat4x4();
                    Desc.strParentName = BinaryObject[m_iBinaryObjectIndex]->Get_Name();
                    CUI_Effect* pEffect = dynamic_cast<CUI_Effect*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIEffect"), &Desc));
                    dynamic_cast<CGroup*>(BinaryObject[m_iBinaryObjectIndex])->Set_PartObject(pEffect);
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }
        }
    }

    //������ ������Ʈ Ȯ�� ����
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
                 m_StartUV = dynamic_cast<CUI_Texture*>(Objects[n])->Get_StartUV();
                 m_EndUV = dynamic_cast<CUI_Texture*>(Objects[n])->Get_EndUV();
                 m_iBinaryPick = GROUP_OBJECT;
             }
             if (is_selected)
                 ImGui::SetItemDefaultFocus();
         }
         ImGui::EndListBox();
    }



    if (ImGui::Button("Group Object Remove"))
    {
        m_pObjectManager->Remove_GroupObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex, m_iBinaryGroupObjectIndex);
        m_iBinaryGroupObjectIndex = -1; // �ʱ�ȭ ��Ű��S
    }
    ImGui::SameLine();



    if (m_iBinaryGroupObjectIndex != -1 && m_iBinaryPick == GROUP_OBJECT)
    {
        //���̳ʸ� ����������
        ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");

        if (ImGui::IsKeyPressed(ImGuiKey_W))
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
        ImGuizmo::DecomposeMatrixToComponents((_float*)&m_WorldMatirx, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (_float*)&m_WorldMatirx);

        Objects[m_iBinaryGroupObjectIndex]->Get_TransformCom()->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatirx));


        _int iPass = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_ShaderPass();

        if (ImGui::RadioButton("NonBlend", &iPass, 0))
            dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(iPass);
        else if (ImGui::RadioButton("Blend", &iPass, 1))
            dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(iPass);
        else if (ImGui::RadioButton("Alpha Blend", &iPass, 2))
            dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(iPass);
        else if (ImGui::RadioButton("Color Alpha Blend", &iPass, 4))
            dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(iPass);
        else if (ImGui::RadioButton("Color Alpha Blend Effect", &iPass, 5))
            dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(iPass);

        if (4 == iPass|| 5 == iPass)
        {
            _float4 vColor = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_Color();
            static bool alpha_preview = true;
            static bool alpha_half_preview = false;
            static bool drag_and_drop = true;
            static bool options_menu = true;
            static bool hdr = false;
            ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

            if (ImGui::ColorEdit4("MyColor##2f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
            {
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_Color(vColor);
            }
        }

        if (CObject_Manager::TEXT != dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_TypeIndex())
        {
            ImGui::DragFloat2("StartUV", (float*)&m_StartUV, 0.001f, 0.0f, 1.f);
            ImGui::DragFloat2("EndUV", (float*)&m_EndUV, 0.001f, 0.0f, 1.f);

            if (FAILED(dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Change_UV(m_StartUV, m_EndUV)))
                MSG_BOX("UV ���� ����");
        }

        if (CObject_Manager::BTN == dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_TypeIndex())
        {
            ImGui::DragFloat2("ClickStartUV", (float*)&m_ClickStartUV, 0.001f, 0.0f, 1.f);
            ImGui::DragFloat2("ClickEndUV", (float*)&m_ClickEndUV, 0.001f, 0.0f, 1.f);

            if (FAILED(dynamic_cast<CBtn*>(Objects[m_iBinaryGroupObjectIndex])->Chage_ClickUV(m_ClickStartUV, m_ClickEndUV)))
                MSG_BOX("ClickUV ���� ����");
        }

        if(CObject_Manager::EFFECT == dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_TypeIndex())
        {
            _float3 vLifeTime = dynamic_cast<CUI_Effect*>(Objects[m_iBinaryGroupObjectIndex])->Get_LifeTime();
            _float fLife[2] = { vLifeTime.y, vLifeTime.z };
            if (ImGui::DragFloat2("LifeTime", fLife, 0.001f, 0.0f, 10.f))
            {
                vLifeTime.y = fLife[0];
                vLifeTime.z = fLife[1];
                dynamic_cast<CUI_Effect*>(Objects[m_iBinaryGroupObjectIndex])->Set_LifeTime(vLifeTime);
            }

            _float fSpeed = dynamic_cast<CUI_Effect*>(Objects[m_iBinaryGroupObjectIndex])->Get_Speed();
            if (ImGui::DragFloat("LifeSpeed", &fSpeed, 0.001f, 0.0f, 10.f))
            {
                dynamic_cast<CUI_Effect*>(Objects[m_iBinaryGroupObjectIndex])->Set_Speed(fSpeed);
            }
        }

    }

    ImGui::End();
}

void CImgui_Manager::Guizmo_Test()
{
    ImGuizmo::BeginFrame();

    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    float viewManipulateRight = g_iWinSizeX;
    float viewManipulateTop = 0;

    ImGuizmo::SetRect(0, 0, g_iWinSizeX, g_iWinSizeY);

    _matrix cameraView = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
    _matrix viewInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW); // ī�޶� ���� 
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