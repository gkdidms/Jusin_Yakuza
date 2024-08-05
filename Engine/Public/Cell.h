#pragma once
#include "Base.h"

/* 1. �׺���̼��� �����ϴ� �ϳ��� �ﰡ��. */
/* 2. �ﰢ���� �����ϴ� ������ ������ �����Ѵ�. */
/* 3. �� ���� �����Ͽ� ��ü�� �����ġ�� �ȿ� �ִ���? ������? ����.  */

BEGIN(Engine)
class ENGINE_DLL CCell :
    public CBase
{
public:
    enum TYPE { POINT_A, POINT_B, POINT_C, POINT_END };
    enum LINE_TYPE { LINE_AB, LINE_BC, LINE_CA, LINE_END };

    enum OPTION { OPTION_NONE, OPTION_STAIRS, OPTION_END }; // �� Ÿ�� (�Ϲ�, ���)

private:
    CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CCell() = default;

public:
    _vector Get_Point(TYPE eType) { return XMLoadFloat3(&m_vPoints[eType]); }
    _int Get_Index() { return m_iIndex; }
    OPTION Get_Option() { return m_OptionType; }
    _int Get_NeighborIndex(_uint eType) { return m_iNeighborIndices[eType]; }
    _int Get_ParentIndex() { return m_iParentIndex; }
    _vector Get_WayPoint() const { return XMLoadFloat3(&m_vWayPoint); }
    _uint Get_NeighborLine(_uint iNeighborIndex) {
        for (size_t i = 0; i < LINE_END; ++i)
        {
            if (iNeighborIndex == m_iNeighborIndices[i])
                return i;
        }

        return LINE_END;
    }

public:
    void    Set_Option(CCell::OPTION  iOption) {m_OptionType = iOption;}
    void Set_ParentIndex(_uint iIndex) { m_iParentIndex = iIndex; }

public:
    HRESULT Initialize(const _float3* pPoints, _int iIndex, OPTION OptionType);
    _bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
    _float Compute_Height(_fvector vPosition);
    _bool isIn(_fvector vPosition, _int* pNeighborsIndex, _vector&  vNormal);
    void SetUp_Neighbor(LINE_TYPE eNeighbor, CCell* pCell) {
        m_iNeighborIndices[eNeighbor] = pCell->m_iIndex;
    }

#ifdef _DEBUG
public:
    HRESULT Render();
#endif // _DEBUG

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

private:
    _float3 m_vPoints[POINT_END] = {}; // �ﰢ���� ��ǥ�� ����
    _int m_iNeighborIndices[LINE_END] = { -1, -1, -1 }; // ������ cell index�� ����

    _int m_iIndex = { -1 };
    OPTION m_OptionType = { OPTION_END }; // ���� �ɼ��� ����

private:
    _float3 m_vWayPoint = {};
    _int m_iParentIndex = { -1 };
#ifdef _DEBUG
private:
    class CVIBuffer_Cell* m_pVIBufferCom = { nullptr };
#endif // _DEBUG

public:
    static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex, OPTION OptionType);
    virtual void Free() override;
};

END