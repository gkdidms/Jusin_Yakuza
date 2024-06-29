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
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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
    ImGui::Begin(u8"����Ÿ�� �̹��� ����");

    if (ImGui::CollapsingHeader(u8"�׷� / ����Ÿ�� �̹��� ����"))
    {
        //1.�׷��� �����
        auto Objects = m_pObjectManager->Get_Objects();

        ImGui::InputText(u8"������ Group �̸�", m_szGroupName, MAX_PATH);
        if (ImGui::Button(u8"Group ����"))
        {
            if (FAILED(m_pObjectManager->Add_Group(m_pGameInstance->StringToWstring(string(m_szGroupName)))))
            {
                MSG_BOX("Group ���� ���� / �̹� �����ϴ� �׷��Դϴ�.");
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

            ImGui::RadioButton(u8"�̹��� ����", &m_iObjectType, 0); ImGui::SameLine();
            ImGui::RadioButton(u8"�ؽ�Ʈ ����", &m_iObjectType, 2);

            if (m_iObjectType == 0)
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

                        CObject_Manager::OBJ_MNG_DESC Desc{};
                        Desc.strFileName = m_pGameInstance->StringToWstring(fileName);
                        Desc.strFilePath = m_pGameInstance->StringToWstring(filePathName);
                        Desc.iTextureType = CObject_Manager::IMG;

                        if (FAILED(m_pObjectManager->Add_RenderTextureObject(m_pGameInstance->StringToWstring(strName), &Desc)))
                            MSG_BOX("�̹��� ���� ����");
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }
            }
            else if (m_iObjectType == 2)
            {
                ImGui::InputText(u8"�ؽ�Ʈ �ۼ� : ", m_szText, MAX_PATH);

                static ImVec4 vAmbientColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
                ImGui::ColorEdit4(u8"Color ����", (float*)&m_vColor, ImGuiColorEditFlags_Float);

                if (ImGui::Button("Create"))
                {
                    //�ؽ��� ����
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
                        m_iGroupObjectIndex = -1; // �ʱ�ȭ ��Ű��
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Up"))
                    {
                        if (FAILED(m_pObjectManager->Move_ObjectIndex(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex, CObject_Manager::UP)))
                            MSG_BOX("�̵� ����");
                        else
                            m_iGroupObjectIndex++;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Down"))
                    {
                        if (FAILED(m_pObjectManager->Move_ObjectIndex(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex, CObject_Manager::DOWN)))
                            MSG_BOX("�̵� ����");
                        else
                            m_iGroupObjectIndex--;
                    }

                    Textures = m_pObjectManager->Get_GroupObject(m_pGameInstance->StringToWstring(m_strSelectTag)); // ���� �ٽ� �ҷ�����
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


                    //���̴� ����
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
        if (ImGui::Button(u8"����Ÿ�� �̹��� ����"))
        {
            wstring strFilePath = TEXT("../../Client/Bin/Resources/Textures/UI/CreateFile/") + m_pGameInstance->StringToWstring(m_strSelectTag) + TEXT(".dds");
            if (FAILED(m_pObjectManager->Create_Texture(m_iGroupIndex, strFilePath)))
            {
                MSG_BOX("����Ÿ�� �̹��� ���� ����");
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
        if (m_isCreateBinaryObject)
        {
            ImGui::SeparatorText(u8"������Ʈ ����");
            ImGui::RadioButton(u8"�̹���", &m_iBinaryObjectType, 0); ImGui::SameLine();
            ImGui::RadioButton(u8"��ư", &m_iBinaryObjectType, 1); ImGui::SameLine();
            ImGui::RadioButton(u8"�ؽ�Ʈ", &m_iBinaryObjectType, 2); ImGui::SameLine();
            ImGui::RadioButton(u8"�ؽ�Ʈ ����", &m_iBinaryObjectType, 3); ImGui::SameLine();
            ImGui::RadioButton(u8"�׷�", &m_iBinaryObjectType, 4);

            ImGui::InputText(u8"������ ������Ʈ �̸� : ", m_szObjectName, MAX_PATH);

            //������Ʈ ����
            if (m_iBinaryObjectType == 0)
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
            else if (m_iBinaryObjectType == 1)
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
            else if (m_iBinaryObjectType == 2 || m_iBinaryObjectType == 3)
            {
                //�ؽ�Ʈ ����
                ImGui::InputText(u8"�ؽ�Ʈ �ۼ� : ", m_szText, MAX_PATH);

                static ImVec4 vAmbientColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
                ImGui::ColorEdit4(u8"vColor ����", (float*)&m_vColor, ImGuiColorEditFlags_Float);

                if (ImGui::Button(u8"�ؽ�Ʈ ����"))
                {
                    //�ؽ��� ����
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
                //�׷� ����
                if (ImGui::Button(u8"�׷� ����"))
                {
                    CObject_Manager::OBJ_MNG_DESC Desc{};
                    Desc.strName = m_szObjectName;
                    Desc.iTextureType = m_iBinaryObjectType;

                    if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                        MSG_BOX("���̳ʸ� ������Ʈ ���� ����");
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
                    MSG_BOX("���̳ʸ� ������Ʈ ���� �Ұ�");

                if (BinaryObject.size() <= 1)
                    m_iBinaryObjectIndex = -1;

                auto BinaryObject = m_pObjectManager->Get_GroupBinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag));
            }

            //������ ���̳ʸ��� ���� ���� �ɼ� �߰� 
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
        ImGui::RadioButton(u8"�̹���", &m_iBinaryGroupObjectType, 0); ImGui::SameLine();
        ImGui::RadioButton(u8"��ư", &m_iBinaryGroupObjectType, 1); ImGui::SameLine();
        ImGui::RadioButton(u8"�ؽ�Ʈ", &m_iBinaryGroupObjectType, 2); ImGui::SameLine();
        ImGui::RadioButton(u8"�ؽ�Ʈ ����", &m_iBinaryGroupObjectType, 3); ImGui::SameLine();

        ImGui::InputText(u8"������ ������Ʈ �̸� : ", m_szBinaryGroupObjectName, MAX_PATH);

        //������Ʈ ����
        if (m_iBinaryGroupObjectType == 0)
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
            //�ؽ�Ʈ ����
            ImGui::InputText(u8"�ؽ�Ʈ �ۼ� : ", m_szText, MAX_PATH);

            static ImVec4 vAmbientColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
            ImGui::ColorEdit4(u8"vColor ����", (float*)&m_vColor, ImGuiColorEditFlags_Float);

            if (ImGui::Button(u8"�ؽ�Ʈ ����"))
            {
                //�ؽ��� ����
                CObject_Manager::OBJ_MNG_DESC Desc{};
                Desc.strText = m_pGameInstance->StringToWstring(string(m_szText));
                Desc.iTextureType = m_iBinaryGroupObjectType;
                Desc.vColor = XMLoadFloat4(&m_vColor);
                Desc.strName = m_szObjectName;
            }
        }
        else if (m_iBinaryGroupObjectType == 4)
        {
            //�׷� ����
            if (ImGui::Button(u8"�׷� ����"))
            {
                CObject_Manager::OBJ_MNG_DESC Desc{};
                Desc.strName = m_szObjectName;
                Desc.iTextureType = m_iBinaryGroupObjectType;

                if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                    MSG_BOX("���̳ʸ� ������Ʈ ���� ����");
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
                m_iBinaryPick = GROUP_OBJECT;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    if (m_iBinaryGroupObjectIndex != -1 && m_iBinaryPick == GROUP_OBJECT)
    {
        //���̳ʸ� ����������
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