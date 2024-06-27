#include "DebugManager.h"

#include "GameInstance.h"
#include "FileTotalMgr.h"

#include "DebugCamera.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CDebugManager)

CDebugManager::CDebugManager()
    : m_pGameInstance { CGameInstance::GetInstance() },
    m_pFileTotalMgr{ CFileTotalMgr::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pFileTotalMgr);
}

HRESULT CDebugManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);


    //초기화
    m_fHDRLight = m_pGameInstance->Get_HDRLight();

    return S_OK;
}

void CDebugManager::Tick()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    /*_bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);*/

    Window_Debug();
}

HRESULT CDebugManager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void CDebugManager::Window_Debug()
{
    ImGui::Begin("Debug");

    if (ImGui::CollapsingHeader("Player"))
    {
        //Time 제어
        ImGui::SeparatorText("Time");

        ImGui::Checkbox("TimeDelta Stop", &m_isTimeStop);

        if (ImGui::InputFloat("TimeSpeed", &m_fSpeed))
            m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fSpeed);

        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

        _bool isAnimStart = pPlayer->isAnimStart();
        if (ImGui::Checkbox("Anim Start", &isAnimStart))
            pPlayer->Set_AnimStart(isAnimStart);

        //Camera 제어
        ImGui::SeparatorText("Camera");

        CDebugCamera* pCamera = dynamic_cast<CDebugCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), 0));
        _float fSensor = pCamera->Get_Sensor();
        if (ImGui::InputFloat("CameraSensor", &fSensor))
            pCamera->Set_Sensor(fSensor);

        CTransform* pCameraTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(TEXT("Com_Transform")));

        _float fSpeed = pCameraTransform->Get_Speed();
        if (ImGui::InputFloat("CameraSpeed", &fSpeed))
            pCameraTransform->Set_Speed(fSpeed);
    }

    if (ImGui::CollapsingHeader("Deferred Shader"))
    {
        ImGui::SeparatorText("HDR");

        if (ImGui::InputFloat("HDR Light", &m_fHDRLight, 0.f))
            m_pGameInstance->Set_HDRLight(m_fHDRLight);

        ImGui::SeparatorText("SSAO");
        _float fRadiuse = m_pGameInstance->Get_SSAORadiuse();

        if (ImGui::SliderFloat("Radiuse", &fRadiuse, 0.001f, 1.f))
            m_pGameInstance->Set_SSAORadiuse(fRadiuse);

        _float fBlur = m_pGameInstance->Get_SSAOBlur();
        if (ImGui::SliderFloat("Blur", &fBlur, 0.1f, 10.f))
            m_pGameInstance->Set_SSAOBlur(fBlur);

        _float fBias = m_pGameInstance->Get_SSAOBias();
        if (ImGui::SliderFloat("Bias", &fBias, 0.0001f, 1.f))
            m_pGameInstance->Set_SSAOBias(fBias);
    }

    if (ImGui::CollapsingHeader("Light"))
    {
        ImGui::SeparatorText("Light");

        if (ImGui::InputInt("Light Number", &m_iLightPass, 0))
            m_pFileTotalMgr->Set_Lights_In_Client(m_iLightPass);
    }

    ImGui::End();
}

void CDebugManager::Release_Debug()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CDebugManager::DestroyInstance();
}

void CDebugManager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pFileTotalMgr);
}
