#include "../Public/MapDataMgr.h"
#include "GameInstance.h"
#include "GameObject.h"

#include <string.h>
#include "Engine_Defines.h"

IMPLEMENT_SINGLETON(CMapDataMgr)


char* StringToChar(string s)
{
	_char chr[MAX_PATH];
	strcpy_s(chr, s.c_str());
	return chr;
}

/*
string WstrToStr(const wstring& source)
{
	return string().assign(source.begin(), source.end());
}

wstring StrToWstr(const string& source)
{
	return wstring().assign(source.begin(), source.end());
}?
*/


CMapDataMgr::CMapDataMgr()
{
}

HRESULT CMapDataMgr::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);

	return S_OK;
}

void CMapDataMgr::Priority_Tick(_float fTimeDelta)
{
}

void CMapDataMgr::Tick(_float fTimeDelta)
{
}

void CMapDataMgr::Late_Tick(_float fTimeDelta)
{
}

void CMapDataMgr::Render()
{
}

HRESULT CMapDataMgr::Export_Bin_Map_Data(MAP_TOTALINFORM_DESC* mapObjData)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/MapData/");

	strcat_s(fullPath, "MapObject_Data");

	char cLevel[2] = "";
	_itoa_s(mapObjData->iLevelIndex, cLevel, 10);
	strcat_s(fullPath, "_");
	strcat_s(fullPath, cLevel);
	strcat_s(fullPath, ".dat");

	fs::path path(fullPath);

	//if (!exists(path))
	//	fs::create_(path);

	ofstream out(fullPath, ios::binary);
	//file.open(fullPath, ios::out | ios::binary);

	if (!out) {
		return E_FAIL;
	}


	int		iLevel = mapObjData->iLevelIndex;
	out.write((char*)&iLevel, sizeof(int));

	XMFLOAT2 planeSize = mapObjData->vPlaneSize;
	out.write((char*)&planeSize, sizeof(XMFLOAT2));


	int iNumMapObj = mapObjData->iNumMapObj;
	out.write((char*)&iNumMapObj, sizeof(int));

	for (int i = 0; i < mapObjData->iNumMapObj; i++)
	{
		OBJECTPLACE_DESC PObjPlaceDesc = mapObjData->pMapObjDesc[i];
		out.write((char*)&PObjPlaceDesc, sizeof(OBJECTPLACE_DESC));
	}

	out.close();

	return S_OK;
}

HRESULT CMapDataMgr::Import_Bin_Map_Data_OnClient(MAP_TOTALINFORM_DESC* mapObjData, int iLevel)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/MapData/");

	strcat_s(fullPath, "MapObject_Data");
	strcat_s(fullPath, "_");

	char cLevel[2] = "";
	_itoa_s(iLevel, cLevel, 10);
	strcat_s(fullPath, cLevel);
	strcat_s(fullPath, ".dat");

	ifstream in(fullPath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("파일 개방 실패");
		return E_FAIL;
	}


	in.read((char*)&mapObjData->iLevelIndex, sizeof(int));

	in.read((char*)&mapObjData->vPlaneSize, sizeof(XMFLOAT2));

	in.read((char*)&mapObjData->iNumMapObj, sizeof(int));

	mapObjData->pMapObjDesc = new OBJECTPLACE_DESC[mapObjData->iNumMapObj];

	for (int i = 0; i < mapObjData->iNumMapObj; i++)
	{
		in.read((char*)&mapObjData->pMapObjDesc[i], sizeof(OBJECTPLACE_DESC));
	}

	in.close();

	return S_OK;
}

HRESULT CMapDataMgr::Import_Bin_Map_Data_OnTool(MAP_TOTALINFORM_DESC* mapObjData, char* fileName)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/MapData/");

	strcat_s(fullPath, fileName);

	/*char cLevel[2] = "";
	_itoa_s(mapObjData->iLevelIndex, cLevel, 10);
	strcat_s(fullPath, cLevel);
	strcat_s(fullPath, ".bin");*/


	ifstream in(fullPath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("파일 개방 실패");
		return E_FAIL;
	}


	in.read((char*)&mapObjData->iLevelIndex, sizeof(int));

	in.read((char*)&mapObjData->vPlaneSize, sizeof(XMFLOAT2));

	in.read((char*)&mapObjData->iNumMapObj, sizeof(int));

	mapObjData->pMapObjDesc = new OBJECTPLACE_DESC[mapObjData->iNumMapObj];

	for (int i = 0; i < mapObjData->iNumMapObj; i++)
	{
		in.read((char*)&mapObjData->pMapObjDesc[i], sizeof(OBJECTPLACE_DESC));
	}

	in.close();

	return S_OK;
}



void CMapDataMgr::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
