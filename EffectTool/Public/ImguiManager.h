#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Effect.h"

#pragma region "Imgui"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"
#include "ImCurveEdit.h"
#pragma endregion

BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CVIBuffer_Instance;
END

BEGIN(Client)
class CImguiManager final:
    public CBase
{
public:
	enum PASS{ 
		PASS_DIRECTION=2 , 
		PASS_NODIRECTION ,
		PASS_DIRECTIONCOLOR , 
		PASS_WEIGHTBLEND,
		PASS_END};
private:
	CImguiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImguiManager() = default;

public:
	HRESULT Initialize(void* pArg=nullptr) ;
	void Tick(const _float& fTimeDelta) ;
	HRESULT Render() ;

private:
	void EditTransform(_float* cameraView, _float* cameraProjection, _float* matrix);
	void Guizmo(_float fTimeDelta);


	HRESULT Create_Particle();
	HRESULT Create_Trail();


	HRESULT Edit_Particle(_uint Index);
	HRESULT Load_Desc(_uint Index);

	void Create_Tick(_float fTimeDelta);
	void Editor_Tick(_float fTimeDelta);
	void Guizmo_Tick(_float fTimeDelta);
	void Timeline_Tick(_float fTimeDelta);

	bool IsMouseHoveringCircle(float mouse_x, float mouse_y, float circle_x, float circle_y, float radius);
	void UpdateCirclePosition();

	void Color_Palette();
	void Reset_Particle();
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	//파티클 액션에 대한 bool 값.
	_bool m_bSpread = { false };
	_bool m_bDrop = { false };
	_bool m_bGuizmo = { false };
	//생성 파티클 담는 곳
	vector<CGameObject*> m_EditParticle = {  };
	CEffect::EFFECT_DESC m_EffectDesc = {};	

	bool m_bDragging = { false };

	_int m_iCurEditIndex = { -1 };
	char text_input_buffer[256] = { "" };

	_float m_fParticleTime = { 0.f };
	_float m_fMaxTime = { 30.f };

	public:
	static CImguiManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END
