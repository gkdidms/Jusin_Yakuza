#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "imgui.h"
#include "GameObject.h"
#include "Construction.h"

BEGIN(Engine)
class CGameInstance;
END

/* ������ �߰��Ҷ����� ���� */

class CObjPlace_Manager final : public CBase
{
	DECLARE_SINGLETON(CObjPlace_Manager);

public:
	enum OBJECT_TYPE {
		CONSTRUCTION, /* �׳� ���๰ */
		ITEM,
		MONSTER,
		OBJ_END
	};

public:
	typedef struct tObjPlaceDesc
	{
		/* ������Ʈ ��ġ�� �� ���� �׸� ���� */
		int				iLayer;
		int				iShaderPass;
		int				iObjType;
		int				iObjPropertyType;
	}MAPTOOL_OBJPLACE_DESC;


private:
	CObjPlace_Manager();
	virtual ~CObjPlace_Manager() = default;

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
	void					Get_CurrentGameObject_Desc(CConstruction::MAPOBJ_DESC*	mapDesc, int iNumObject);
	void					Edit_CurrentGameObject_Desc(CConstruction::MAPOBJ_DESC* mapDesc, int iNumObject);
	/* object ��ġ */
	bool					Add_CloneObject_Imgui(MAPTOOL_OBJPLACE_DESC objDesc, _uint iFolderNum, _uint iObjectIndex);
	/* Object ���� ���� */
	void					Set_Map_Object();
	void					Add_Decal_IMGUI();


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
	int						Find_Layers_Index(char*	strLayer);

	void					Get_Decal_Texture(int iNumObject);
	void					Update_DecalNameList();

	/* gameobject �ȿ� ��ġ�� decal�� ������ */
	void					Update_DecalNameList_In_Object();
	void					Update_DecalList_In_Object();
	void					Edit_Installed_Decal(int iObjectDecalNum);


private:
	vector<const char*>							m_Layers = { "Layer_GameObjects", "Layer_Monster" };
	/* object�̸�, cgameobject */
	multimap<wstring, CGameObject*>				m_GameObjects; /* �߰��� ������Ʈ ���� */
	vector<char*>								m_ObjectNames_Map0; /* �߰��� ������Ʈ �̸��� */
	vector<char*>								m_ObjectNames_Map1; /* �߰��� ������Ʈ �̸��� */
	vector<char*>								m_FileNames; /* �ʸ��� ������ bin ����Ʈ�� */
	vector<char*>								m_DecalNames; 
	vector<char*>								m_DecalNames_Obj;
	class CGameInstance*						m_pGameInstance = { nullptr };
	/* ��ġ �� ���� ������Ʈ���� �����ִ� */
	CGameObject*								m_pShownObject = { nullptr }; 
	bool										m_bShowExample = { false };
	vector<char*>								m_ObjectNames; /* �ʿ� ��ġ�� ���̵鿡 ���� ����Ʈ */
	class CDecal*								m_pDecal;

private:
	bool										m_bFindObject = { false };
	bool										m_bAddDecal_IMGUI = { false };
	_uint										m_iObjectNums = 0;
	/* ��ġ���� bool ���� */
	_bool										m_bDoing_Place_Object = false;
	/* ���콺 Lbtn ������ �ѹ��� �߰��ǰ� */
	_bool										m_bInstallOneTime = { false };
	CConstruction::MAPOBJ_DESC					m_tCurrentObjectDesc;
	int											m_iCurrentObjectIndex = { 0 };

	vector<DECAL_DESC>							m_Decals; /* �ش� ������Ʈ�� �ִ� ��� decal�� ���� */
	vector<CDecal*>								m_ObjectDecals;
	int											m_iDecalObjNum = { -1 };

public:
	virtual void Free() override;
};

