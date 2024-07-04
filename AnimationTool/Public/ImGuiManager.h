#pragma once
#include "Client_Defines.h"
#include "Base.h"

#pragma region "Imgui"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#pragma endregion

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CImguiManager final :
	public CBase
{
private:
	enum Collider_Type
	{
		AABB, OBB, SPHERE
	};

	// 선택된거는 빨간색으로 보여준다
	//콜라이더 활성화(노랑), 콜라이더 비활성화(주황), 사운드 활성화(초록), 이펙트 활성화(파랑)
	enum Animation_Event_Type
	{
		COLLIDER_ACTIVATION, COLLIDER_DISABLE, SOUND_ACTIVATION, EFFECT_ACTIVATION, ANIMATION_EVENT_TYPE_END
	};

public:
	struct Animation_Event
	{
		_uint iType;
		_float fAinmPosition;
		string strChannelName;
		_uint iBoneIndex;
		_bool isSelected = { false };
	};

private:
	CImguiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImguiManager() = default;

public:
	HRESULT Initialize(void* pArg = nullptr);
	void Tick(const _float& fTimeDelta);
	HRESULT Render();

public:
	void Connect_Model_Ref();
	string Get_FirstModel_Name();

private:
	void ModelList();
	void AnimListWindow();
	void BoneListWindow();
	void KeyFrameWindow();
	void MeshListWindow();
	void AnimEventWindow();

	/* 키프레임 윈도우에서 애니메이션 이벤트에 이펙트, 사운드 추가하는 창 */
	void EffectListWindow();
	void SoundListWindow();


	void DrawTimeline(ImDrawList* d);
	void DrawChannels();

	//주석
private:
	void LoadAnimationCharacterList();

private:
	void Update_Model_Position();
	void Update_Model_RotationX();
	void Update_Model_RotationY();
	void Update_Model_RotationZ();
	void Update_Model_Scaled();

private:
	void Reset_Collider_Value();
	void Setting_Collider_Value(_uint iBoneIndex);

private:
	/* Save */
	void All_Save();
	void AlphaMesh_Save(string strPath);
	void AnimationLoop_Save(string strPath);
	void AnimationEvent_Save(string strPath);
	void ColliderState_Save(string strPath);

	/* Load */
	void All_Load();
	void AlphaMesh_Load(string strPath);
	void AnimationLoop_Load(string strPath);
	void AnimationEvent_Load(string strPath);
	void ColliderState_Load(string strPath);

	/* Initailize */
	void Setting_InitialData();

private:
	ImGuiIO* io;

private:
	_bool					m_isOnToolWindows = { false };

	int						m_iAnimIndex = { 0 };
	int						m_iSearchAnimIndex = { 0 };
	int						m_iAddedAnimSelectedIndex = { 0 };

	int						m_iModelSelectedIndex = { 0 };

	int						m_iBoneSelectedIndex = { 0 };
	int						m_iColliderSelectedIndex = { 0 };

	int						m_iChannelSelectedIndex = { 0 };

	int						m_iEventSelectedIndex = { 0 };

	int						m_iMeshSelectedIndex = { 0 };
	int						m_iAddedMeshSelectedIndex = { 0 };

	char									m_szSearchChannelName[_MAX_PATH];
	char									m_szSearchBoneName[_MAX_PATH];

	vector<string>							m_ModelNameList;

	vector<string>							m_AnimNameList;
	unordered_map<_uint, string>			m_AddedAnims;			// 애니메이션 인덱스와 이름을 저장한다.

	vector<string>							m_BoneNameList;
	unordered_map<_uint, string>			m_AddedColliders;		// 뼈 인덱스와 뼈 이름을 저장한다

	vector<string>							m_ChannelNameList;

	vector<string>							m_MeshNameList;
	unordered_map<_uint, string>			m_AddedMeshes;			// 매쉬 인덱스와 이름을 저장한다.

	// first: 애니메이션 이름, second: 이벤트정보
	multimap<string, Animation_Event>		m_AnimationEvents;		


private:
	_bool					m_isEffectListWindow = { false };

	_uint						m_iEffectType = { 0 };
	int							m_iEffectSelectedIndex = { 0 };

	vector<string>				m_EffectTypeList;
	multimap<_uint, string>		m_EffectFiles;

private:
	_bool					m_isSoundListWindow = { false };

	_uint					m_iSoundType = { 0 };
	vector<string>			m_SoundTypeList;
	
private:
	_float					m_fTimeDeltaScale = { 1.f };

private:
	float					m_ModelPosition[3] = { 0.f };
	float					m_ModelRotation[3] = { 0.f };
	float					m_ModelScale = { 0.f };

	float					m_fAnimationPosition = { 0.f };

	bool					m_isPause = { false };

	int						m_iColliderType = { 0 };
	float					m_fColliderRadius = { 0.f };
	float					m_ColliderPosition[3] = { 0.f };
	float					m_ColliderExtents[3] = { 0.f };

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
