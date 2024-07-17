#pragma once

#include "Base.h"
#include "imgui.h"
#include "TriggerObj.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

class CTrigger_Manager : public CBase
{
public:
	enum TRIGGER_TYPE {
		TRIGGER_MOVE_LEVEL,
		TRIGGER_CINEMACHINE,
		TRIGGER_YONEDA,
		TRIGGER_END
	};

	DECLARE_SINGLETON(CTrigger_Manager);

private:
	CTrigger_Manager();
	virtual ~CTrigger_Manager() = default;


public:
	HRESULT								Initialize();
	void								Priority_Tick(_float fTimeDelta);
	void								Tick(_float fTimeDelta);
	void								Late_Tick(_float fTimeDelta);
	void								Render();

public:
	void								Set_TriggerObj_Render(_bool bRender) { m_bRender = bRender; }
	void								Show_FileName();
	void								Show_Add_Trigger_IMGUI();


private:
	void								Update_TriggerObjName();
	void								Edit_Trigger();

	void								Edit_GameObject_Transform(int iNumObject);
	void								Edit_Installed_Trigger();
	void								Click_To_Select_Trigger(int& iObjectNum);

private:
	CGameInstance*						m_pGameInstance;

private:
	vector<char*>						m_FileNames;
	vector<char*>						m_TriggerNames;
	vector<CTriggerObj*>				m_TriggerObjects;

private:
	_int								m_iCurrentTriggerIndex = { 0 };
	_int								m_iCurrentTriggerFile = { 0 };
	LIGHT_DESC							m_tCurTriggerDesc;

	bool								m_bRender = { false };
	bool								m_bAddTrigger = { false };


public:
	virtual void Free() override;
};

