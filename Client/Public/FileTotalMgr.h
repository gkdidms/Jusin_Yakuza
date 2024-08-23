#pragma once

#include "Base.h"
#include "CineCamera.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CFileTotalMgr final : public CBase
{
	DECLARE_SINGLETON(CFileTotalMgr)

public: 
	/* MapTool - ���� �ٲٸ� �ȵ�!!!!!!!!!!!!!!!!! */
	enum OBJECT_TYPE {
		CONSTRUCTION, /* �׳� ���๰ */
		ROAD,
		ITEM,
		MONSTER_RUSH,
		PLAYER,
		SKY,
		LIGHT,
		MONSTER_WPA,
		MONSTER_SHAKEDOWN,
		MONSTER_YONEDA,
		MONSTER_KUZE,
		LARGE_CONSTRUCTION,
		ADTENTURE_SUIT,
		MAP,
		MONSTER_WPH,
		MONSTER_DEFAULT,
		MAP_NONCULL, // MAP�̶� ���� Ŭ�������� �ø� �ȵ�(��MESH)


		ROADNML,
		ROADCAB,
		ROADSTANDING_NML,
		ROADTISSUE,
		ROADYOP,
		NISHIKI,
		ADVENTURE_REACTOR,

		PLAYER_POSITION, // �����Ǹ� ����

		OBJ_END
	};


	enum TRIGGER_TYPE {
		TRIGGER_MOVE_LEVEL,
		TRIGGER_CINEMACHINE,
		TRIGGER_YONEDA,
		TRIGGER_MONSTER,
		TRIGGER_QUEST,
		TRIGGER_END
	};

	enum TRIGGER_CINEMACHINE_TYPE {

		TRIGGER_CINEMACHINE_END
	};


private:
	CFileTotalMgr();
	virtual ~CFileTotalMgr() = default;

public:
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

	/* MapTool */
public:
	/* ���Ϲ�ȣ, LEVEL_GAME���� ���� */
	HRESULT									Set_MapObj_In_Client(int iMapLoadingNum, int iStageLevel);
	HRESULT									Set_GameObject_In_Client(int iStageLevel);
	HRESULT									Set_Lights_In_Client(int iLightLoadingNum);
	HRESULT									Set_Collider_In_Client(int iColliderLoadingNum, int iStageLevel);
	HRESULT									Set_Trigger_In_Client(int iTriggerLoadingNum, int iStageLevel);


	HRESULT									Set_MapObj_In_Client_Trigger(int iMapLoadingNum, int iStageLevel);
	HRESULT									Set_GameObject_In_Client_Trigger(int iStageLevel);

	HRESULT									Set_NaviRoute_In_Client(int iRouteNum);

	void									Load_Cinemachine(int iCineNum, int iStageLevel); //Cinemachine ī�޶� ���� ��ü�� load
	void									Setting_Start_Cinemachine(int iCineNum);
	_uint Get_CineCameraIndex();

	/* LEVEL �ٲ�� �ʱ�ȭ ��Ű�� */
	void									Reset_Cinemachine();

	const _float4x4*						Get_PlayerMatrix() { return &m_PlayerPosition.front(); }



private:
	class CGameInstance* m_pGameInstance = { nullptr };
	CCineCamera* m_pCinemachineCam = { nullptr };

private:
	MAP_TOTALINFORM_DESC					m_MapTotalInform{};
	LIGHT_DESC_IO							m_LightTotalInform{};
	COLLIDER_IO								m_MapCollider{};
	TRIGGER_IO								m_Trigger{};
	MAP_TOTALINFORM_DESC					m_MapTriggerInform{}; //Trigger�� �ҷ����� object

	vector<wstring>							m_Layers = { TEXT("Layer_GameObjects"), TEXT("Layer_Monster"), TEXT("Layer_Player"), TEXT("Layer_NPC"), TEXT("Layer_Item"), TEXT("Layer_Passerby"),  TEXT("Layer_MonsterGroup"), TEXT("Layer_Nishiki") , TEXT("Layer_AdventureReactor") };

	vector<_float4x4>							m_PlayerPosition;

private:
	HRESULT									Set_Terrain_Size(int iStageLevel);
	int										Find_Layers_Index(char* strLayer);

	HRESULT									Import_Bin_Map_Data_OnClient(MAP_TOTALINFORM_DESC* mapObjData, int iLevel);
	HRESULT									Import_Bin_Light_Data_OnClient(LIGHT_DESC_IO* lightData, int iLevel);
	HRESULT									Import_Bin_Collider_Data_OnTool(COLLIDER_IO* ColliderData, int iLevel);
	HRESULT									Import_Bin_Trigger_Data_OnTool(TRIGGER_IO* TriggerData, int iLevel);
	HRESULT									Import_Bin_TriggerMap_Data_OnClient(MAP_TOTALINFORM_DESC* mapObjData, int iLevel);


public:
	virtual void							Free() override;
};
END
