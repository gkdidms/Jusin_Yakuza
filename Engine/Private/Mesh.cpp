#include "Mesh.h"
#include "Bone.h"

#include "GameInstance.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CVIBuffer{pDevice, pContext}
{
}

CMesh::CMesh(const CMesh& rhs)
    :CVIBuffer{ rhs }
	, m_iMaterialIndex{ rhs.m_iMaterialIndex }
	, m_OffsetMatrices{ rhs.m_OffsetMatrices }
	, m_isAlphaApply{ rhs.m_isAlphaApply }
	, m_iNumBones{ rhs.m_iNumBones }
	, m_iModelType {rhs.m_iModelType}
	, m_localMatrix{ rhs.m_localMatrix }
	, m_vMaxPosition {rhs.m_vMaxPosition}
	, m_vMinPosition {rhs.m_vMinPosition}
	, m_isTool {rhs.m_isTool}
	, m_BoneIndices {rhs.m_BoneIndices}
{
	memcpy(&m_szName, &rhs.m_szName, sizeof(char) * _MAX_PATH);

	if(CModel::TYPE_PARTICLE ==m_iModelType)
	{
		m_pIndices = new _uint[m_iNumIndices];
		ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

		memcpy(m_pIndices, rhs.m_pIndices, sizeof(_uint) * m_iNumIndices);

		m_pVertices = new VTXMESH[m_iNumVertices];
		ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

		memcpy(m_pVertices, rhs.m_pVertices, sizeof(VTXMESH) * m_iNumVertices);
	}
}

HRESULT CMesh::Initialize_Prototype(CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones, _bool isTool)
{
	strcpy_s(m_szName, pAIMesh->mName.data);

	m_isTool = isTool;

	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iModelType = eModelType;

	//�� Read�� ���� 3���� �ϳ��� �ﰢ������ ������ִ� �ɼ��̱� ������, ���ۿ��� ���������� Ʈ���̾ޱ۸���Ʈ �ɼ��� ����Ѵ�.
	
	m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_GIFormat = DXGI_FORMAT_R32_UINT;		//���� ������ 6������ �Ѿ ���� ���� ������ 32��Ʈ
	m_iNumVertexBuffers = 1;

	m_iNumVertices = pAIMesh->mNumVertices;			//������ ������ �Ž� ����ü�� �˰��ִ�!

	m_iIndexStride = 4;
	m_iNumIndices = pAIMesh->mNumFaces * 3;			// mNumFaces�� �������� �����̱� ������ *3 ���ָ� �ȴ�.


	/* ���� ���� ���� */
	HRESULT			hr = eModelType == CModel::TYPE_NONANIM || eModelType == CModel::TYPE_PARTICLE ? (eModelType == CModel::TYPE_PARTICLE ? Ready_Vertices_For_ParticleMesh(pAIMesh, PreTransformMatrix) : Ready_Vertices_For_NonAnimMesh(pAIMesh, PreTransformMatrix)) : Ready_Vertices_For_AnimMesh(pAIMesh, Bones);

	if (FAILED(hr))
		return E_FAIL;

	/* �ε��� ���� ����*/
	
	m_Buffer_Desc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
	
	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}


	if (CModel::TYPE_PARTICLE==eModelType)
	{
		m_pIndices = new _uint[m_iNumIndices];
		ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

		memcpy(m_pIndices, pIndices, sizeof(_uint) * m_iNumIndices);
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

    return S_OK;
}

HRESULT CMesh::Initialize_Prototype(CModel::MODELTYPE eModelType, const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones, _bool isTool)
{
	strcpy_s(m_szName, pAIMesh->mName);

	m_isTool = isTool;
	m_iModelType = eModelType;

	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	//�� Read�� ���� 3���� �ϳ��� �ﰢ������ ������ִ� �ɼ��̱� ������, ���ۿ��� ���������� Ʈ���̾ޱ۸���Ʈ �ɼ��� ����Ѵ�.
	m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_GIFormat = DXGI_FORMAT_R32_UINT;		//���� ������ 6������ �Ѿ ���� ���� ������ 32��Ʈ
	m_iNumVertexBuffers = 1;

	m_iNumVertices = pAIMesh->mNumVertices;			//������ ������ �Ž� ����ü�� �˰��ִ�!

	m_iIndexStride = 4;
	m_iNumIndices = pAIMesh->mNumFaces * 3;			// mNumFaces�� �������� �����̱� ������ *3 ���ָ� �ȴ�.


	/* ���� ���� ���� */
	HRESULT			hr = eModelType == CModel::TYPE_NONANIM || eModelType == CModel::TYPE_PARTICLE ? (eModelType == CModel::TYPE_PARTICLE ? Ready_Vertices_For_ParticleMesh(pAIMesh, PreTransformMatrix):Ready_Vertices_For_NonAnimMesh(pAIMesh, PreTransformMatrix)) : Ready_Vertices_For_AnimMesh(pAIMesh, Bones);

	if (FAILED(hr))
		return E_FAIL;

	/* �ε��� ���� ����*/
	m_Buffer_Desc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}

	if (eModelType == CModel::TYPE_PARTICLE)
	{
		m_pIndices = new _uint[m_iNumIndices];
		ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

		memcpy(m_pIndices, pIndices,  sizeof(_uint) * m_iNumIndices);
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CMesh::Initialize()
{
	if (m_iModelType == CModel::TYPE_ANIM && !m_isTool)
	{
		Ready_BoneBuffer();
		Ready_Buffer();
	}
		

	return S_OK;
}

HRESULT CMesh::Render(_bool isTool)
{
	if (m_iModelType == CModel::TYPE_ANIM && !isTool)
	{
		m_pContext->CopyResource(m_pProcessedVertexBuffer, m_pUAVOut);

		ID3D11Buffer* pVertices[] = {
			m_pProcessedVertexBuffer,
		};

		_uint pStrideVertices[] = {
			sizeof(VTXANIMBONE),
		};

		_uint pStartVertices[] = {
			0,
		};

		m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertices, pStrideVertices, pStartVertices);
		m_pContext->IASetIndexBuffer(m_pIB, m_GIFormat, 0);
		m_pContext->IASetPrimitiveTopology(m_Primitive_Topology);

		m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
	}
	else
	{
		if (FAILED(__super::Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_NonAnimMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	_float3		vSumPosition = _float3(0, 0, 0);
	_float3		vMinScale, vMaxScale;


	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix)));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));		//���°�� �ؽ�ó�� ��ġ������ �����Ұ�����!
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		vSumPosition.x += pVertices[i].vPosition.x;
		vSumPosition.y += pVertices[i].vPosition.y;
		vSumPosition.z += pVertices[i].vPosition.z;	


		if (0 == i)
		{
			vMinScale = pVertices[i].vPosition;
			vMaxScale = pVertices[i].vPosition;
		}
		else
		{
			if (vMinScale.x > pVertices[i].vPosition.x)
				vMinScale.x = pVertices[i].vPosition.x;
			if (vMinScale.y > pVertices[i].vPosition.y)
				vMinScale.y = pVertices[i].vPosition.y;
			if (vMinScale.z > pVertices[i].vPosition.z)
				vMinScale.z = pVertices[i].vPosition.z;


			if (vMaxScale.x < pVertices[i].vPosition.x)
				vMaxScale.x = pVertices[i].vPosition.x;
			if (vMaxScale.y < pVertices[i].vPosition.y)
				vMaxScale.y = pVertices[i].vPosition.y;
			if (vMaxScale.z < pVertices[i].vPosition.z)
				vMaxScale.z = pVertices[i].vPosition.z;
		}
	}

	_float3			vMeshScale = _float3(0,0,0);

	vMeshScale.x = abs(-vMinScale.x + vMaxScale.x);
	vMeshScale.y = abs(-vMinScale.y + vMaxScale.y);
	vMeshScale.z = abs(-vMinScale.z + vMaxScale.z);

	float		fMaxScale = 0;
	if (vMeshScale.x > vMeshScale.y)
		fMaxScale = vMeshScale.x;
	else
		fMaxScale = vMeshScale.y;

	if (vMeshScale.z > fMaxScale)
		fMaxScale = vMeshScale.z;

	m_fScale = fMaxScale;

	m_vMinPosition = vMinScale;
	m_vMaxPosition = vMaxScale;


	vSumPosition.x /= m_iNumVertices;
	vSumPosition.y /= m_iNumVertices;
	vSumPosition.z /= m_iNumVertices;

	XMStoreFloat4x4(&m_localMatrix, XMMatrixTranslation(vSumPosition.x, vSumPosition.y, vSumPosition.z));


	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_NonAnimMesh(const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	_float3		vSumPosition = _float3(0, 0, 0);
	_float3		vMinScale, vMaxScale;

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix)));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));		//���°�� �ؽ�ó�� ��ġ������ �����Ұ�����!
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		XMStoreFloat4x4(&m_localMatrix, XMMatrixTranslation(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z));

		vSumPosition.x += pVertices[i].vPosition.x;
		vSumPosition.y += pVertices[i].vPosition.y;
		vSumPosition.z += pVertices[i].vPosition.z;

		if (0 == i)
		{
			vMinScale = pVertices[i].vPosition;
			vMaxScale = pVertices[i].vPosition;
		}
		else
		{
			if (vMinScale.x > pVertices[i].vPosition.x)
				vMinScale.x = pVertices[i].vPosition.x;
			if (vMinScale.y > pVertices[i].vPosition.y)
				vMinScale.y = pVertices[i].vPosition.y;
			if (vMinScale.z > pVertices[i].vPosition.z)
				vMinScale.z = pVertices[i].vPosition.z;


			if (vMaxScale.x < pVertices[i].vPosition.x)
				vMaxScale.x = pVertices[i].vPosition.x;
			if (vMaxScale.y < pVertices[i].vPosition.y)
				vMaxScale.y = pVertices[i].vPosition.y;
			if (vMaxScale.z < pVertices[i].vPosition.z)
				vMaxScale.z = pVertices[i].vPosition.z;
		}
	}

	_float3			vMeshScale = _float3(0, 0, 0);

	vMeshScale.x = abs(-vMinScale.x + vMaxScale.x);
	vMeshScale.y = abs(-vMinScale.y + vMaxScale.y);
	vMeshScale.z = abs(-vMinScale.z + vMaxScale.z);

	float		fMaxScale = 0;
	if (vMeshScale.x > vMeshScale.y)
		fMaxScale = vMeshScale.x;
	else
		fMaxScale = vMeshScale.y;

	if (vMeshScale.z > fMaxScale)
		fMaxScale = vMeshScale.z;

	m_fScale = fMaxScale;
	m_vMinPosition = vMinScale;
	m_vMaxPosition = vMaxScale;

	vSumPosition.x /= m_iNumVertices;
	vSumPosition.y /= m_iNumVertices;
	vSumPosition.z /= m_iNumVertices;

	XMStoreFloat4x4(&m_localMatrix, XMMatrixTranslation(vSumPosition.x, vSumPosition.y, vSumPosition.z));


	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_ParticleMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	_float3		vSumPosition = _float3(0, 0, 0);
	_float3		vMinScale, vMaxScale;


	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix)));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));		//���°�� �ؽ�ó�� ��ġ������ �����Ұ�����!
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		vSumPosition.x += pVertices[i].vPosition.x;
		vSumPosition.y += pVertices[i].vPosition.y;
		vSumPosition.z += pVertices[i].vPosition.z;


		if (0 == i)
		{
			vMinScale = pVertices[i].vPosition;
			vMaxScale = pVertices[i].vPosition;
		}
		else
		{
			if (vMinScale.x > pVertices[i].vPosition.x)
				vMinScale.x = pVertices[i].vPosition.x;
			if (vMinScale.y > pVertices[i].vPosition.y)
				vMinScale.y = pVertices[i].vPosition.y;
			if (vMinScale.z > pVertices[i].vPosition.z)
				vMinScale.z = pVertices[i].vPosition.z;


			if (vMaxScale.x < pVertices[i].vPosition.x)
				vMaxScale.x = pVertices[i].vPosition.x;
			if (vMaxScale.y < pVertices[i].vPosition.y)
				vMaxScale.y = pVertices[i].vPosition.y;
			if (vMaxScale.z < pVertices[i].vPosition.z)
				vMaxScale.z = pVertices[i].vPosition.z;
		}
	}

	_float3			vMeshScale = _float3(0, 0, 0);

	vMeshScale.x = abs(-vMinScale.x + vMaxScale.x);
	vMeshScale.y = abs(-vMinScale.y + vMaxScale.y);
	vMeshScale.z = abs(-vMinScale.z + vMaxScale.z);

	float		fMaxScale = 0;
	if (vMeshScale.x > vMeshScale.y)
		fMaxScale = vMeshScale.x;
	else
		fMaxScale = vMeshScale.y;

	if (vMeshScale.z > fMaxScale)
		fMaxScale = vMeshScale.z;

	m_fScale = fMaxScale;


	vSumPosition.x /= m_iNumVertices;
	vSumPosition.y /= m_iNumVertices;
	vSumPosition.z /= m_iNumVertices;

	XMStoreFloat4x4(&m_localMatrix, XMMatrixTranslation(vSumPosition.x, vSumPosition.y, vSumPosition.z));


	m_InitialData.pSysMem = pVertices;

	m_pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	memcpy(m_pVertices, pVertices, sizeof(VTXMESH) * m_iNumVertices);

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_ParticleMesh(const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	_float3		vSumPosition = _float3(0, 0, 0);
	_float3		vMinScale, vMaxScale;

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix)));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));		//���°�� �ؽ�ó�� ��ġ������ �����Ұ�����!
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		XMStoreFloat4x4(&m_localMatrix, XMMatrixTranslation(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z));

		vSumPosition.x += pVertices[i].vPosition.x;
		vSumPosition.y += pVertices[i].vPosition.y;
		vSumPosition.z += pVertices[i].vPosition.z;

		if (0 == i)
		{
			vMinScale = pVertices[i].vPosition;
			vMaxScale = pVertices[i].vPosition;
		}
		else
		{
			if (vMinScale.x > pVertices[i].vPosition.x)
				vMinScale.x = pVertices[i].vPosition.x;
			if (vMinScale.y > pVertices[i].vPosition.y)
				vMinScale.y = pVertices[i].vPosition.y;
			if (vMinScale.z > pVertices[i].vPosition.z)
				vMinScale.z = pVertices[i].vPosition.z;


			if (vMaxScale.x < pVertices[i].vPosition.x)
				vMaxScale.x = pVertices[i].vPosition.x;
			if (vMaxScale.y < pVertices[i].vPosition.y)
				vMaxScale.y = pVertices[i].vPosition.y;
			if (vMaxScale.z < pVertices[i].vPosition.z)
				vMaxScale.z = pVertices[i].vPosition.z;
		}
	}

	_float3			vMeshScale = _float3(0, 0, 0);

	vMeshScale.x = abs(-vMinScale.x + vMaxScale.x);
	vMeshScale.y = abs(-vMinScale.y + vMaxScale.y);
	vMeshScale.z = abs(-vMinScale.z + vMaxScale.z);

	float		fMaxScale = 0;
	if (vMeshScale.x > vMeshScale.y)
		fMaxScale = vMeshScale.x;
	else
		fMaxScale = vMeshScale.y;

	if (vMeshScale.z > fMaxScale)
		fMaxScale = vMeshScale.z;

	m_fScale = fMaxScale;

	vSumPosition.x /= m_iNumVertices;
	vSumPosition.y /= m_iNumVertices;
	vSumPosition.z /= m_iNumVertices;

	XMStoreFloat4x4(&m_localMatrix, XMMatrixTranslation(vSumPosition.x, vSumPosition.y, vSumPosition.z));


	m_InitialData.pSysMem = pVertices;

	m_pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	memcpy(m_pVertices, pVertices, sizeof(VTXMESH) * m_iNumVertices);

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_AnimMesh(const aiMesh* pAIMesh, const vector<class CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	if (m_isTool)
	{
		m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
		m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_Buffer_Desc.CPUAccessFlags = 0;
		m_Buffer_Desc.MiscFlags = 0;
		m_Buffer_Desc.StructureByteStride = m_iVertexStride;
	}
	else
	{
		m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
		m_Buffer_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		m_Buffer_Desc.CPUAccessFlags = 0;
		m_Buffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		m_Buffer_Desc.StructureByteStride = m_iVertexStride;
	}

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));		//���°�� �ؽ�ó�� ��ġ������ �����Ұ�����!
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	//2. ������ �������� Position�� Normal�� �θ� ��� ��ȯ�� ���� + OffsetMatrix ��ȯ���� �������ش�.
	//3. ������ �������� vBlendIndices�� vBlendWeights ���� ä���ش�.
	//->�� �������� aiScene->aiMesh[i]->aiBone[j]��, ->aiVertexWeight���� �о�� �����ϰ� �ִ�. (mesh, bone, aiVertexWeight�� �迭�̴�)
	//->Ǯ� ����ϸ� �о�� �𵨿� i��° �޽ÿ� ������ �ִ� j��° ���� �������� �����ͼ� ������ �־�����ϴ� ���̴�.

	m_iNumBones = pAIMesh->mNumBones;

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4		OffsetMatrix;		// ���� ���� ����ϴ� ���� ũ���� ���� ������ �� �����Ƿ�, ���� �޽��� ������ �°� �������ִ� ������ ���ִ� ��Ʈ�����̴�.
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.emplace_back(OffsetMatrix);

		_uint iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(pAIBone->mName.data))
					return true;

				++iBoneIndex;

				return false;
			});

		m_BoneIndices.emplace_back(iBoneIndex);

		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight		VertexWeight = pAIBone->mWeights[j];

			/* VertexWeight.mVertexId : �� ���� ������ �ִ� ������ ��, j��° ������ �ε��� */
			/* pVertices[VertexWeight.mVertexId].vBlendIndices: �� �������� �������� �ִ� ���� �ε����� �ִ� �װ� �����Ѵ�. */
			if (0.0f == pVertices[VertexWeight.mVertexId].vBlendWeights.x)
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.x = i;			//mesh ������ �� �迭�� �ε���
				pVertices[VertexWeight.mVertexId].vBlendWeights.x = VertexWeight.mWeight;
			}

			else if (0.0f == pVertices[VertexWeight.mVertexId].vBlendWeights.y)
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.y = i;
				pVertices[VertexWeight.mVertexId].vBlendWeights.y = VertexWeight.mWeight;
			}

			else if (0.0f == pVertices[VertexWeight.mVertexId].vBlendWeights.z)
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.z = i;
				pVertices[VertexWeight.mVertexId].vBlendWeights.z = VertexWeight.mWeight;
			}

			else
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.w = i;
				pVertices[VertexWeight.mVertexId].vBlendWeights.w = VertexWeight.mWeight;
			}
		}

	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(m_szName))
					return true;

				++iBoneIndex;

				return false;
			});

		m_BoneIndices.emplace_back(iBoneIndex);
	
		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_AnimMesh(const BAiMesh* pAIMesh, const vector<class CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	if (m_isTool)
	{
		m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
		m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_Buffer_Desc.CPUAccessFlags = 0;
		m_Buffer_Desc.MiscFlags = 0;
		m_Buffer_Desc.StructureByteStride = m_iVertexStride;
	}
	else
	{
		m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
		m_Buffer_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		m_Buffer_Desc.CPUAccessFlags = 0;
		m_Buffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		m_Buffer_Desc.StructureByteStride = m_iVertexStride;
	}

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));		//���°�� �ؽ�ó�� ��ġ������ �����Ұ�����!
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	//2. ������ �������� Position�� Normal�� �θ� ��� ��ȯ�� ���� + OffsetMatrix ��ȯ���� �������ش�.
	//3. ������ �������� vBlendIndices�� vBlendWeights ���� ä���ش�.
	//->�� �������� aiScene->aiMesh[i]->aiBone[j]��, ->aiVertexWeight���� �о�� �����ϰ� �ִ�. (mesh, bone, aiVertexWeight�� �迭�̴�)
	//->Ǯ� ����ϸ� �о�� �𵨿� i��° �޽ÿ� ������ �ִ� j��° ���� �������� �����ͼ� ������ �־�����ϴ� ���̴�.

	m_iNumBones = pAIMesh->mNumBones;

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		BAiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4		OffsetMatrix;		// ���� ���� ����ϴ� ���� ũ���� ���� ������ �� �����Ƿ�, ���� �޽��� ������ �°� �������ִ� ������ ���ִ� ��Ʈ�����̴�.
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.emplace_back(OffsetMatrix);

		_uint iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(pAIBone->mName))
					return true;

				++iBoneIndex;

				return false;
			});


		m_BoneIndices.emplace_back(iBoneIndex);

		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			BAiVertexWeight		VertexWeight = pAIBone->mWeights[j];

			/* VertexWeight.mVertexId : �� ���� ������ �ִ� ������ ��, j��° ������ �ε��� */
			/* pVertices[VertexWeight.mVertexId].vBlendIndices: �� �������� �������� �ִ� ���� �ε����� �ִ� �װ� �����Ѵ�. */
			if (0.0f == pVertices[VertexWeight.mVertexId].vBlendWeights.x)
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.x = i;			//mesh ������ �� �迭�� �ε���
				pVertices[VertexWeight.mVertexId].vBlendWeights.x = VertexWeight.mWeight;
			}

			else if (0.0f == pVertices[VertexWeight.mVertexId].vBlendWeights.y)
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.y = i;
				pVertices[VertexWeight.mVertexId].vBlendWeights.y = VertexWeight.mWeight;
			}

			else if (0.0f == pVertices[VertexWeight.mVertexId].vBlendWeights.z)
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.z = i;
				pVertices[VertexWeight.mVertexId].vBlendWeights.z = VertexWeight.mWeight;
			}

			else
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.w = i;
				pVertices[VertexWeight.mVertexId].vBlendWeights.w = VertexWeight.mWeight;
			}
		}

	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(m_szName))
					return true;

				++iBoneIndex;

				return false;
			});


		m_BoneIndices.emplace_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;

}

void CMesh::Fill_Matrices(vector<class CBone*>& Bones, _float4x4* pMeshBoneMatrices)
{
	for (size_t i = 0; i < m_iNumBones; i++)
		XMStoreFloat4x4(&pMeshBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix()));
}

void CMesh::Bind_Matrices(vector<class CBone*>& Bones, _float4x4* pMeshBoneMatrices)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&pMeshBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix()));
		XMStoreFloat4x4(&pMeshBoneMatrices[i], XMMatrixTranspose(XMLoadFloat4x4(&pMeshBoneMatrices[i])));
	}

	m_pContext->UpdateSubresource(m_pBoneMatrixBuffer, 0, nullptr, pMeshBoneMatrices, 0, 0);

	m_pContext->CSSetConstantBuffers(0, 1, &m_pBoneMatrixBuffer);
}

HRESULT CMesh::Ready_Buffer()
{
	D3D11_BUFFER_DESC Desc{};
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.ByteWidth = sizeof(_matrix) * 512;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.StructureByteStride = sizeof(_matrix);

	if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pBoneMatrixBuffer)))
		return E_FAIL;

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones, _bool isTool)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, PreTransformMatrix, Bones, isTool)))
	{
		MSG_BOX("Failed To Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODELTYPE eModelType, const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones, _bool isTool)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, PreTransformMatrix, Bones, isTool)))
	{
		MSG_BOX("Failed To Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Clone()
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CMesh");
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

	if(CModel::TYPE_PARTICLE==m_iModelType)
	{
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pIndices);
	}

	Safe_Release(m_pBoneMatrixBuffer);
}
