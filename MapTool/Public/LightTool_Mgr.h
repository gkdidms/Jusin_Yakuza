#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "LightObj.h"

class CLightTool_Mgr final : public CBase
{
	DECLARE_SINGLETON(CLightTool_Mgr);

private:
	CLightTool_Mgr();
	virtual ~CLightTool_Mgr() = default;

public:
	HRESULT Initialize();
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Render();

public:
	void							Show_LightMgr_IMGUI();
	void							Show_FileName();

private:
	void							Update_FileName();

	void							Select_LightDesc();
	bool							Add_Light_Imgui();


private:
	CGameInstance*					m_pGameInstance;

private:
	vector<char*>					m_FileNames;
	vector<CLightObj*>				m_LightObjects;
	vector<char*>					m_LightsName;

private:
	_int							m_iCurrentLightIndex = { 0 };
	_int							m_iCurrentLightFile = { 0 };
	LIGHT_DESC						m_tCurLightDesc;

	bool							m_bLightAdd = { false };

public:
	virtual void Free() override;
};

