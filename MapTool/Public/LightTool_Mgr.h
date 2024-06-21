#pragma once

#include "Base.h"
#include "Client_Defines.h"

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

private:
	CGameInstance*				m_pGameInstance;

public:
	virtual void Free() override;
};

