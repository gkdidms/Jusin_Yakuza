#pragma once
#include "Client_Defines.h"
#include "Base.h"

#pragma region "Imgui"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"
#pragma endregion


#define V2 ImVec2

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
	void KeyFrameWindow();
	void MeshListWindow();
	void FXWindow(ImGuiIO& io);
	void FX(ImDrawList* d, V2 a, V2 b, V2 s, ImVec4 m, float t);

	void DrawTimeline(ImDrawList* d, const vector<float>& data);
	void DrawChannels();

private:
	void LoadAnimationCharacterList();
	void Connect_Model_Ref();

private:
	void Update_Model_RotationX();
	void Update_Model_RotationY();
	void Update_Model_RotationZ();
	void Update_Model_Scaled();

private:
	_bool					m_isAnimListWindow = { false };
	int						m_iAnimIndex = { 0 };
	int						m_iModelSelectedIndex = { 0 };
	int						m_iBoneSelectedIndex = { 0 };
	int						m_iChannelSelectedIndex = { 0 };
	int						m_iMeshSelectedIndex = { 0 };

	vector<string>			m_ModelNameList;
	vector<string>			m_AnimNameList;
	vector<string>			m_BoneNameList;
	vector<string>			m_ChannelNameList;
	vector<string>			m_MeshNameList;

private:
	_float					m_fTimeDelta = { 0.f };


private:
	float					m_ModelPosition[3] = { 0.f };
	float					m_ModelRotation[3] = { 0.f };
	float					m_ModelScale = { 0.f };

	bool					m_isPause = { false };

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
