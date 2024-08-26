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
class CAnimation;
END

BEGIN(Client)
class CImguiManager final :
	public CBase
{
private:
	enum Model_Type
	{
		PLAYER, ENEMY, NPC, 
	};

	enum AnimCom_Type
	{
		MODEL_ANIMS, ANIM_COM, SYNC_COM, 
	};

	enum Collider_Type
	{
		AABB, OBB, SPHERE
	};
	enum Collider_Action_Type
	{
		ATTACK, HIT
	};
	enum Collider_Part_Type
	{
		HAND_A, FOOT_A, JOINT_A, HEAD_A,
		HEAD_H = 10, BODY_H, LEG_H,
	};

	// ���õȰŴ� ���������� �����ش�
	//�ݶ��̴� Ȱ��ȭ(���), �ݶ��̴� ��Ȱ��ȭ(��Ȳ), ���� Ȱ��ȭ(�ʷ�)
	enum Animation_Event_Type
	{
		COLLIDER_ACTIVATION, COLLIDER_DISABLE, SOUND_ACTIVATION, 
		ANIMATION_EVENT_TYPE_END
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

	struct Collider_State
	{
		string strBoneName;
		int iColliderActionType;
		int iColliderPartType;
		_bool	isAlways;
	};

	struct Animation_RimLightState
	{
		_uint iType;					//0���� on 1���� off
		_float fAinmPosition;
		string strMeshName;
	};

	struct Animation_TrailState
	{
		_uint iType;					//0���� on 1���� off
		_float fAinmPosition;
		string strBonelName;
		string strTrailProtoName;
		_uint iBoneIndex;
	};

	struct Animation_FaceEventState
	{
		_uint iType;					//0���� on 1���� off 2���� change
		_float fAinmPosition;
		_uint iFaceAnimIndex;
	};

	struct Animation_BloodEventState
	{
		_float fAinmPosition;
		_uint iBoneIndex;
		string strBonelName;
		_uint iBloodEffectType;		// ��½�ų ����Ʈ�� Ÿ��
		_bool isLoop;				// ���� ����
		_bool isOn;					// ����Ʈ On/Off ����
	};

	struct Animation_RadialEventState
	{
		_uint iType;				//0���� on 1���� off
		_float fAinmPosition;
		_float fForce;
	};

	struct Animation_SoundEventState
	{
		_uint iChannel;
		_float fAinmPosition;
		_float fSoundVolume;
		string strSoundFileName;
		_bool isPlay = { false };
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

	/* Ű������ �����쿡�� �ִϸ��̼� �̺�Ʈ�� ����Ʈ, ���� �߰��ϴ� â */
	void EffectListWindow();
	void RimLightWindow();
	void TrailWindow();
	void SoundListWindow();

	void FaceEventWindow();
	void BloodEventWindow();
	void RadialEventWindow();

	void DrawTimeline(ImDrawList* d);
	void DrawChannels();

private:
	void Update_Model_Position();
	void Update_Model_RotationX();
	void Update_Model_RotationY();
	void Update_Model_RotationZ();
	void Update_Model_Scaled();

private:
	/* Initailize */
	void Setting_Collider_Value(_uint iBoneIndex);
	void Setting_InitialData(string strFolderType);
	void Setting_ModelList(string strFolderType);

	/* Reset */
	void Reset_Collider_Value();
	void Clear_EffectStateMap();

private:
	void Create_Effect(string& strBoneName, string& strEffectName, wstring wstrLayer = TEXT("Layer_Effect"));

	void Play_SoundEvent();
	void Reset_SoundEvent_Played();

private:
	/* Save */
	void All_Save();
	void AlphaMesh_Save(string strPath);
	void AnimationLoop_Save(string strPath);
	void AnimationEvent_Save(string strPath);
	void ColliderState_Save(string strPath);
	void EffectState_Save(string strPath);
	void RimEvent_Save(string strPath);
	void TrailEvent_Save(string strPath);
	void SoundEvent_Save(string strPath);
	void FaceEvent_Save(string strPath);
	void BloodEvent_Save(string strPath);
	void RadialEvent_Save(string strPath);

	/* Load */
	void All_Load();
	void AlphaMesh_Load(string strPath);
	void AnimationLoop_Load(string strPath);
	void AnimationEvent_Load(string strPath);
	void ColliderState_Load(string strPath);
	void EffectState_Load(string strPath);
	void RimEvent_Load(string strPath);
	void TrailEvent_Load(string strPath);
	void SoundEvent_Load(string strPath);
	void FaceEvent_Load(string strPath);
	void BloodEvent_Load(string strPath);
	void RadialEvent_Load(string strPath);

	/* Functional*/
private:
	void Gui_Select_Bone(_uint iBoneIndex);
	void Setting_AnimationList();			//�÷��̾�,�� ������ ���� ���� �ִϸ��̼� ������Ʈ�� �о�´�
	void Setting_RimLight();				// ������Ʈ �Ž� �̸� �����ϱ�
	void Setting_Trail();				// Ʈ���� �¿��� �����ϱ�
	void Change_Model();

private:
	ImGuiIO* io;
	fs::path m_RootDir;

private:
	_bool					m_isOnToolWindows = { false };

	int						m_iAnimIndex = { 0 };
	int						m_iSearchAnimIndex = { 0 };
	int						m_iAddedAnimSelectedIndex = { 0 };

	int						m_iModelSelectedIndex = { 0 };

	int						m_iBoneSelectedIndex = { 0 };
	int						m_iColliderSelectedIndex = { 0 };
	int						m_iEventBoneIndex = { 0 };

	int						m_iChannelSelectedIndex = { 0 };

	int						m_iEventSelectedIndex = { 0 };

	int						m_iMeshSelectedIndex = { 0 };
	int						m_iAddedMeshSelectedIndex = { 0 };

	char									m_szSearchChannelName[_MAX_PATH];
	char									m_szSearchBoneName[_MAX_PATH];

	vector<string>							m_ModelNameList;

	vector<string>							m_AnimNameList;
	unordered_map<_uint, string>			m_AddedAnims;			// �ִϸ��̼� �ε����� �̸��� �����Ѵ�.

	vector<string>							m_BoneNameList;
	unordered_map<_uint, Collider_State>	m_AddedColliders;		// �� �ε����� �� �̸��� �����Ѵ�

	vector<string>							m_ChannelNameList;

	vector<string>							m_MeshNameList;
	unordered_map<_uint, string>			m_AddedMeshes;			// �Ž� �ε����� �̸��� �����Ѵ�.

	// first: �ִϸ��̼� �̸�, second: �̺�Ʈ����
	multimap<string, Animation_Event>		m_AnimationEvents;

	//first: �� �̸�, second: �̸�
	multimap<string, string>				m_EffectState;

private:
	vector<CAnimation*>			m_Anims;

	/* ����Ʈ ������ */
private:
	_bool						m_isEffectListWindow = { false };

	_bool						m_isEffectOff = { false };

	_uint						m_iEffectType = { 0 };
	int							m_iEffectSelectedIndex = { 0 };
	int							m_iAddedEffectSelectedIndex = { 0 };

	vector<string>				m_EffectTypeList;
	multimap<_uint, string>		m_EffectFiles;
	string						m_SelectedEffectName = "Prototype_GameObject_Particle_Trail_asd";			// Ʈ���� ���� â���� ����Ϸ��� ������

	/* ������Ʈ ������ */
private:
	_bool						m_isRimLightWindow = { false };

	int							m_iRimLightEventIndex = { 0 };

	// first: �ִϸ��̼� �̸�, second: ������Ʈ �̺�Ʈ����
	multimap<string, Animation_RimLightState>		m_RimLightEvents;

	/* Ʈ���� ������ */
private:
	_bool						m_isTrailWindow = { false };

	int							m_iTrailEventIndex = { 0 };

	// first: �ִϸ��̼� �̸�, second: Ʈ���� �̺�Ʈ����
	multimap<string, Animation_TrailState>		m_TrailEvents;

	/* ���̽� �̺�Ʈ ������ */
private:
	_bool						m_isFaceWindow = { false };
	int							m_iFaceEventIndex = { 0 };
	int							m_iFaceAnimIndex = { 0 };

	// first: �ִϸ��̼� �̸�, second: Ʈ���� �̺�Ʈ����
	multimap<string, Animation_FaceEventState>		m_FaceEvents;

	/* ���� ����Ʈ ������ */
private:
	_bool						m_isBloodWindow = { false };
	int							m_iBloodEventIndex = { 0 };
	int							m_eBloodEffectType = { 0 };
	bool						m_isBloodEffectLoop = { false };
	bool						m_isBloodEffectOn = { true };

	// first: �ִϸ��̼� �̸�, second: �̺�Ʈ����
	multimap<string, Animation_BloodEventState>		m_BloodEvents;

	/* ����� �̺�Ʈ ������ */
private:
	_bool						m_isRadialEventWindow = { false };
	int							m_iRadialEventIndex = { 0 };
	float						m_fRadialForce = { 1.f };

	// first: �ִϸ��̼� �̸�, second: �̺�Ʈ����
	multimap<string, Animation_RadialEventState>		m_RadialEvents;

	/* ���� �̺�Ʈ ������ */
private:
	_bool						m_isCutSceneEvents = { false };
	_bool						m_isSoundListWindow = { false };
	CHANNELID					m_eSoundCannel = { SOUND_EFFECT };
	string						m_strSelectSoundFileName = "";
	int							m_iSoundEventIndex = { 0 };
	float						m_fSoundVolume = { 1.f };

	// first: �ִϸ��̼� �̸�, second: �̺�Ʈ����
	multimap<string, Animation_SoundEventState>		m_SoundEvents;

private:
	_float						m_fTimeDeltaScale = { 1.f };

private:
	bool						m_isAlwaysCollider = { false };
	float						m_ModelPosition[3] = { 0.f };
	float						m_ModelRotation[3] = { 0.f };
	float						m_ModelScale = { 0.f };

	float						m_fAnimationPosition = { 0.f };

	int							m_iModelType = { PLAYER };
	int							m_iModelPathType = { PLAYER };

private:
	bool						m_isPause = { false };

	int							m_iColliderType = { 0 };
	int							m_iColliderActionType = { 0 };
	int							m_iColliderPartType = { 0 };

	float						m_fColliderRadius = { 0.f };
	float						m_ColliderPosition[3] = { 0.f };
	float						m_ColliderExtents[3] = { 0.f };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	class CAnimModel* m_pRenderModel = { nullptr };

public:
	static CImguiManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END