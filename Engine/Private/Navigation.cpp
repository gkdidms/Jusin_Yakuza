#include "Navigation.h"

#include "Shader.h"
#include "GameInstance.h"

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& rhs)
    : CComponent{ rhs },
    m_pShaderCom{ rhs.m_pShaderCom },
    m_Cells{ rhs.m_Cells },
    m_Routes{ rhs.m_Routes }
{
    Safe_AddRef(m_pShaderCom);

    for (auto& pCell : m_Cells)
        Safe_AddRef(pCell);
}

void CNavigation::Set_Points(const _float3* vPoints, _int OptionType)
{
    CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_iIndexCount++, CCell::OPTION(OptionType));
    if (nullptr == pCell)
        return;

    m_Cells.emplace_back(pCell);
}

vector<ROUTE_IO> CNavigation::Get_RouteIndexs(_int iIndex)
{
    return m_Routes.find(iIndex)->second;
}

HRESULT CNavigation::Initialize_Prototype()
{
    m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    return S_OK;
}

HRESULT CNavigation::Initialize_Prototype(const wstring strFilePath)
{
    m_pShaderCom = CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_Cell.hlsl", VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    // 파일 열어서 가져오기 
    if (FAILED(Load_File(strFilePath)))
        return E_FAIL;

    if (FAILED(SetUp_Neighbors()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);
        m_iCurrentLine = pDesc->iCurrentLine;
        m_iCurrentWayPointIndex = pDesc->iWayPointIndex;

        if (m_iCurrentWayPointIndex == -1) Find_WayPointIndex(pDesc->vPosition);
    }

    return S_OK;
}

void CNavigation::Tick()
{
}

int CNavigation::Find_PlayerMonster_Index(_fvector vTargetPos)
{
    for (int i = 0; i < m_Cells.size(); i++)
    {
        /* 그냥 넣어주는거 */
        int			iNeighborsIndex;

        if (true == m_Cells[i]->isIn(vTargetPos, &iNeighborsIndex))
        {
            return i;
        }
    }

    return -1;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    m_WorldMatrix.m[3][1] += 0.2f;
    m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

    m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
    m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

    for (auto& pCell : m_Cells)
    {
        _vector vColor{};
        if (pCell->Get_Option() == CCell::OPTION_NONE)
        {
            vColor = XMVectorSet(0.f, 0.f, 1.f, 1.f);
            m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector));
        }

        else if (pCell->Get_Option() == CCell::OPTION_STAIRS)
        {
            vColor = XMVectorSet(1.f, 0.f, 0.f, 1.f);
            m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector));
        }

        m_pShaderCom->Begin(0);

        pCell->Render();
    }

    return S_OK;
}

_bool CNavigation::isRemove(_int iIndex)
{
    for (auto iter = m_Cells.begin(); iter != m_Cells.end();)
    {
        if ((*iter)->Get_Index() == iIndex)
        {
            Safe_Release(*iter);
            m_Cells.erase(iter);

            return true;
        }
        else ++iter;
    }
    return false;
}
#endif // _DEBUG



HRESULT CNavigation::Load_File(const wstring strFilePath)
{
    ifstream ifs(strFilePath, ios::binary | ios::in);

    if (ifs.fail())
        return E_FAIL;

    _uint NumCells = { 0 };
    ifs.read((_char*)&NumCells, sizeof(_uint));

    for (size_t i = 0; i < NumCells; ++i)
    {
        _float3* vPoint = new _float3[3];
        ifs.read((_char*)vPoint, sizeof(_float3) * 3);

        _int iOption = { 0 };
        ifs.read((_char*)&iOption, sizeof(_int));

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoint, i, CCell::OPTION(iOption));

        m_Cells.emplace_back(pCell);
        Safe_Delete(vPoint);
    }

    // 루트 개수 읽어오기
    _uint		iRouteCnt = 0;
    ifs.read((_char*)&iRouteCnt, sizeof(_uint));

    for (int i = 0; i < iRouteCnt; i++)
    {
        _uint		iCellCnt = 0;

        ifs.read((_char*)&iCellCnt, sizeof(_uint));

        ROUTE_IO* arr = new ROUTE_IO[iCellCnt];
        ifs.read(reinterpret_cast<char*>(arr), iCellCnt * sizeof(ROUTE_IO));

        vector<ROUTE_IO>		routeCells;

        for (int j = 0; j < iCellCnt; j++)
        {
            routeCells.push_back(arr[j]);
        }

        m_Routes.emplace(i, routeCells);

        Safe_Delete(arr);
    }

    ifs.close();

    return S_OK;
}

HRESULT CNavigation::SetUp_Neighbors()
{
    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDescCell : m_Cells)
        {
            if (pSourCell == pDescCell)
                continue;

            if (pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
                pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDescCell);

            if (pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
                pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDescCell);

            if (pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
                pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDescCell);
        }
    }

    return S_OK;
}

void CNavigation::Find_WayPointIndex(_vector vPosition)
{
    //포지션으로 웨이포인트 인덱스 만들기
    _float fMinDistance = 0.f;
    _uint iIndex = 0;
    for (auto& vWayPoint : m_Routes[m_iCurrentLine])
    {
        _float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vWayPoint.vPosition) - vPosition));

        ++iIndex;

        if (fMinDistance == 0.f || fMinDistance >= fDistance)
        {
            if (iIndex >= m_Routes[m_iCurrentLine].size())
                iIndex = 0;

            m_iCurrentWayPointIndex = iIndex;
            m_iPreWayPointIndex = iIndex == 0 ? m_Routes[m_iCurrentLine].size() - 1 : iIndex - 1;
            fMinDistance = fDistance;
        }
    }

    m_vNextDir = XMVector3Normalize(XMLoadFloat4(&m_Routes[m_iCurrentLine][m_iCurrentWayPointIndex].vPosition) - vPosition);
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring strFilePath)
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strFilePath)))
    {
        MSG_BOX("Failed To Created : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
    CNavigation* pInstance = new CNavigation(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

_bool CNavigation::isMove(_fvector vMovePos)
{
    _int iNeighborsIndex = { -1 };

    // 셀 안에 있는지?
    if (m_Cells[m_iCurrentIndex]->isIn(vMovePos, &iNeighborsIndex))
        return true;
    else
    {
        if (-1 != iNeighborsIndex)
        {
            // 셀 밖에 있는지?
            while (true)
            {
                if (-1 == iNeighborsIndex)
                    return false;

                if (m_Cells[iNeighborsIndex]->isIn(vMovePos, &iNeighborsIndex))
                {
                    m_iCurrentIndex = iNeighborsIndex;
                    return true;
                }

            }
        }
        else
            return false;
    }
}

_vector CNavigation::Compute_WayPointDir(_vector vPosition, const _float& fTimeDelta)
{
    //현재 플레이어의 위치에서 다음 웨이포인트까지의 방향벡터를 구한다.
    _vector vCurrentWayPoint = XMLoadFloat4(&m_Routes[m_iCurrentLine][m_iCurrentWayPointIndex].vPosition);

    //XMStoreFloat4(vMovePos, XMVectorLerp(vPosition, vCurrentWayPoint, fTimeDelta * 20.f));
    
    _vector vDir = vCurrentWayPoint - vPosition;
    _float fDistance = XMVectorGetX(XMVector3Length(vDir));

    _vector vResultDir;
    if (m_vPreDir.m128_f32[0] == 0.f)
    {
        vResultDir = XMVector3Normalize(vDir);
    }
    else
    {
        m_fTime += fTimeDelta * 6.f;
        vResultDir = XMVectorLerp(m_vPreDir, m_vNextDir, m_fTime >= 1.f ? 1.f : m_fTime);
    }

    //특정 거리보다 작다면 다음 웨이포인트로 이동한다.
    if (fDistance <= m_fMaxDistance)
    {
        m_iCurrentWayPointIndex++;

        // 인덱스가 배열의 길이보다 크다면 다시 초기값으로 돌아간다.
        if (m_iCurrentWayPointIndex >= m_Routes[m_iCurrentLine].size())
            m_iCurrentWayPointIndex = 0.f;

        m_vPreDir = XMVector3Normalize(vDir);
        m_vNextDir = XMVector3Normalize(XMLoadFloat4(&m_Routes[m_iCurrentLine][m_iCurrentWayPointIndex].vPosition) - XMLoadFloat4(&m_Routes[m_iCurrentLine][m_iPreWayPointIndex].vPosition));
        m_fTime = 0.f;

        m_iPreWayPointIndex = m_iCurrentWayPointIndex;
    }

    return vResultDir;
}

_float CNavigation::Compute_Height(_fvector vPosition)
{
    // 예외처리
    if (-1 == m_iCurrentIndex)
        m_iCurrentIndex = 0;

    return m_Cells[m_iCurrentIndex]->Compute_Height(vPosition);
}

void CNavigation::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);


    for (auto& Pair : m_Routes)
        Pair.second.clear();
    m_Routes.clear();

    for (auto& pCell : m_Cells)
        Safe_Release(pCell);
    m_Cells.clear();
}