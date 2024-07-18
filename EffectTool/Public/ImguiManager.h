#pragma once

#include "Base.h"
#include "Effect.h"
#include "Particle_Point.h"
#include "TRailEffect.h"
#include "Aura.h"
#include "Client_Defines.h"

#pragma region "Imgui"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"
#include "ImCurveEdit.h"
#include "ImGuiFileDialog.h"
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
	enum MODE {
		MODE_PARTICLE,
		MODE_TRAIL,
		MODE_AURA,
		MODE_END
	};
	enum PASS{ 
		PASS_NOCOLOR,
		PASS_COLOR,
		PASS_ROTATE,
		PASS_ROTANIM,
		PASS_ANIM,
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
	

	//�����Լ�
	HRESULT Mode_Select(_float fTimeDelta);
	HRESULT Edit_Particle(_uint Index);
	HRESULT Load_Desc(_uint Index);
	void File_Selctor(_bool* bChange);
	void Load_Selctor();
	void Guizmo_Tick(_float fTimeDelta);

	//��ƼŬ �Լ�
	HRESULT Create_Particle();
	void Create_Tick(_float fTimeDelta);
	void Editor_Tick(_float fTimeDelta);
	void Timeline_Tick(_float fTimeDelta);

	bool IsMouseHoveringCircle(float mouse_x, float mouse_y, float circle_x, float circle_y, float radius);
	void UpdateCirclePosition();

	void Color_Palette();
	void Reset_Particle();

	//Ʈ���� �Լ�
	HRESULT Create_Trail();

	void CreateTrail_Tick(_float fTimeDelta);
	void EditorTrail_Tick(_float fTimeDelta);


	//�ƿ�� �Լ�
	HRESULT Create_Aura();

	void CreateAura_Tick(_float fTimeDelta);
	void EditorAura_Tick(_float fTimeDelta);




	HRESULT AllEffect_Save();
	HRESULT AllEffect_Load();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	//���
	_uint m_iMode = { MODE_END };
	//��ƼŬ �׼ǿ� ���� bool ��.
	_bool m_bSpread = { false };
	_bool m_bDrop = { false };
	_bool m_bSizeup = { false };
	_bool m_bSizedown = { false };
	_bool m_bAura = { false };
	_bool m_bGuizmo = { false };
	//���� ��ƼŬ ��� ��
	vector<CGameObject*> m_EditParticle = {  };
	vector<CGameObject*> m_EditTrail = {  };
	vector<CGameObject*> m_EditAura = {  };

	CParticle_Point::PARTICLE_POINT_DESC m_EffectDesc = {};
	CTRailEffect::TRAIL_DESC m_TrailDesc = {};
	CAura::AURA_DESC m_AuraDesc = {};

	bool m_bDragging = { false };

	_int m_iCurEditIndex = { -1 };
	char text_input_buffer[256] = { "" };
	char m_TrailTag[256] = { "" };
	char m_AuraTag[256] = { "" };

	_float m_fParticleTime = { 0.f };
	_float m_fMaxTime = { 30.f };

	_int m_iCurTexture = { 0 };
	vector<wstring> TextureTags;

	public:
	static CImguiManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END
