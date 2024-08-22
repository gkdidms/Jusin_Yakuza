#pragma once

#include "Base.h"
#include "imgui.h"
#include "GameObject.h"
#include "Construction.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

/* ������ �߰��Ҷ����� ���� */

class CTriggerObj_Manager final : public CBase
{
	DECLARE_SINGLETON(CTriggerObj_Manager);

public:
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
		MAP, /* ���� �� ���� ū �� */
		MONSTER_WPH,
		MONSTER_DEFAULT,
		MAP_NONCULL,


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

	//Edit �Լ� ����ϴ����ΰ�
	enum EDITTYPE
	{
		OBJECT,
		DECAL,
		COLLIDER,
		EDITTYPE_END
	};

	enum ColliderType
	{
		AABB,
		OBB,
		SPHERE,
		COLLIDER_END
	};

public:
	typedef struct tObjPlaceDesc
	{
		/* ������Ʈ ��ġ�� �� ���� �׸� ���� */
		int				iLayer;
		int				iShaderPass;
		int				iObjType;
		int				iObjPropertyType;
		int				iNPCDirection;
		int				iNaviRouteNum;
		int				iGroupMonster; //bool ����
		int				iGroupNum;
		XMFLOAT4X4		vOffsetMatrix;
	}MAPTOOL_OBJPLACE_DESC;


private:
	CTriggerObj_Manager();
	virtual ~CTriggerObj_Manager() = default;

public:
	HRESULT					Initialize();
	void					Priority_Tick(const _float& fTimeDelta);
	void					Tick(const _float& fTimeDelta);
	void					Late_Tick(const _float& fTimeDelta);
	void					Render();

public:
	void					Show_Installed_GameObjectsList(); /* imgui�� ������ ���̵� */
	void					Save_GameObject(int iLevel); /* ���� map�� ������ object ����*/
	void					Load_GameObject(int iNum);
	void					Edit_Installed_GameObject(int iNumObject);
	void					Edit_GameObject_Transform(int iNumObject);
	void					Get_CurrentGameObject_Desc(CConstruction::MAPOBJ_DESC* mapDesc, int iNumObject);
	void					Edit_CurrentGameObject_Desc(CConstruction::MAPOBJ_DESC* mapDesc, int iNumObject);
	/* object ��ġ */
	bool					Add_CloneObject_Imgui(MAPTOOL_OBJPLACE_DESC objDesc, _uint iFolderNum, _uint iObjectIndex);
	/* Object ���� ���� */
	void					Set_Map_Object();
	void					Add_Decal_IMGUI();
	void					Add_ObjectCollider_IMGUI();


private:
	void					Load_ModelName();
	HRESULT					Import_Bin_Map_Data_OnTool(MAP_TOTALINFORM_DESC* mapObjData, char* fileName);
	HRESULT					Export_Bin_Map_Data(MAP_TOTALINFORM_DESC* mapObjData);

	string					modifyString(string& input);
	void					Show_ExampleModel(MAPTOOL_OBJPLACE_DESC objDesc, _uint iFolderNum, _uint iObjectIndex);
	string					Find_ModelName(_uint iFolderNum, _uint iObjectIndex);

	void					Delete_Object(int iNumObject); /* ���� object ���� */
	void					Update_ObjectNameList();

	void					Show_FileName();
	void					Update_FileName();

	void					Click_To_Select_Object(int& iObjectNum);
	int						Find_Layers_Index(char* strLayer);

	void					Get_Decal_Texture(int iNumObject);
	void					Update_DecalNameList();

	/* gameobject �ȿ� ��ġ�� decal�� ������ */
	void					Update_DecalNameList_In_Object();
	void					Update_ColliderNameList();
	void					Update_ColliderList_In_Object();


	void					Update_DecalList_In_Object();
	void					Edit_Installed_Decal(int iObjectDecalNum);

	void					Reset_Collider();


private:
	vector<const char*>							m_Layers = { "Layer_GameObjects", "Layer_Monster", "Layer_Player", "Layer_NPC" , "Layer_Item", "Layer_Passerby", "Layer_MonsterGroup", "Layer_Nishiki" , "Layer_AdventureReactor" };
	/* object�̸�, cgameobject */
	multimap<wstring, CGameObject*>				m_GameObjects; /* �߰��� ������Ʈ ���� */
	vector<char*>								m_ObjectNames_Map0; /* �߰��� ������Ʈ �̸��� */
	vector<char*>								m_ObjectNames_Map1; /* �߰��� ������Ʈ �̸��� */
	vector<char*>								m_ObjectNames_Map2; /* �߰��� ������Ʈ �̸��� */
	vector<char*>								m_ObjectNames_Map3; /* �߰��� ������Ʈ �̸��� */
	vector<char*>								m_ObjectNames_MapKaraoke; /* �߰��� ������Ʈ �̸��� */

	vector<char*>								m_MonsterNames; /* �߰��� ������Ʈ �̸��� */

	vector<char*>								m_FileNames; /* �ʸ��� ������ bin ����Ʈ�� */
	vector<char*>								m_DecalNames;
	vector<char*>								m_DecalNames_Obj;
	vector<char*>								m_ColliderNames;

	class CGameInstance* m_pGameInstance = { nullptr };
	/* ��ġ �� ���� ������Ʈ���� �����ִ� */
	CGameObject* m_pShownObject = { nullptr };
	bool										m_bShowExample = { false };
	vector<char*>								m_ObjectNames; /* �ʿ� ��ġ�� ���̵鿡 ���� ����Ʈ */
	class CDecal* m_pDecal;

private:
	bool										m_bFindObject = { false };
	bool										m_bAddDecal_IMGUI = { false };
	bool										m_bAddObjectCollider_IMGUI = { false };
	_uint										m_iObjectNums = 0;
	/* ��ġ���� bool ���� */
	_bool										m_bDoing_Place_Object = false;
	/* ���콺 Lbtn ������ �ѹ��� �߰��ǰ� */
	_bool										m_bInstallOneTime = { false };
	CConstruction::MAPOBJ_DESC					m_tCurrentObjectDesc;
	int											m_iCurrentObjectIndex = { 0 };

	int											m_iCurrentCollidrNum = { 0 };

	vector<DECAL_DESC>							m_Decals; /* �ش� ������Ʈ�� �ִ� ��� decal�� ���� */
	vector<CDecal*>								m_ObjectDecals;
	vector<CCollider*>							m_ObjectColliders;
	int											m_iDecalObjNum = { -1 };

	EDITTYPE									m_eEditType;

	OBJCOLLIDER_DESC							m_tCurColliderDesc;
	int											m_iCurColliderIndex = { 0 };

public:
	virtual void Free() override;
};

