#pragma once

#include "Base.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CMapDataMgr final : public CBase
{
	DECLARE_SINGLETON(CMapDataMgr);

private:
	CMapDataMgr();
	virtual ~CMapDataMgr() = default;

public:
	HRESULT			Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void			Priority_Tick(_float fTimeDelta);
	void			Tick(_float fTimeDelta);
	void			Late_Tick(_float fTimeDelta);
	void			Render();

public:
	HRESULT			Save_Bin_Map_Data(MAP_TOTALINFORM_DESC* mapObjData);
	HRESULT			Load_Bin_Map_Data_OnClient(MAP_TOTALINFORM_DESC* mapObjData, int iLevel);
	HRESULT			Load_Bin_Map_Data_OnTool(MAP_TOTALINFORM_DESC* mapObjData, char* fileName);

	HRESULT			Set_MapObj_In_Client(int iLevel);
	HRESULT			Set_MapObj_In_Tool(int iLevel);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END