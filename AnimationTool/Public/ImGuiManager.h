#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CImguiManager final :
	public CBase
{
private:
	CImguiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImguiManager() = default;

public:
	HRESULT Initialize(void* pArg = nullptr);
	void Tick(const _float& fTimeDelta);
	HRESULT Render();

private:
	void ModelList();
	void AnimListWindow();
	void BoneListWindow();

private:
	void LoadAnimationCharacterList();
	void Connect_Model_Ref();

private:
	void Update_Model_RotationX();
	void Update_Model_RotationY();
	void Update_Model_RotationZ();
	void Update_Model_Scaled();

private:
	_bool m_isAnimListWindow = { false };
	int m_iModelSelectedIndex = { 0 };
	int m_iBoneSelectedIndex = { 0 };

	vector<string> m_ModelNameList;
	vector<string> m_AnimNameList;
	vector<string> m_BoneNameList;

private:
	_float				m_fTimeDelta = { 0.f };

	int					m_iAnimIndex = { 0 };

private:
	float				m_ModelPosition[3] = { 0.f };
	float				m_ModelRotation[3] = { 0.f };
	float				m_ModelScale = { 0.f };

	bool				m_isPause = { false };

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	class CAnimModel*		m_pRenderModel = { nullptr };

public:
	static CImguiManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END
