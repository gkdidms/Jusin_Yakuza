#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "imgui.h"

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
	CGameInstance*				m_pGameInstance;

public:
	virtual void Free() override;

};

