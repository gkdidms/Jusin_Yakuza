#pragma once

#include "Base.h"

#include "imgui.h"
#include "UITool_Defines.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(UITool)
class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager);

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Render();

private:
	void Window_Image();

private:
	void Guizmo_Test();
	HRESULT Create_Texture();
	
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	class CObject_Manager* m_pObjectManager = { nullptr };

private:
	ImGuizmo::OPERATION mCurrentGizmoOperation = { ImGuizmo::TRANSLATE };
	ImGuizmo::MODE mCurrentGizmoMode = { ImGuizmo::WORLD };
	_float4x4 m_WorldMatirx = {};
	_float4x4						m_ViewMatrix, m_ProjMatrix;

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:
	_uint m_iWinSizeX = { 1280 };
	_uint m_iWinSizeY = { 720 };

private:
	_char m_szGroupName[MAX_PATH] = { "" };
	_int m_iGroupIndex = { -1 };
	_int m_iGroupObjectIndex = { -1 };
	string m_strSelectTag = "";

public:
	static void Release_Imgui();
	virtual void Free() override;

};

END