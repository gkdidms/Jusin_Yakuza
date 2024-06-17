#include "VIBuffer_Instance_Point.h"

CVIBuffer_Instance_Point::CVIBuffer_Instance_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Instance_Point::CVIBuffer_Instance_Point(const CVIBuffer_Instance_Point& rhs)
	: CVIBuffer_Instance{ rhs }
{
}

HRESULT CVIBuffer_Instance_Point::Initialize_Prototype(const INSTANCE_DESC& InstanceDesc)
{
	if (FAILED(__super::Initialize_Prototype(InstanceDesc)))
		return E_FAIL;

	m_InstanceDesc = InstanceDesc;

	uniform_real_distribution<float>	Size(InstanceDesc.vSize.x, InstanceDesc.vSize.y);
	_float	fSize = Size(m_RandomNumber);

	m_GIFormat = DXGI_FORMAT_R16_UINT;
	m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOINT);

	m_iIndexStride = 2;
	m_iIndexCountPerInstance = 1;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_iInstanceStride = sizeof(VTXMATRIX);

#pragma region VERTEX_BUFFER
	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXPOINT* pVertexts = new VTXPOINT[m_iNumVertices];

	pVertexts[0].vPosition = _float3{ 0.f, 0.f, 0.f };
	pVertexts[0].vPSize = _float2{ fSize, fSize };

	m_ResourceData.pSysMem = pVertexts;

	__super::Create_Buffer(&m_pVB);

	Safe_Delete_Array(pVertexts);

#pragma endregion


#pragma region INDEX_BUFFER
	m_Buffer_Desc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	m_ResourceData.pSysMem = pIndices;

	__super::Create_Buffer(&m_pIB);

	Safe_Delete_Array(pIndices);
#pragma endregion

#pragma region INSTANCE_BUFFER
	// ��ƼŬ���� ���� ��ǥ�� �����ϱ� ���� ����
	// ��ǥ �Ӹ� �ƴ϶� �پ��� ������ �����Ͽ� ����Ѵ�.
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	VTXMATRIX* pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_iNumInstance);

	m_pSpeeds = new _float[m_iNumInstance];
	ZeroMemory(m_pSpeeds, sizeof(_float) * m_iNumInstance);

	m_pPower = new _float[m_iNumInstance];
	ZeroMemory(m_pPower, sizeof(_float) * m_iNumInstance);

	m_pAccelTime = new _float[m_iNumInstance];
	ZeroMemory(m_pAccelTime, sizeof(_float) * m_iNumInstance);

	m_pOriginalPositions = new _float3[m_iNumInstance];
	ZeroMemory(m_pOriginalPositions, sizeof(_float3) * m_iNumInstance);

	uniform_real_distribution<float>	RangeX(InstanceDesc.vPivotPos.x - InstanceDesc.vRange.x * 0.5f, InstanceDesc.vPivotPos.x + InstanceDesc.vRange.x * 0.5f);
	uniform_real_distribution<float>	RangeY(InstanceDesc.vPivotPos.y - InstanceDesc.vRange.y * 0.5f, InstanceDesc.vPivotPos.y + InstanceDesc.vRange.y * 0.5f);
	uniform_real_distribution<float>	RangeZ(InstanceDesc.vPivotPos.z - InstanceDesc.vRange.z * 0.5f, InstanceDesc.vPivotPos.z + InstanceDesc.vRange.z * 0.5f);


	uniform_real_distribution<float>	Speed(InstanceDesc.vSpeed.x, InstanceDesc.vSpeed.y);
	uniform_real_distribution<float>	Power(InstanceDesc.vPower.x, InstanceDesc.vPower.y);

	uniform_real_distribution<float>	LifeTime(InstanceDesc.vLifeTime.x, InstanceDesc.vLifeTime.y);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		
		// Right, Up, Loop, Pos ������ ���� ����� ��ǥ�� �־��ش�.
		pInstanceVertices[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, fSize, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, fSize, 0.f);
		pInstanceVertices[i].vTranslation = _float4(RangeX(m_RandomNumber), RangeY(m_RandomNumber), RangeZ(m_RandomNumber), 1.f);
		m_pOriginalPositions[i] = _float3(pInstanceVertices[i].vTranslation.x, pInstanceVertices[i].vTranslation.y, pInstanceVertices[i].vTranslation.z); // Loop�� ���� �������ش�.
		pInstanceVertices[i].vLifeTime.x = LifeTime(m_RandomNumber); // ��ƼŬ�� ������� �� �ִ� �ð�.

		m_pPower[i] = Power(m_RandomNumber);
		m_pSpeeds[i] = Speed(m_RandomNumber);
		m_pAccelTime[i] = 0.f;
	}
	m_ResourceData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_ResourceData, &m_pVBInstance)))
		return E_FAIL;


	Safe_Delete_Array(pInstanceVertices);
#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_Instance_Point::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Instance_Point* CVIBuffer_Instance_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC& InstanceDesc)
{
	CVIBuffer_Instance_Point* pInstance = new CVIBuffer_Instance_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(InstanceDesc)))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Instance_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Instance_Point::Clone(void* pArg)
{
	CVIBuffer_Instance_Point* pInstance = new CVIBuffer_Instance_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Instance_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instance_Point::Free()
{
	__super::Free();
}
