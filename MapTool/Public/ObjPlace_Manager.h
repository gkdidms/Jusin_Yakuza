#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "imgui.h"
#include "GameObject.h"
#include "Construction.h"

BEGIN(Engine)
class CGameInstance;
END

/* 데이터 추가할때마다 수정 */

class CObjPlace_Manager final : public CBase
{
	DECLARE_SINGLETON(CObjPlace_Manager);

public:
	enum OBJECT_TYPE {
		CONSTRUCTION, /* 그냥 건축물 */
		ITEM,
		MONSTER,
		OBJ_END
	};

public:
	typedef struct tObjPlaceDesc
	{
		/* 오브젝트 설치할 때 고르는 항목 저장 */
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
	void					Show_Installed_GameObjectsList(); /* imgui에 보여줄 아이들 */
	void					Save_GameObject(int iLevel); /* 현재 map에 놓여진 object 저장*/
	void					Load_GameObject(int iNum);
	void					Edit_Installed_GameObject(int iNumObject);
	void					Edit_GameObject_Transform(int iNumObject);
	void					Get_CurrentGameObject_Desc(CConstruction::MAPOBJ_DESC*	mapDesc, int iNumObject);
	void					Edit_CurrentGameObject_Desc(CConstruction::MAPOBJ_DESC* mapDesc, int iNumObject);
	void					Update_CurrentGameObject_Desc();

	/* object 배치 */
	bool					Add_CloneObject_Imgui(MAPTOOL_OBJPLACE_DESC objDesc, _uint iFolderNum, _uint iObjectIndex);
	/* Object 종류 고르기 */
	void					Set_Map_Object();


private:
	void					Load_ModelName();



private:
	string					modifyString(string& input);
	void					Show_ExampleModel(MAPTOOL_OBJPLACE_DESC objDesc, _uint iFolderNum, _uint iObjectIndex);
	string					Find_ModelName(_uint iFolderNum, _uint iObjectIndex);

	void					Delete_Object(int iNumObject); /* 선택 object 삭제 */
	void					Update_ObjectNameList();

	void					Show_FileName();
	void					Update_FileName();


private:
	int						Click_To_Select_Object();

	int						Find_Layers_Index(char*	strLayer);


private:
	vector<const char*>							m_Layers = { "Layer_GameObjects", "Layer_Monster" };
	/* object이름, cgameobject */
	multimap<wstring, CGameObject*>				m_GameObjects; /* 추가한 오브젝트 저장 */
	vector<char*>								m_ObjectNames_Map0; /* 추가한 오브젝트 이름들 */
	vector<char*>								m_ObjectNames_Map1; /* 추가한 오브젝트 이름들 */
	vector<char*>								m_FileNames; /* 맵마다 저장한 bin 리스트들 */


	class CGameInstance*						m_pGameInstance = { nullptr };

	/* 설치 전 무슨 오브젝트인지 보여주는 */
	CGameObject*								m_pShownObject = { nullptr }; 
	bool										m_bShowExample = { false };


	vector<char*>								m_ObjectNames; /* 맵에 설치된 아이들에 대한 리스트 */

private:
	_uint										m_iObjectNums = 0;

	/* 배치관련 bool 변수 */
	_bool										m_bDoing_Place_Object = false;

	/* 마우스 Lbtn 누르면 한번만 추가되게 */
	_bool										m_bInstallOneTime = { false };

	CConstruction::MAPOBJ_DESC					m_tCurrentObjectDesc;

	int											m_iCurrentObjectIndex = { -1 };


public:
	virtual void Free() override;
};

