#ifdef _DEBUG




#include "DebugManager.h"

#include "GameInstance.h"
#include "FileTotalMgr.h"
#include "SystemManager.h"
#include "UIManager.h"

#include "DebugCamera.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CDebugManager)

CDebugManager::CDebugManager()
    : m_pGameInstance { CGameInstance::GetInstance() },
    m_pFileTotalMgr{ CFileTotalMgr::GetInstance() },
    m_pSystemManager{ CSystemManager::GetInstance() },
    m_pUIManager { CUIManager::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pFileTotalMgr);
    Safe_AddRef(m_pSystemManager);
    Safe_AddRef(m_pUIManager);
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

        ImGui::Checkbox("Limit", &m_isLimit);
        ImGui::Checkbox("TimeDelta Stop", &m_isTimeStop);

        if (ImGui::InputFloat("TimeSpeed", &m_fSpeed))
            m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fSpeed);

        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

        _bool isAnimStart = pPlayer->isAnimStart();
        if (ImGui::Checkbox("Anim Start", &isAnimStart))
            pPlayer->Set_AnimStart(isAnimStart);

        ImGui::NewLine();

        ImGui::Checkbox("Player Pos Move (Picking)", &m_isPlayerMove);
        ImGui::Text("! Mouse RB Click !");
        if (m_isPlayerMove)
        {
            if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
            {
                _bool isPicking = { false };
                _vector vMovePos = m_pGameInstance->Picking(&isPicking);

                if (isPicking)
                {
                    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
                    pPlayer->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vMovePos);
                }
            }
        }

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

        //Camera 제어
        ImGui::SeparatorText("Render");

        _bool isRender = pPlayer->Get_ObjectRender();
        if (ImGui::Checkbox("PlayerRender", &isRender))
            pPlayer->Set_ObjectRender(isRender);

    }

    if (ImGui::CollapsingHeader("Deferred Shader"))
    {
        ImGui::SeparatorText("Shader ON/OFF");
        _bool isHDR = m_pGameInstance->isHDR();
        if (ImGui::Checkbox("HDR", &isHDR))
            m_pGameInstance->Set_HDR(isHDR);

        _bool isSSAO = m_pGameInstance->isSSAO();
        if (ImGui::Checkbox("SSAO", &isSSAO))
            m_pGameInstance->Set_SSAO(isSSAO);

        _bool isPBR = m_pGameInstance->isPBR();
        if (ImGui::Checkbox("PBR", &isPBR))
            m_pGameInstance->Set_PBR((isPBR));

        _bool isBOF = m_pGameInstance->isBOF();
        if (ImGui::Checkbox("BOF", &isBOF))
            m_pGameInstance->Set_BOF(isBOF);

        _bool isShadow = m_pGameInstance->isShadow();
        if (ImGui::Checkbox("Shadow", &isShadow))
            m_pGameInstance->Set_Shadow(isShadow);
        
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

        //그림자 제어
        ImGui::NewLine();
        ImGui::SeparatorText("Shadow");
        _vector vShadowPos = m_pSystemManager->Get_ShadowViewPos();

        if (ImGui::InputFloat4("ShadowViewPos", (_float*)&vShadowPos))
        {
            m_pSystemManager->Set_ShadowViewPos(vShadowPos);
            m_pGameInstance->Set_ShadowViewPos(vShadowPos);
        }
    }

    if (ImGui::CollapsingHeader("Light"))
    {
        ImGui::SeparatorText("Light");

        if (ImGui::InputInt("Light Number", &m_iLightPass, 0))
            m_pFileTotalMgr->Set_Lights_In_Client(m_iLightPass);
    }

    if (ImGui::CollapsingHeader("Particle / UI"))
    {
        ImGui::SeparatorText("Particle");
        if (ImGui::Button("pang"))
        {
            if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_Particle_Point_Hit1_Part0"), TEXT("Layer_Particle"), nullptr)))
                MSG_BOX("pang!");
        }

        ImGui::SeparatorText("UI");
        _bool isRender = m_pUIManager->Get_Render();
        if (ImGui::Checkbox("Render ON/OFF", &isRender))
            m_pUIManager->Set_Render(isRender);
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
    Safe_Release(m_pSystemManager);

    Safe_Release(m_pUIManager);
}

#endif // _DEBUG