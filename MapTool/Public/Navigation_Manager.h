#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Cell.h"
#include "Navigation.h"
#include "Shader.h"

class CNavigation_Manager final : public CBase
{
	DECLARE_SINGLETON(CNavigation_Manager);

private:
	CNavigation_Manager();
	virtual ~CNavigation_Manager() = default;

public:
	HRESULT					Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void					Priority_Tick(_float fTimeDelta);
	void					Tick(_float fTimeDelta);
	void					Late_Tick(_float fTimeDelta);
	HRESULT					Render();



private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CShader* m_pShader = nullptr;



public:
	virtual void Free() override;
};

