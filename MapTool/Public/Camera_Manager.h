#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "CameraObj.h"

class CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager);

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Initialize();
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Render();


private:
	CGameInstance*					m_pGameInstance;

private:
	vector<char*>					m_FileNames;
	vector<CCameraObj*>				m_CameraObjects;
	vector<char*>					m_CameraName;

private:
	_int							m_iCurrentCameraIndex = { 0 };
	_int							m_iCurrentCameraFile = { 0 };
	LIGHT_DESC						m_tCameraDesc;



public:
	virtual void Free() override;
};

