#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "CineCamera.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CFileTotalMgr final : public CBase
{
	DECLARE_SINGLETON(CFileTotalMgr)

public:
	enum OBJECT_TYPE {
		CONSTRUCTION, /* 그냥 건축물 */
		ITEM,
		MONSTER,
		OBJ_END
	};

private:
	CFileTotalMgr();
	virtual ~CFileTotalMgr() = default;

public:
	void											Tick(_float fTimeDelta);
	void											Late_Tick(_float fTimeDelta);

public:
	/* 파일번호, LEVEL_GAME같은 레벨 */
	HRESULT									Set_MapObj_In_Client(int iMapLoadingNum, LEVEL iStageLevel);
	HRESULT									Set_GameObject_In_Client(LEVEL iStageLevel);
	HRESULT									Set_Lights_In_Client(int iLightLoadingNum);

	void									Load_Cinemachine(int iCineNum, int iStageLevel);

	/* LEVEL 바뀌면 초기화 시키기 */
	void									Reset_Cinemachine();


private:
	HRESULT									Set_Terrain_Size(int iStageLevel);
	int										Find_Layers_Index(char* strLayer);

	HRESULT									Import_Bin_Map_Data_OnClient(MAP_TOTALINFORM_DESC* mapObjData, int iLevel);
	HRESULT									Import_Bin_Light_Data_OnClient(LIGHT_DESC_IO* lightData, int iLevel);


private:
	class CGameInstance* m_pGameInstance = { nullptr };
	CCineCamera* m_pCinemachineCam = { nullptr };

private:
	MAP_TOTALINFORM_DESC					m_MapTotalInform{};
	LIGHT_DESC_IO							m_LightTotalInform{};

	vector<wstring>							m_Layers = { TEXT("Layer_GameObjects"), TEXT("Layer_Monster") };


public:
	virtual void							Free() override;
};
END
