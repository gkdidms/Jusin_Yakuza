#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "imgui.h"
#include "Navigation_Manager.h"

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
	/* Object ���� ���� */
	void	Set_Map_Object();
	/* ��ġ�� Object �����ϰԲ� */
	void	Show_Object_List();

	/* Navi Tool ������ */
	void	Set_NaviTool_IMGUI();



private:
	CNavigation_Manager*			m_pNavigationMgr;

	/* imgui â ���� bool ���� */
private:
	_bool				m_bMain_IMGUI = true;
	_bool				m_bTerrain_IMGUI = false;
	_bool				m_bObject_Place_IMGUI = false;
	_bool				m_bNaviTool_IMGUI = false; /* �׺���̼� �� ȭ�� */


	/* ��ġ���� bool ���� */
private:
	_bool				m_bDoing_Place_Object = false;


public:
	virtual void Free() override;

};

