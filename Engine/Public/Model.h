#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Animation.h"
#include "Bone.h"

BEGIN(Engine)
class ENGINE_DLL CModel final:
    public CComponent
{
public:
    typedef struct tAnimDesc{
        tAnimDesc(_uint iAnimIndex, _bool isLoop)
            : iAnimIndex{ iAnimIndex }, isLoop{ isLoop } {}

        _uint iAnimIndex = { 0 };
        _bool isLoop = { false };
    }ANIM_DESC;

private:
    CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CModel(const CModel& rhs);
    virtual ~CModel() = default;

public:
    _uint Get_NumMeshes() { return m_iNumMeshes; }

    _bool Get_Animation_Finished() const { return m_Animations[m_AnimDesc.iAnimIndex]->IsFinished(); } //현재 재생되고 있는 애니메이션 모델이 종료 되었는지 확인하는 함수
    const _float4x4* Get_BoneCombinedTransformationMatrix(const _char* szBoneName) const;

public:
    HRESULT Initialize_Prototype(const _char* szModelFilePath, _fmatrix PreTransformMatrix, const _char* szBinaryFilePath);
    HRESULT Initialize(void* pArv) override;
    void Render(_uint iMeshIndex);

    HRESULT Bind_Material(class CShader* pShader, const char* strConstansName, _uint iMeshIndex, aiTextureType iTextureType);
    HRESULT Bind_BoneMatrices(class CShader* pShader, const char* strConstansName, _uint iMeshIndex);

public:
    void Play_Animation(const _float& fTimeDelta, _float4x4* vMovePos);
    void Set_Animation(ANIM_DESC AnimDesc) {
        if (m_AnimDesc.iAnimIndex == AnimDesc.iAnimIndex)
            return;
        m_AnimDesc = AnimDesc;
        m_Animations[m_AnimDesc.iAnimIndex]->Reset();
    }

private:
    _float4x4	m_PreTransformMatrix;
    CMesh::MESHTYPE m_eMeshType = { CMesh::TYPE_END };

private:
    _uint m_iNumMeshes = { 0 };
    vector<class CMesh*> m_Meshes;

private:
    _uint m_iNumMaterials = { 0 };
    vector<MESH_MATERIAL> m_Materials;

private:
    _uint m_iNumBones = { 0 };
    vector<class CBone*> m_Bones;

    _float4x4 m_MeshBoneMatrices[512];

private:
    _uint m_iNumAnimations = { 0 };
    vector<class CAnimation*> m_Animations;
    ANIM_DESC m_AnimDesc{ 0, false};

private:
    CTexture* Ready_Materials(const char* pModelFilePath, const char* pFilePath);
    HRESULT Ready_Model(const _char* szModelFilePath, const _char* szBinaryFilePath);

public:
    //static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CMesh::MESHTYPE eMeshType, const _char* szModelFilePath, _fmatrix PreTransformMatrix, const _char* szBinaryFilePath = nullptr,CREATETYPE eCreateType = CREATE_AI);
    static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szModelFilePath, _fmatrix PreTransformMatrix, const _char* szBinaryFilePath = nullptr);
    virtual CComponent* Clone(void* vArg) override;
    virtual void Free() override;
};
END
