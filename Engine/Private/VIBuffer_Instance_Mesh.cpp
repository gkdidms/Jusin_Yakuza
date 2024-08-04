#include "VIBuffer_Instance_Mesh.h"
#include "Mesh.h"
#include "GameInstance.h"
CVIBuffer_Instance_Mesh::CVIBuffer_Instance_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CVIBuffer_Instance(pDevice, pContext)
{
}

CVIBuffer_Instance_Mesh::CVIBuffer_Instance_Mesh(const CVIBuffer_Instance_Mesh& rhs)
    :CVIBuffer_Instance(rhs)
{
}

HRESULT CVIBuffer_Instance_Mesh::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer_Instance_Mesh::Initialize(void* pArg)
{

	m_iInstanceStride = sizeof(VTXMATRIX);

	CMesh* m_Meshes = nullptr;
	if (nullptr != pArg)
	{
		INSTANCE_MESH_DESC* pDesc = static_cast<INSTANCE_MESH_DESC*>(pArg);
		m_Meshes = pDesc->pMeshs;
	}
	 
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iMaterialIndex = m_Meshes->Get_MaterialIndex();	

	m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_GIFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 2;
	m_iNumVertices = m_Meshes->Get_NumVertex();	
	m_iIndexStride = 4;
	m_iIndexCountPerInstance = /*m_Meshes->Get_NumVertex()*/m_Meshes->Get_NumIndices();	
	m_iNumIndices = m_iIndexCountPerInstance * m_InstanceDesc->iNumInstance;

#pragma region VerTex

	m_iVertexStride = sizeof(VTXMESH);
	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;	

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i], &m_Meshes->Get_Vertices()[i], sizeof(VTXMESH));
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;



#pragma region INDEX_BUFFER 

	m_Buffer_Desc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = 0;


	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
	_uint      iNumIndices = { 0 };

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		_uint iFaceCnt = m_Meshes->Get_NumIndices() / 3;
		_uint      iNumFacesIndices = { 0 };
		for (size_t j = 0; j < iFaceCnt; j++)
		{
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
		}

	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);


#pragma endregion INDEX_BUFFER


#pragma region INSTANCE_BUFFER
	// 파티클들의 월드 좌표를 저장하기 위한 버퍼
	// 좌표 뿐만 아니라 다양한 정보를 저장하여 사용한다.
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_InstanceDesc->iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	VTXMATRIX* pInstanceVertices = new VTXMATRIX[m_InstanceDesc->iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_InstanceDesc->iNumInstance);

	m_pSpeeds = new _float[m_InstanceDesc->iNumInstance];
	ZeroMemory(m_pSpeeds, sizeof(_float) * m_InstanceDesc->iNumInstance);

	m_pOriginalPositions = new _float3[m_InstanceDesc->iNumInstance];
	ZeroMemory(m_pOriginalPositions, sizeof(_float3) * m_InstanceDesc->iNumInstance);

	m_pOriginalSize = new _float[m_InstanceDesc->iNumInstance];
	ZeroMemory(m_pOriginalSize, sizeof(_float) * m_InstanceDesc->iNumInstance);

	m_pOriginalOffsets = new _float3[m_InstanceDesc->iNumInstance];
	ZeroMemory(m_pOriginalOffsets, sizeof(_float3) * m_InstanceDesc->iNumInstance);

	//돈 용 제작
	m_pOriginalAngleVelocity = new _float3[m_InstanceDesc->iNumInstance];
	ZeroMemory(m_pOriginalAngleVelocity, sizeof(_float3) * m_InstanceDesc->iNumInstance);


	m_pCurrentWorldMatrix = m_InstanceDesc->WorldMatrix;

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		_float	fRectSize = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);


		_float Speed = m_pGameInstance->Get_Random(m_InstanceDesc->vSpeed.x, m_InstanceDesc->vSpeed.y);
		_float RangeX = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.x - m_InstanceDesc->vRange.x * 0.5f, m_InstanceDesc->vPivotPos.x + m_InstanceDesc->vRange.x * 0.5f);
		_float RangeY = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.y - m_InstanceDesc->vRange.y * 0.5f, m_InstanceDesc->vPivotPos.y + m_InstanceDesc->vRange.y * 0.5f);
		_float RangeZ = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.z - m_InstanceDesc->vRange.z * 0.5f, m_InstanceDesc->vPivotPos.z + m_InstanceDesc->vRange.z * 0.5f);

		_float RadiusX = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.x - m_InstanceDesc->fRadius, m_InstanceDesc->vPivotPos.x + m_InstanceDesc->fRadius);
		_float RadiusY = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.y - m_InstanceDesc->fRadius, m_InstanceDesc->vPivotPos.y + m_InstanceDesc->fRadius);
		_float RadiusZ = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.z - m_InstanceDesc->fRadius, m_InstanceDesc->vPivotPos.z + m_InstanceDesc->fRadius);


		_float LifeTime = m_pGameInstance->Get_Random(m_InstanceDesc->vLifeTime.x, m_InstanceDesc->vLifeTime.y);


		_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

		// Look을 랜덤으로 잡아주면 나머지 계산해주기(hlsl에서)
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

		if (m_InstanceDesc->bRadius)
			pInstanceVertices[i].vTranslation = _float4(RadiusX, RadiusY, RadiusZ, 1.f);
		else
			pInstanceVertices[i].vTranslation = _float4(RangeX, RangeY, RangeZ, 1.f);

		if (m_InstanceDesc->isAura)
			m_pOriginalOffsets[i] = _float3(m_InstanceDesc->vOffsetPos.x + XMVectorGetX(WorlPosition), m_InstanceDesc->vOffsetPos.y + XMVectorGetY(WorlPosition), m_InstanceDesc->vOffsetPos.z + XMVectorGetZ(WorlPosition)); // Loop를 위해 저장해준다.
		else
			m_pOriginalOffsets[i] = _float3(m_InstanceDesc->vOffsetPos.x, m_InstanceDesc->vOffsetPos.y, m_InstanceDesc->vOffsetPos.z); // Loop를 위해 저장해준다.

		m_pOriginalPositions[i] = _float3(pInstanceVertices[i].vTranslation.x, pInstanceVertices[i].vTranslation.y, pInstanceVertices[i].vTranslation.z); // Loop를 위해 저장해준다.
		pInstanceVertices[i].vLifeTime.x = LifeTime; // 파티클이 살아있을 수 있는 시간.
		m_pOriginalSize[i] = pInstanceVertices[i].vRectSize.x = fRectSize;
		XMStoreFloat4(&pInstanceVertices[i].vDirection, XMVectorSetW(XMLoadFloat4(&pInstanceVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));
		if (m_InstanceDesc->isAura)
		{
			pInstanceVertices[i].vTranslation.x += XMVectorGetX(WorlPosition);
			pInstanceVertices[i].vTranslation.y += XMVectorGetY(WorlPosition);
			pInstanceVertices[i].vTranslation.z += XMVectorGetZ(WorlPosition);
		}
		pInstanceVertices[i].vRectSize.y = m_pGameInstance->Get_Random(0.f, 360.f);


		m_pSpeeds[i] = Speed;

	}

	m_InitialData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InitialData, &m_pVBInstance)))
		return E_FAIL;


	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

    return S_OK;
}

CVIBuffer_Instance_Mesh* CVIBuffer_Instance_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Instance_Mesh* pInstance = new CVIBuffer_Instance_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Instance_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Instance_Mesh::Clone(void* pArg)
{
	CVIBuffer_Instance_Mesh* pInstance = new CVIBuffer_Instance_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Instance_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instance_Mesh::Free()
{
	__super::Free();
}
