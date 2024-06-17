#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class CMesh final :
    public CVIBuffer
{
public:
    enum MESHTYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
    
    typedef struct {
        MESHTYPE eMeshType;
        const char* pName;
        _int iMaterialIndex;
        _int iNumVertices;
        _int iNumIndices;
        _int iNumFaces;
        unsigned int* Indices;
        vector<VTXANIMMESH> AnimMeshed;
        vector<VTXMESH> Meshed;
        _int iNumBones;
        vector<_float4x4> OffsetMatrices;
        vector<_int> BoneIndices;
        _int iNumWeight;
    } MESH_DESC ;

private:
    CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMesh(const CMesh& rhs);
    virtual ~CMesh() = default;

public:
    _bool Compare_MeshName(const char* pName) { return !strcmp(m_szName, pName); }

public:
    _float3* Get_VtxPos() { return m_pVtxPos; }
    _uint* Get_VtxIndices() { return m_pVtxIndices; }
    _uint Get_NumFaces() { return m_iNumFaces; }

public:
    _uint Get_MaterialIndex() { return m_iMaterialIndex; }
    void Fill_Matrices(const vector<class CBone*>& Bones, _float4x4* BoneTransformMatrix);

public:
    HRESULT Initialize_Prototype(_fmatrix PreTransformMatrix, vector<class CBone*> Bones, void* pArg);

private:
    _uint m_iMaterialIndex = { 0 };

    _uint m_iNumBones = { 0 };
    _uint m_iNumFaces = { 0 };

    vector<_uint> m_BoneIndices;
    vector<_float4x4> m_OffsetMatrices;

    char m_szName[MAX_PATH] = "";

    _float3* m_pVtxPos;
    _uint* m_pVtxIndices;

private:
    HRESULT Ready_Vertices_For_NonAnim(vector<VTXMESH> Meshed, _fmatrix PreTransformMatrix);
    HRESULT Ready_Vertices_For_Anim(vector<VTXANIMMESH> AnimMeshed, _int iNumBones, vector<_float4x4> OffsetMatrices, vector<_int> BoneIndices, _int iNumWeight, vector<class CBone*> Bones);

public:
    static CMesh* Create(
        ID3D11Device* pDevice, 
        ID3D11DeviceContext* pContext, 
        _fmatrix PreTransformMatrix, 
        vector<class CBone*> Bones,
        void* pArg);

    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;
};

END