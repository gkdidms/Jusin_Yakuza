#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "imgui.h"
#include "GameObject.h"

BEGIN(Engine)
class CGameInstance;
END

class CObjPlace_Manager final : public CBase
{
	DECLARE_SINGLETON(CObjPlace_Manager);

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
	void					Set_GameObjectsList(); /* imgui에 보여줄 아이들 */
	void					Save_GameObject(int iLevel); /* 현재 map에 놓여진 object 저장*/
	void					Load_GameObject(int iNum);
	void					Edit_GameObject(int iNumObject);

	bool					Add_CloneObject_Imgui(_uint iLayerIndex, _uint iObjectIndex);
	/* Object 종류 고르기 */
	void					Set_Map_Object();


private:
	void					Load_ModelName();



private:
	string					modifyString(string& input);



private:
	/* object이름, cgameobject */
	multimap<wstring, CGameObject*>				m_GameObjects; /* 추가한 오브젝트 저장 */
	vector<char*>								m_ObjectNames_Map0; /* 추가한 오브젝트 이름들 */
	vector<char*>								m_ObjectNames_Map1; /* 추가한 오브젝트 이름들 */
	vector<char*>								m_FileNames; /* 맵마다 저장한 bin 리스트들 */

	vector<char*>								m_Layers;
	vector<char*>								m_ModelNames;

	class CGameInstance*						m_pGameInstance = { nullptr };

private:
	_uint										m_iObjectNums = 0;

	/* 배치관련 bool 변수 */
	_bool										m_bDoing_Place_Object = false;


public:
	virtual void Free() override;
};

