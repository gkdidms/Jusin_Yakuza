#include "FileTotalMgr.h"
#include "GameInstance.h"
#include "MapDataMgr.h"
#include "Terrain.h"
#include "GameObject.h"
#include "Construction.h"
#include "SystemManager.h"

IMPLEMENT_SINGLETON(CFileTotalMgr)


CFileTotalMgr::CFileTotalMgr()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CFileTotalMgr::Tick(_float fTimeDelta)
{
    //if (nullptr != m_pCinemachineCam)
    //    m_pCinemachineCam->Tick(fTimeDelta);
}

void CFileTotalMgr::Late_Tick(_float fTimeDelta)
{
    //if (nullptr != m_pCinemachineCam)
    //    m_pCinemachineCam->Late_Tick(fTimeDelta);
}



HRESULT CFileTotalMgr::Set_MapObj_In_Client(int iMapLoadingNum, LEVEL iStageLevel)
{
    Safe_Delete_Array(m_MapTotalInform.pMapObjDesc);

    Import_Bin_Map_Data_OnClient(&m_MapTotalInform, iMapLoadingNum);

    /* terrain 수정*/
    Set_Terrain_Size(iStageLevel);
    Set_GameObject_In_Client(iStageLevel);

    //Safe_Delete_Array(m_MapTotalInform.pMapObjDesc);

    return S_OK;
}


HRESULT CFileTotalMgr::Set_GameObject_In_Client(LEVEL iStageLevel)
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

HRESULT CFileTotalMgr::Set_Lights_In_Client(int iLightLoadingNum)
{
    Safe_Delete_Array(m_LightTotalInform.pLightDesc);

    /* 모든 라이트 삭제 */
    m_pGameInstance->Delete_AllLights();

    Import_Bin_Light_Data_OnClient(&m_LightTotalInform, iLightLoadingNum);

    for (int i = 0; i < m_LightTotalInform.iNumLights; i++)
    {
        LIGHT_DESC		lightDesc;
        lightDesc = m_LightTotalInform.pLightDesc[i];

        /* gameinstance에 추가 */
        m_pGameInstance->Add_Light(lightDesc);
    }

    return S_OK;
}

void CFileTotalMgr::Load_Cinemachine(int iCineNum, int iStageLevel)
{
    if (nullptr == m_pCinemachineCam)
    {
        CCineCamera::CINE_CAMERA_DESC		cineDesc;
        cineDesc.iFileNum = iCineNum;
        m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_CCineCamera"), TEXT("Layer_Camera"), &cineDesc);
        m_pCinemachineCam = dynamic_cast<CCineCamera*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Camera"), CAMERA::CAMERA_CINEMACHINE));
        //m_pCinemachineCam = dynamic_cast<CCineCamera*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_CCineCamera"), &cineDesc));
    }
    else
    {
        m_pCinemachineCam->Load_CamBin(iCineNum);
        m_pCinemachineCam->Setting_Start_Cinemachine();
    }

    CSystemManager::GetInstance()->Set_Camera(CAMERA::CAMERA_CINEMACHINE);
}

void CFileTotalMgr::Reset_Cinemachine()
{
    if (nullptr != m_pCinemachineCam)
        Safe_Release(m_pCinemachineCam);
}


HRESULT CFileTotalMgr::Set_Terrain_Size(int iStageLevel)
{
    /* 땅 크기 수정*/
    CTerrain::TERRAIN_DESC terrainDesc;
    terrainDesc.vPlaneSize = m_MapTotalInform.vPlaneSize;

    if (FAILED(m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Terrain"), TEXT("Layer_BackGround"), &terrainDesc)))
    {
        return E_FAIL;
    }

    return S_OK;
}

int CFileTotalMgr::Find_Layers_Index(char* strLayer)
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

HRESULT CFileTotalMgr::Import_Bin_Map_Data_OnClient(MAP_TOTALINFORM_DESC* mapObjData, int iLevel)
{
    char fullPath[MAX_PATH];
    strcpy_s(fullPath, "../Bin/DataFiles/MapData/");

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

HRESULT CFileTotalMgr::Import_Bin_Light_Data_OnClient(LIGHT_DESC_IO* lightData, int iLevel)
{
    char fullPath[MAX_PATH];
    strcpy_s(fullPath, "../Bin/DataFiles/LightData/");

    strcat_s(fullPath, "Light_Data");
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


    in.read((char*)&lightData->iNumLights, sizeof(int));

    lightData->pLightDesc = new LIGHT_DESC[lightData->iNumLights];

    for (int i = 0; i < lightData->iNumLights; i++)
    {
        in.read((char*)&lightData->pLightDesc[i], sizeof(LIGHT_DESC));
    }

    in.close();

    return S_OK;
}




void CFileTotalMgr::Free()
{
    Safe_Delete_Array(m_MapTotalInform.pMapObjDesc);
    Safe_Delete_Array(m_LightTotalInform.pLightDesc);

    m_Layers.clear();

    if (nullptr != m_pCinemachineCam)
        Safe_Release(m_pCinemachineCam);

    Safe_Release(m_pGameInstance);
}