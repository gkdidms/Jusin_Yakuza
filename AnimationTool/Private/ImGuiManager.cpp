#include "../../Imgui/imgui/imgui.h"
#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include "..\Default\framework.h"
#include "ImGuiManager.h"

#include "GameInstance.h"

#include "Layer.h"


CImGuiManager::CImGuiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{pDevice, pContext}
{
}

CImGuiManager::CImGuiManager(const CImGuiManager& rhs)
    :CGameObject{ rhs }
{
}

HRESULT CImGuiManager::Initialize_Prototype()
{
    //Setup ImGui
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    ImGui::StyleColorsDark();

    return S_OK;
}

HRESULT CImGuiManager::Initialize(void* pArg)
{
    m_bIsClone = true;

    return S_OK;
}

HRESULT CImGuiManager::Render()
{
    Render_Begin();
    Update();
    Render_End();

    return S_OK;
}

void CImGuiManager::Render_Begin()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void CImGuiManager::Render_End()
{
}

void CImGuiManager::Update()
{
}

void CImGuiManager::Late_Tick(_float fTimeDelta)
{
    m_fTimeDelta = fTimeDelta;
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_UI, this);
}

CImGuiManager* CImGuiManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CImGuiManager* pInstance = new CImGuiManager(pDevice, pContext);
    
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CImGuiManager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CImGuiManager::Clone(void* pArg)
{
    CImGuiManager* pInstance = new CImGuiManager(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Create Cloned : CImGuiManager");
        Safe_Release(pInstance);
    }

    return pInstance;

}

void CImGuiManager::Free()
{
    __super::Free();
    
    // 클론이 아닐 때 ImGui 이닛했기때문에 
    if (!m_bIsClone)
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}
