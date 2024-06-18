#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "imgui.h"

BEGIN(Engine)
class CGameObject;
END

class CObjPlace_Manager final : public CBase
{
	DECLARE_SINGLETON(CObjPlace_Manager);

private:
	CObjPlace_Manager();
	virtual ~CObjPlace_Manager() = default;

public:
	HRESULT					Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void					Priority_Tick(_float fTimeDelta);
	void					Tick(_float fTimeDelta);
	void					Late_Tick(_float fTimeDelta);
	void					Render();

//	/* wstring 받아서 */
//	void					Set_GameObjectsList(); /* imgui에 보여줄 아이들 */
//	void					Save_GameObject(int iLevel); /* 현재 map에 놓여진 object 저장*/
//	void					Load_GameObject(int iNum);
//
//
//public:
//	bool					Add_CloneObject_Imgui(_uint iLayerIndex, _uint iObjectIndex);
//
//public:
//	void					Add_LayerString(const wstring layer) { m_Layers.push_back(layer); }
//
//private:
//	bool					Find_Layer(vector<wstring> layer, wstring objname);
//	bool					Compare_Layer(wstring layer, wstring objname);
//	void					Delete_Object(int iNumObject); /* 선택 object 삭제 */
//	void					Update_ObjectNameList();
//	void					Edit_GameObject(int iNumObject);
//
//	void					Show_FileName();
//	void					Update_FileName();
//
//public:
//	vector<wstring>	m_Layers = { TEXT("Layer_Land"),TEXT("Layer_Palm"),TEXT("Layer_Cliff"), TEXT("Layer_Monster"), TEXT("Layer_GameObject") };
//
//	vector<wstring>	m_Lands = { TEXT("Prototype_Component_Model_CrabMapFlat2"),TEXT("Prototype_Component_Model_CrabMapFlat3"),TEXT("Prototype_Component_Model_CrabMapFlat4"),
//		TEXT("Prototype_Component_Model_CrabMapFlat5"),TEXT("Prototype_Component_Model_CrabMapFlat6"), TEXT("Prototype_Component_Model_CrabMapFlat7"),TEXT("Prototype_Component_Model_CrabMapFlat8") };
//
//	vector<wstring>	m_Cliffs = { TEXT("Prototype_Component_Model_CliffFlat1") , TEXT("Prototype_Component_Model_CliffFlat2"),TEXT("Prototype_Component_Model_CliffFlat3"),TEXT("Prototype_Component_Model_CliffFlat4"),TEXT("Prototype_Component_Model_CliffLarge1"),TEXT("Prototype_Component_Model_CliffLarge2"),TEXT("Prototype_Component_Model_CliffLarge3")
//	TEXT("Prototype_Component_Model_CliffLarge4") ,TEXT("Prototype_Component_Model_CliffLarge5") ,TEXT("Prototype_Component_Model_CliffLarge6") ,TEXT("Prototype_Component_Model_CliffMedium1") ,TEXT("Prototype_Component_Model_CliffMedium2") };
//
//	vector<wstring>	m_Palms = { TEXT("Prototype_Component_Model_Palm_1"),TEXT("Prototype_Component_Model_Palm_2"),TEXT("Prototype_Component_Model_Palm_3"),TEXT("Prototype_Component_Model_Palm_4") };
//
//	vector<wstring>	m_Monsters = { TEXT("Prototype_GameObject_Ant"), TEXT("Prototype_GameObject_Skull"), TEXT("Prototype_GameObject_Slug"),
//		TEXT("Prototype_GameObject_Scorpion"), TEXT("Prototype_GameObject_Pumpkin"), TEXT("Prototype_GameObject_StarFish") , TEXT("Prototype_GameObject_EnemyCrab"), TEXT("Prototype_GameObject_Pufferfish")
//	};
//	vector<wstring>	m_Objects = { TEXT("Prototype_GameObject_Shrub"), TEXT("Prototype_GameObject_BrightStone") , TEXT("Prototype_GameObject_BombStone") };
//
//	vector<wstring>	m_GroundObjects = { TEXT("Prototype_Component_Model_Rock1"), TEXT("Prototype_Component_Model_RockScatter") , TEXT("Prototype_Component_Model_Acacia") , TEXT("Prototype_Component_Model_Leaf")
//		, TEXT("Prototype_Component_Model_coral1"), TEXT("Prototype_Component_Model_coral2"), TEXT("Prototype_Component_Model_coral3")
//	, TEXT("Prototype_Component_Model_seaweed"), TEXT("Prototype_Component_Model_Shrub"), TEXT("Prototype_Component_Model_seaweedshort")
//	, TEXT("Prototype_Component_Model_CoralAsset1") , TEXT("Prototype_Component_Model_CoralAsset2") , TEXT("Prototype_Component_Model_CoralAsset3"), TEXT("Prototype_Component_Model_CoralAsset4")
//	, TEXT("Prototype_Component_Model_Marimo")
//	, TEXT("Prototype_Component_Model_Clam") , TEXT("Prototype_Component_Model_CoralB1"), TEXT("Prototype_Component_Model_Gorgonian"), TEXT("Prototype_Component_Model_SeaFan")
//	};
//

private:
	/* object이름, cgameobject */
	multimap<wstring, CGameObject*>			m_GameObjects;
	vector<char*>							m_ObjectNames;
	vector<char*>							m_FileNames;

private:
	_uint								m_iObjectNums = 0;


public:
	virtual void Free() override;
};

