#include "..\Public\VIBuffer_AABBCube.h"

CVIBuffer_AABBCube::CVIBuffer_AABBCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CVIBuffer_AABBCube::CVIBuffer_AABBCube(const CVIBuffer_AABBCube& rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_AABBCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_AABBCube::Initialize(void* pArg)
{

	// ���� ������Ʈ ũ�⿡ ���� ť�� �׷��ֱ�
	AABBCUBE_DESC* aabbDesc = (AABBCUBE_DESC*)pArg;

	m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_GIFormat = DXGI_FORMAT_R16_UINT;
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXCUBE_OCCULUSION);
	m_iNumVertices = 8;

	m_iIndexStride = 2;
	m_iNumIndices = 36;

#pragma region VERTEX_BUFFER 

	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXCUBE_OCCULUSION* pVertices = new VTXCUBE_OCCULUSION[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXCUBE_OCCULUSION) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = pVertices[0].vPosition;

	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexcoord = pVertices[1].vPosition;

	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = pVertices[2].vPosition;

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexcoord = pVertices[3].vPosition;

	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexcoord = pVertices[4].vPosition;

	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexcoord = pVertices[5].vPosition;

	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexcoord = pVertices[6].vPosition;

	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexcoord = pVertices[7].vPosition;

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

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

	/* +x */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* -x */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;

	/* +y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* -y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* +z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* -z */  // ����� -z�� ��Ÿ���ϴ�.
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	if (FAILED(Ready_AABBCubeBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_AABBCube::Render()
{
	ID3D11Buffer* pVertices[] = {
		m_pVB,
	};

	_uint pStrideVertices[] = {
		m_iVertexStride,
	};

	_uint pStartVertices[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertices, pStrideVertices, pStartVertices);
	m_pContext->IASetIndexBuffer(m_pIB, m_GIFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_Primitive_Topology);

	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	m_pContext->CopyResource(m_pSRVIn, m_pVB);

	return S_OK;
}

void CVIBuffer_AABBCube::Copy_ResultResources(ID3D11Buffer* pBuffer)
{
	m_pContext->CopyResource(pBuffer, m_pUAVOut);
}

CVIBuffer_AABBCube* CVIBuffer_AABBCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_AABBCube* pInstance = new CVIBuffer_AABBCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_AABBCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_AABBCube::Clone(void* pArg)
{
	CVIBuffer_AABBCube* pInstance = new CVIBuffer_AABBCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_AABBCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_AABBCube::Free()
{
	__super::Free();
}
