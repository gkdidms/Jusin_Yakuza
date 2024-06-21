#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "imgui.h"
#include "Navigation_Manager.h"
#include "ObjPlace_Manager.h"

BEGIN(Engine)
class CGameInstance;
END

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
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


private:
	/* Terrain�� ���� ���� */
	void	Set_Terrain_IMGUI();
	/* ��ġ�� Object �����ϰԲ� */
	void	Show_Object_List();
	/* Navi Tool ������ */
	void	Set_NaviTool_IMGUI();
	void	Show_MapObj_Place_IMGUI();

private:
	CNavigation_Manager* m_pNavigationMgr;
	CObjPlace_Manager* m_pObjPlace_Manager;
	CGameInstance* m_pGameInstance;

	/* imgui â ���� bool ���� */
private:
	_bool				m_bMain_IMGUI = true;
	_bool				m_bTerrain_IMGUI = false;
	_bool				m_bNaviTool_IMGUI = false; /* �׺���̼� �� ȭ�� */
	_bool				m_bObject_Place_IMGUI = false;




public:
	virtual void Free() override;

};

