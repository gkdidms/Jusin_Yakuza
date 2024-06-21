#include "DebugManager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CDebugManager)

CDebugManager::CDebugManager()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
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


    //ÃÊ±âÈ­
    m_isHDR = m_pGameInstance->Get_HDR();

    return S_OK;
}

void CDebugManager::Tick()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    _bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

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
    ImGui::Begin("Window");

    ImGui::Checkbox("TimeDelta Stop", &m_isTimeStop);
    
    if (ImGui::InputFloat("TimeSpeed", &m_fSpeed))
    {
        m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fSpeed);
    }

    if (ImGui::Checkbox("HDR", &m_isHDR))
    {
        m_pGameInstance->Set_HDR(m_isHDR);
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
}
