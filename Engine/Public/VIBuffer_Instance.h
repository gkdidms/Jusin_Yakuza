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
        _float3 vRange;//������ü ����
        _float fRadius;//�� ����
        _float2 vSize;//���� ������(����*����)(������Ʈ��)
        _float2 vRectSize;//�� ��ƼŬ ������
        _float2 vSpeed;
        _float2 vLifeTime;
        _bool isLoop;
        const _float4x4* WorldMatrix;
        _bool isAura;
    }INSTANCE_DESC;

    //typedef struct tBlendSort
    //{
    //    VTXMATRIX vMatrix;
    //    _float ViewZ;
    //}BlendSort;
protected:
    CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
    virtual ~CVIBuffer_Instance() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Render() override;
    virtual HRESULT Compute_Render() ;

protected:
    //�⺻ ���� ������ �ְ� �ؾߵ�.
    ID3D11Buffer* m_pVBInstance = { nullptr };
    D3D11_BUFFER_DESC	m_InstanceBufferDesc = {};
    INSTANCE_DESC* m_InstanceDesc = {};

    _uint m_iNumInstance = { 0 };
    _uint m_iIndexCountPerInstance = { 0 };
    _uint m_iInstanceStride = { 0 };

    _float* m_pSpeeds = { nullptr };
    _float3* m_pOriginalPositions = { nullptr };
    _float* m_pOriginalSize = { nullptr };
    _float3* m_pOriginalOffsets = { nullptr };


    const _float4x4* m_pCurrentWorldMatrix = { nullptr };



    //���� cpu ���� ��Ʈ��
    VTXMATRIX* m_pTempVertices;



    //��ǻƮ ���̴�(�ν��Ͻ��� �����ϸ� ��� ���� �ʿ�.)
    class CComputeShader* m_pComputeShader = { nullptr };


    //����� �޾ƿ� ����(�б�/���� ����)
    ID3D11Buffer* m_pOutBuffer = { nullptr };
    D3D11_BUFFER_DESC m_OutBufferDesc = {};


    //�б�/���� ���� ���ε������� ��ȯ��
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;

    ID3D11UnorderedAccessView* mOutputUAV = {};

    //����� ������ ���۸� ������ �ʿ��� �κи� ����ֱ�.
    ID3D11Buffer* m_pRenderBuffer = { nullptr };
    D3D11_BUFFER_DESC m_RenderBufferDesc = {};

    _bool m_isReset = { false };
public:
    void Spread(_float fTimeDelta);
    void Aura(_float fTimeDelta);
    void Reset();

    void Drop(_float fTimeDelta);
    _bool LifeTime_Check();
    void SizeUp_Time(_float fTimeDelta);
    void SizeDown_Time(_float fTimeDelta);

    void Compute_Sort();

    
public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;
};
END
