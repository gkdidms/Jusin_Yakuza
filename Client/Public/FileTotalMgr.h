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
	/* MapTool */
	enum OBJECT_TYPE {
		CONSTRUCTION, /* �׳� ���๰ */
		ROAD,
		ITEM,
		MONSTER,
		PLAYER,
		SKY,
		LIGHT,
		OBJ_END
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

	void									Load_Cinemachine(int iCineNum, int iStageLevel);

	/* LEVEL �ٲ�� �ʱ�ȭ ��Ű�� */
	void									Reset_Cinemachine();

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	CCineCamera* m_pCinemachineCam = { nullptr };

private:
	MAP_TOTALINFORM_DESC					m_MapTotalInform{};
	LIGHT_DESC_IO							m_LightTotalInform{};
	COLLIDER_IO								m_MapCollider{};

	vector<wstring>							m_Layers = { TEXT("Layer_GameObjects"), TEXT("Layer_Monster"), TEXT("Layer_Player") };

private:
	HRESULT									Set_Terrain_Size(int iStageLevel);
	int										Find_Layers_Index(char* strLayer);

	HRESULT									Import_Bin_Map_Data_OnClient(MAP_TOTALINFORM_DESC* mapObjData, int iLevel);
	HRESULT									Import_Bin_Light_Data_OnClient(LIGHT_DESC_IO* lightData, int iLevel);

	HRESULT									Import_Bin_Collider_Data_OnTool(COLLIDER_IO* ColliderData, int iLevel);


public:
	virtual void							Free() override;
};
END
