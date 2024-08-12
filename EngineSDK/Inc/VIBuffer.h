#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer :
    public CComponent
{
protected:
    CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer(const CVIBuffer& rhs);
    virtual ~CVIBuffer() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Render() override;
    virtual HRESULT Bind_Compute(class CComputeShader* pShader) override;

public:
    HRESULT Create_Buffer(ID3D11Buffer** pOut);
    HRESULT Ready_ComputeBuffer();
    _uint Get_NumVertex() { return m_iNumVertices; }
    _uint Get_NumIndices() { return m_iNumIndices; }

protected:
    ID3D11ShaderResourceView* m_pVertexBufferSRV = { nullptr };
    ID3D11UnorderedAccessView* m_pResultBufferUAV = { nullptr };
    ID3D11Buffer* m_pProcessedVertexBuffer = { nullptr };

    ID3D11Buffer* m_pVB;
    ID3D11Buffer* m_pUAVOut;
    ID3D11Buffer* m_pIB;

protected:
    D3D11_BUFFER_DESC m_Buffer_Desc = {};
    D3D11_SUBRESOURCE_DATA m_InitialData = {};

    _uint m_iNumVertexBuffers = { 0 };
    _uint m_iVertexStride = { 0 };
    _uint m_iNumVertices = { 0 };

    _uint m_iIndexStride = { 0 };
    _uint m_iNumIndices = { 0 };

    DXGI_FORMAT m_GIFormat = {};
    D3D11_PRIMITIVE_TOPOLOGY m_Primitive_Topology = {};
    
    _float4 m_vClearColor = {0.f, 0.f, 0.f, 0.f};

public:
    virtual void Free() override;
};
END
