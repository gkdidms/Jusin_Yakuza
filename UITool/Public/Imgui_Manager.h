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
	void Window_Binary();

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
	//렌더타겟용
	_char m_szGroupName[MAX_PATH] = { "" };
	_int m_iGroupIndex = { -1 };
	_int m_iGroupObjectIndex = { -1 };
	string m_strSelectTag = "";

	//바이너리용
	_int m_iBinaryGroupIndex = { -1 };
	_int m_iBinaryObjectIndex = { -1 };
	string m_strBinarySelectTag = { "" };
	_bool m_isCreateBinaryObject = { false };
	_char m_szObjectName[MAX_PATH] = { "" };

	//버튼
	string m_strBtnFilePath = { "" };
	string m_strBtnFileName = { "" };
	string m_strBtnClickFilePath = { "" };
	string m_strBtnClickFileName = { "" };

	//텍스트 포맷 (숫자)
	_int m_iNum = { 0 };

private:
	_int m_iObjectType = { 0 };
	_int m_iBinaryObjectType = { 0 };
	_char m_szText[MAX_PATH] = { "" };
	_float4 m_vColor = {};
	_bool m_isBinaryPick = { false };

public:
	static void Release_Imgui();
	virtual void Free() override;

};

END