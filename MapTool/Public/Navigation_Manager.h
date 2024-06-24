#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Cell.h"
#include "Navigation.h"
#include "Shader.h"

BEGIN(Engine)
class CGameInstance;
END

class CNavigation_Manager final : public CBase
{
	DECLARE_SINGLETON(CNavigation_Manager);

private:
	CNavigation_Manager();
	virtual ~CNavigation_Manager() = default;

public:
	HRESULT					Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void					Priority_Tick(_float fTimeDelta);
	void					Tick(_float fTimeDelta);
	void					Late_Tick(_float fTimeDelta);
	HRESULT					Render();

public:
	TCHAR*					StringToTCHAR(string& s);

public:
	void					Load_Cell_IMGUI();
	void					Make_Point(_vector vPickingPos);
	HRESULT					Make_Cell();

	HRESULT					Save_Cells(_uint iIndex);
	HRESULT					Load_Cells(_uint iIndex);

	void					Show_FileName();
	void					Update_FileName();

	void					Delete_Cell(_uint iIndex);
	void					Delete_AllCell();

	/* cell 수정하는 부분 */
	void					Find_Cells();

private:
	_float3					Find_ClosestPoint(_vector pickPos, _float* pMinDistance);
	_float3					Find_ClosestCells(_vector pickPos, _float* pMinDistance);


	/* For IMGUI_MANAGER */
public:
	void					Show_Cells_IMGUI();

private:
	void					Update_CellsName();



private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CShader* m_pShader = nullptr;

	class CGameInstance* m_pGameInstance = { nullptr };

private:
	vector<char*>							m_FileNames;

private:
	vector<class CCell*>					m_Cells;
	static _float4x4						m_WorldMatrix; /* 객체들이 공유할 수 있게끔 static */
	class CNavigation*						m_pNavigationCom = { nullptr };

private:
	vector<char*>							m_CellsName;

private:
	vector<_float3>							m_vPoints;

	/* For IMGUI_MANAGER */
private:
	_int									m_iCurrentCellIndex = { 0 };
	_int									m_iCurrentFileNavi = { 0 };
	_int									m_iCurrentOption = { 0 };


public:
	virtual void Free() override;
};

