#pragma once
#include "Base.h"

#include "Client_Defines.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

BEGIN(Client)
class CDebugManager :
    public CBase
{
    DECLARE_SINGLETON(CDebugManager)
private:
    CDebugManager();
    virtual ~CDebugManager() = default;

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Tick();
    HRESULT Render();

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

private:
    void Window_Debug();

public:
    static void Release_Debug();
    virtual void Free() override;
};
END
