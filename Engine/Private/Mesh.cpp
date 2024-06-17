#include "Mesh.h"

#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer{ rhs }
{
}

void CMesh::Fill_Matrices(const vector<CBone*>& Bones, _float4x4* BoneTransformMatrix)
{
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat4x4(&BoneTransformMatrix[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix()));
	}
}

HRESULT CMesh::Initialize_Prototype(MESHTYPE eMeshType, _fmatrix PreTransformMatrix, const char* pName, _int iMaterialIndex, _int iNumVertices, _int iNumIndices, _int iNumFaces, unsigned int* Indices, vector<VTXANIMMESH> AnimMeshed, vector<VTXMESH> Meshed, _int iNumBones, vector<_float4x4> OffsetMatrices, vector<_int> BoneIndices, _int iNumWeight, vector<class CBone*> Bones)
{
	strcpy_s(m_szName, pName);

	m_iNumVertexBuffers = 1;
 	m_iMaterialIndex = iMaterialIndex;

	m_GIFormat = DXGI_FORMAT_R32_UINT;
	m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iNumVertices = iNumVertices;

	eMeshType == TYPE_NONANIM ? Ready_Vertices_For_NonAnim(Meshed, PreTransformMatrix) : Ready_Vertices_For_Anim(AnimMeshed, iNumBones, OffsetMatrices, BoneIndices, iNumWeight, Bones);

	m_iNumIndices = iNumIndices;
	m_iIndexStride = 4;

	m_Buffer_Desc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = 0;

	m_pVtxIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pVtxIndices, sizeof(_uint) * m_iNumIndices);

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint iCountIndices = { 0 };

	m_iNumFaces = iNumFaces;
	for (size_t i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = m_pVtxIndices[i] = Indices[i];
	}

	m_ResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_NonAnim(vector<VTXMESH> Meshed, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	m_pVtxPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVtxPos, sizeof(_float3) * m_iNumVertices);

	VTXMESH* pVertexts = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertexts, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertexts[i].vPosition, &Meshed[i].vPosition, sizeof(_float3));
		XMStoreFloat3(&pVertexts[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertexts[i].vPosition), PreTransformMatrix));

		memcpy(&pVertexts[i].vNormal, &Meshed[i].vNormal, sizeof(_float3));
		XMStoreFloat3(&pVertexts[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertexts[i].vNormal), PreTransformMatrix));

		memcpy(&pVertexts[i].vTexcoord, &Meshed[i].vTexcoord, sizeof(_float2));
		memcpy(&pVertexts[i].vTangent, &Meshed[i].vTangent, sizeof(_float3));

		m_pVtxPos[i] = pVertexts[i].vPosition;
	}

	m_ResourceData.pSysMem = pVertexts;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertexts);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_Anim(vector<VTXANIMMESH> AnimMeshed, _int iNumBones, vector<_float4x4> OffsetMatrices, vector<_int> BoneIndices, _int iNumWeight, vector<class CBone*> Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i] = AnimMeshed[i];
	}

	/* ���鸦 ��ȸ�ϸ鼭 ������ ���� � �����鿡�� ������ �ִ°��� ĳġ�Ѵ�. */
	m_iNumBones = iNumBones;
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		// �޽ÿ� ���Ե� ��

		// ������ : ������� (���븦 �����ؼ� �ٸ� �𵨿��� ����Ҷ� ���� ũ�⸦ ����ؼ� �Ĺ��� ��İ� ���ϱ� ���� �뵵�� ����Ѵ�.)
		_float4x4 mOffsetMatrix;
		memcpy(&mOffsetMatrix, &OffsetMatrices[i], sizeof(_float4x4));
		//XMStoreFloat4x4(&mOffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&mOffsetMatrix)));
		m_OffsetMatrices.emplace_back(mOffsetMatrix);

		_uint		iBoneIndex = BoneIndices[i];
		m_BoneIndices.emplace_back(iBoneIndex);
	}

	// ���� ���� �޽ö�� ���� ������ 1�� �÷��ش�.
	// ��ü ���� �޽��� �̸��� ������ �̸��� ���� ���� �ε����� ��ȯ
	// ������ ������ ���� �ε����� ���� ������ �׵� ��ķ� ����� �����Ѵ�.
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint iBoneIndex = { 0 };

		auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone) ->_bool {

			if (pBone->Compare_NodeName(m_szName))
				return true;

			++iBoneIndex;

			return false;
			});

		_float4x4 OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.emplace_back(OffsetMatrix);

		m_BoneIndices.emplace_back(iBoneIndex);
	}

	m_ResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MESHTYPE eMeshType, _fmatrix PreTransformMatrix, const char* pName, _int iMaterialIndex, _int iNumVertices, _int iNumIndices, _int iNumFaces, unsigned int* Indices, vector<VTXANIMMESH> AnimMeshed, vector<VTXMESH> Meshed, _int iNumBones, vector<_float4x4> OffsetMatrices, vector<_int> BoneIndices, _int iNumWeight, vector<class CBone*> Bones)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eMeshType, 
		PreTransformMatrix, pName, iMaterialIndex, 
		iNumVertices, iNumIndices, iNumFaces, Indices,
		AnimMeshed, Meshed, iNumBones, OffsetMatrices, BoneIndices, iNumWeight, Bones)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
	__super::Free();
}
