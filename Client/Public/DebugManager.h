#pragma once
#include "Base.h"

#include "Client_Defines.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

/*
1. 디버깅 용 툴.
*/
BEGIN(Client)
class CDebugManager :
    public CBase
{
    DECLARE_SINGLETON(CDebugManager)
private:
    CDebugManager();
    virtual ~CDebugManager() = default;

public:
    _bool isTimeStop() { return m_isTimeStop; }
    _bool isDebug() { return m_isDebug; }

public:
    void Set_Debug(_bool isDebug) { m_isDebug = isDebug; }

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Tick();
    HRESULT Render();

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    _bool m_isDebug = { false }; // 디버그 툴 진입했는지 아닌지 체크

private:
    _bool m_isTimeStop = { false }; // TimeDelta Stop

private:
    void Window_Debug();

public:
    static void Release_Debug();
    virtual void Free() override;
};
END
