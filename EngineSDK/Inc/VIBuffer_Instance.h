#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Instance abstract :
    public CVIBuffer
{
public:
    typedef struct tVIBufferInstanceDesc
    {
        _uint iNumInstance;
        _float3 vOffsetPos;
        _float3 vPivotPos;
        _bool bRadius;
        _float3 vRange;//정육면체 생성
        _float fRadius;//구 생성
        _float2 vSize;//최초 사이즈(가로*세로)(지오메트리)
        _float2 vRectSize;//각 파티클 사이즈
        _float2 vSpeed;
        _float2 vLifeTime;
        _bool isLoop;
    }INSTANCE_DESC;

    typedef struct tBlendSort
    {
        VTXMATRIX vMatrix;
        _float ViewZ;
    }BlendSort;
protected:
    CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
    virtual ~CVIBuffer_Instance() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Render() override;

protected:
    ID3D11Buffer* m_pVBInstance = { nullptr };
    D3D11_BUFFER_DESC	m_InstanceBufferDesc = {};
    INSTANCE_DESC* m_InstanceDesc = {};

    _uint m_iNumInstance = { 0 };
    _uint m_iIndexCountPerInstance = { 0 };
    _uint m_iInstanceStride = { 0 };

    _float* m_pSpeeds = { nullptr };
    _float3* m_pOriginalPositions = { nullptr };

    //인스턴스 임시저장소
    VTXMATRIX* m_pTempVertices = { nullptr };

public:
    void Spread(_float fTimeDelta);
    void Drop(_float fTimeDelta);
    void LifeTime_Check();
    void Blend_Sort();

    
public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;
};
END
