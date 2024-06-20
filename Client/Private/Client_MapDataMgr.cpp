#include "Client_MapDataMgr.h"
#include "GameInstance.h"
#include "MapDataMgr.h"
#include "Terrain.h"
#include "GameObject.h"
#include "Construction.h"


IMPLEMENT_SINGLETON(CClient_MapDataMgr)


CClient_MapDataMgr::CClient_MapDataMgr()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}



HRESULT CClient_MapDataMgr::Set_MapObj_In_Client(int iMapLoadingNum, int iStageLevel)
{
    CMapDataMgr::GetInstance()->Import_Bin_Map_Data_OnClient(&m_MapTotalInform, iMapLoadingNum);

    /* terrain 수정*/
    Set_Terrain_Size(iStageLevel);
    Set_GameObject_In_Client(iStageLevel);

    Safe_Delete_Array(m_MapTotalInform.pMapObjDesc);

    return S_OK;
}


HRESULT CClient_MapDataMgr::Set_GameObject_In_Client(int iStageLevel)
{
    for (int i = 0; i < m_MapTotalInform.iNumMapObj; i++)
    {
        CConstruction::MAPOBJ_DESC		mapDesc;
        mapDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
        int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

        /* Layer 정보 안들어옴 */
        if (iLayer < 0)
            return S_OK;

        mapDesc.iLayer = iLayer;
        mapDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
        mapDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;
        mapDesc.iObjType = m_MapTotalInform.pMapObjDesc[i].iObjType;

        m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Construction"), m_Layers[iLayer], &mapDesc);

    }
    return S_OK;
}


HRESULT CClient_MapDataMgr::Set_Terrain_Size(int iStageLevel)
{
    /* 땅 크기 수정*/
    CTerrain::TERRAIN_DESC terrainDesc;
    terrainDesc.vPlaneSize = m_MapTotalInform.vPlaneSize;

    if (FAILED(CGameInstance::GetInstance()->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Terrain"), TEXT("Layer_BackGround"), &terrainDesc)))
    {
        return E_FAIL;
    }

    return S_OK;
}

int CClient_MapDataMgr::Find_Layers_Index(char* strLayer)
{
    for (int i = 0; i < m_Layers.size(); i++)
    {
        if (0 == strcmp(strLayer, m_pGameInstance->WstringToString(m_Layers[i]).c_str()))
        {
            return i;
        }
    }

    return -1;
}



void CClient_MapDataMgr::Free()
{
    if (nullptr != m_MapTotalInform.pMapObjDesc)
        Safe_Delete_Array(m_MapTotalInform.pMapObjDesc);

    Safe_Release(m_pGameInstance);
}