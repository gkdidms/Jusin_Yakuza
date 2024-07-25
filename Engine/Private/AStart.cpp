#include "AStart.h"

#include "GameInstance.h"

CAStart::CAStart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent {pDevice, pContext}
{
}

CAStart::CAStart(const CAStart& rhs)
	: CComponent { rhs }
{
}

HRESULT CAStart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAStart::Initialize(void* pArg)
{
#ifdef _DEBUG
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Line.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShaderCom)
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

void CAStart::Start_Root(CNavigation* pNavi, _fvector vGoalPos)
{
	_int iStartIndex = pNavi->Get_Index();
	_int iGoalIndex = pNavi->Find_PlayerMonster_Index(vGoalPos);
	if (iGoalIndex == -1)
		return;

	Data_Release();
#ifdef _DEBUG
	Safe_Release(m_pVIBufferCom);
#endif // _DEBUG

	vector<CCell*> Cells = pNavi->Get_Cells();

	if (Make_Route(iStartIndex, iGoalIndex, Cells))
	{
		Make_BastList(iStartIndex, iGoalIndex, Cells);
		
#ifdef _DEBUG
		m_pVIBufferCom = CVIBuffer_Line::Create(m_pDevice, m_pContext, m_BestList);
		if (nullptr == m_pVIBufferCom)
			MSG_BOX("VIBuffer_Lint 생성 불가");
#endif // _DEBUG
	}
}

#ifdef _DEBUG
HRESULT CAStart::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix.m[3][1] += 0.2f;
	m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

	_vector vColor = { 1.f, 0.f, 0.f, 1.f };
	m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector));

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	return S_OK;
}
#endif // _DEBUG

//인접한 셀의 코스트 구하기
_bool CAStart::Make_Route(_uint iStartIndex, _uint iGoalIndex, vector<class CCell*> Cells)
{
	_uint iStartIdx = iStartIndex;

	while (true)
	{
		if (!m_OpenList.empty())
			m_OpenList.pop_front();

		m_CloseList.push_back(iStartIdx);

		for (size_t i = 0; i < CCell::LINE_END; ++i)
		{
			_int iNeighborIndex = Cells[iStartIdx]->Get_NeighborIndex(i);
			if (iNeighborIndex == -1)
				continue;

			CCell* pCell = Cells[iNeighborIndex];

			if (iGoalIndex == pCell->Get_Index())
			{
				pCell->Set_ParentIndex(iStartIdx);
				return true;
			}

			if (false == Check_Close(pCell->Get_Index()) &&
				false == Check_Open(pCell->Get_Index())) {
				pCell->Set_ParentIndex(iStartIdx);
				m_OpenList.push_back(pCell->Get_Index());
			}
		}

		if (m_OpenList.empty())
			return false;

		_uint iStart = iStartIndex;

		m_OpenList.sort([&](int Dst, int Src) -> _bool {
			//Cost = PCost + GCost

			_vector vPCost1 = Cells[iStart]->Get_WayPoint() - Cells[Dst]->Get_WayPoint();
			_vector vPCost2 = Cells[iStart]->Get_WayPoint() - Cells[Src]->Get_WayPoint();

			_vector vGCost1 = Cells[iGoalIndex]->Get_WayPoint() - Cells[Dst]->Get_WayPoint();
			_vector vGCost2 = Cells[iGoalIndex]->Get_WayPoint() - Cells[Src]->Get_WayPoint();

			_float fCost1 = XMVectorGetX(XMVector3Length(vPCost1)) + XMVectorGetX(XMVector3Length(vGCost1));
			_float fCost2 = XMVectorGetX(XMVector3Length(vPCost2)) + XMVectorGetX(XMVector3Length(vGCost2));

			return fCost1 < fCost2;
		});

		iStartIdx = m_OpenList.front();
	}
}

void CAStart::Make_BastList(_uint iStartIndex, _uint iGoalIndex, vector<class CCell*> Cells)
{
	m_BestList.push_front(Cells[iGoalIndex]);

	_uint iRouteIndex = Cells[iGoalIndex]->Get_ParentIndex();

	while (true)
	{
		if (iRouteIndex == iStartIndex)
			break;

		m_BestList.push_front(Cells[iRouteIndex]);
		iRouteIndex = Cells[iRouteIndex]->Get_ParentIndex();
	}
}

_bool CAStart::Check_Close(_uint iIndex)
{
	for (_uint& iCloseIndex : m_CloseList)
	{
		if (iIndex == iCloseIndex)
			return true;
	}

	return false;
}

_bool CAStart::Check_Open(_uint iIndex)
{
	for (_uint& iOpenIndex : m_OpenList)
	{
		if (iIndex == iOpenIndex)
			return true;
	}

	return false;
}

void CAStart::Data_Release()
{
	m_OpenList.clear();
	m_CloseList.clear();
	m_BestList.clear();
}

CAStart* CAStart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAStart* pInstance = new CAStart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent* CAStart::Clone(void* pArg)
{
	CAStart* pInstance = new CAStart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAStart::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
#endif // _DEBUG
}
