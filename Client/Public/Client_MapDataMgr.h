#pragma once

#include "Base.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CMapDataMgr;
END

BEGIN(Client)
class CClient_MapDataMgr final : public CBase
{
	DECLARE_SINGLETON(CClient_MapDataMgr)

public:
	enum OBJECT_TYPE {
		CONSTRUCTION, /* 그냥 건축물 */
		ITEM,
		MONSTER,
		OBJ_END
	};

private:
	CClient_MapDataMgr();
	virtual ~CClient_MapDataMgr() = default;

public:
	HRESULT									Set_MapObj_In_Client(int iMapLoadingNum, int iStageLevel);
	HRESULT									Set_GameObject_In_Client(int iStageLevel);

private:
	HRESULT									Set_Terrain_Size(int iStageLevel);
	int										Find_Layers_Index(char* strLayer);


private:
	class CGameInstance*					m_pGameInstance = { nullptr };
	class CMapDataMgr*						m_pMapDataMgr = { nullptr };


private:
	MAP_TOTALINFORM_DESC					m_MapTotalInform{};

	vector<wstring>							m_Layers = { TEXT("Layer_GameObjects"), TEXT("Layer_Monster") };


public:
	virtual void							Free() override;
};
END
