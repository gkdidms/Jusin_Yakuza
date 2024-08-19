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
public:
	enum WORLD_TYPE { RENDER_TARGET, BINARY_OBJECT, GROUP_OBJECT, WORLD_END };
	enum FONT{NANUM24,NANUM28,NANUM30,NANUM36,FONT_END};
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
	void Window_Binary_Group();

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
	_float2 m_StartUV = {  };
	_float2 m_EndUV = {};	
	_float2 m_ClickStartUV = {};
	_float2 m_ClickEndUV = {};
	_float4 m_ClickColor = {};
	_float4 m_UpPoint = {};
	_float4 m_DownPoint = {};


	_float4x4						m_ViewMatrix, m_ProjMatrix;

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:
	_uint m_iWinSizeX = { 1280 };
	_uint m_iWinSizeY = { 720 };

private:

	//����Ÿ�ٿ�
	_char m_szGroupName[MAX_PATH] = { "" };
	_int m_iGroupIndex = { -1 };
	_int m_iGroupObjectIndex = { -1 };
	string m_strSelectTag = "";

	//���̳ʸ���
	_int m_iBinaryGroupIndex = { -1 };
	_int m_iBinaryObjectIndex = { -1 };
	string m_strBinarySelectTag = { "" };
	_bool m_isCreateBinaryObject = { false };
	_char m_szObjectName[MAX_PATH] = { "" };

	//��ư
	string m_strBtnFilePath = { "" };
	string m_strBtnFileName = { "" };
	string m_strBtnClickFilePath = { "" };
	string m_strBtnClickFileName = { "" };

	//�ؽ�Ʈ ���� (����)
	_int m_iNum = { 0 };

	//���̳ʸ� ������Ʈ �� �׷�
	_bool m_isOpenBinaryGroup = { false };
	_bool m_isCreateBinaryGroupObject = { false };
	_int m_iBinaryGroupObjectIndex = { -1 };
	_int m_iBinaryGroupObjectType = { -1 };
	_char m_szBinaryGroupObjectName[MAX_PATH] = { "" };

	//�̹��� �۾� ������Ʈ �̸���
	_char m_szImageName[MAX_PATH] = { "" };
private:
	_int m_iObjectType = { 0 };
	_int m_iBinaryObjectType = { 0 };
	_char m_szText[MAX_PATH] = { "" };
	_float4 m_vColor = {};
	_uint m_iBinaryPick = { WORLD_END };

	_bool m_isCircle = { false };
	vector<wstring> m_Font;
	_int m_FontIndex = { 0 };
	//���ϰ��
	filesystem::path m_RootDir;
public:
	static void Release_Imgui();
	virtual void Free() override;

};

END