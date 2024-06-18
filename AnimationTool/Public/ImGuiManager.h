#pragma once
#include "AnimationTool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CImGuiManager : public CGameObject
{
private:
	CImGuiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CImGuiManager(const CImGuiManager& rhs);
	virtual ~CImGuiManager() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Render();

private:
	void Render_Begin();
	void Render_End();

	virtual void Update();
	virtual void Late_Tick(_float fTimeDelta);

private:
	_bool								m_bIsClone = false;

//Preferences
private:
	_float								m_fTimeDelta = { 0 };

public:
	static CImGuiManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END