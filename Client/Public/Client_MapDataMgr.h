#pragma once

#include "Base.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CClient_MapDataMgr final : public CBase
{
	DECLARE_SINGLETON(CClient_MapDataMgr)

public:
	enum OBJECT_TYPE {
		CONSTRUCTION, /* �׳� ���๰ */
		ITEM,
		MONSTER,
		OBJ_END
	};

private:
	CClient_MapDataMgr();
	virtual ~CClient_MapDataMgr() = default;

public:
	/* ���Ϲ�ȣ, LEVEL_GAME���� ���� */
	HRESULT									Set_MapObj_In_Client(int iMapLoadingNum, int iStageLevel);
	HRESULT									Set_GameObject_In_Client(int iStageLevel);
	HRESULT									Set_Lights_In_Client(int iLightLoadingNum);

private:
	HRESULT									Set_Terrain_Size(int iStageLevel);
	int										Find_Layers_Index(char* strLayer);

	HRESULT									Import_Bin_Map_Data_OnClient(MAP_TOTALINFORM_DESC* mapObjData, int iLevel);
	HRESULT									Import_Bin_Light_Data_OnClient(LIGHT_DESC_IO* lightData, int iLevel);


private:
	class CGameInstance*					m_pGameInstance = { nullptr };

private:
	MAP_TOTALINFORM_DESC					m_MapTotalInform{};
	LIGHT_DESC_IO							m_LightTotalInform{};

	vector<wstring>							m_Layers = { TEXT("Layer_GameObjects"), TEXT("Layer_Monster") };


public:
	virtual void							Free() override;
};
END
