#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CRenderTarget :
    public CBase
{
private:
    CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CRenderTarget() = default;

public:
    ID3D11RenderTargetView* Get_RTV() { return m_pRTV; }
    ID3D11UnorderedAccessView* Get_UAV() { return m_pUAV; }

public:
    HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArrayCount);
    HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat);
    HRESULT Clear();
    HRESULT Bind_SVR(class CShader* pShader, const _char* pConstantName);
    HRESULT Copy_Resource(ID3D11Texture2D* pDesc);
    HRESULT Create_Texture(const wstring& strSaveFilePath);

#ifdef _DEBUG
public:
    HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
    HRESULT Render_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, _bool isArray);
#endif // _DEBUG

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    ID3D11RenderTargetView* m_pRTV = { nullptr };
    ID3D11ShaderResourceView* m_pSRV = { nullptr };
    ID3D11UnorderedAccessView* m_pUAV = { nullptr };
    ID3D11Texture2D* m_pTexture2D = { nullptr };

#ifdef _DEBUG
private:
    _float4x4 m_WorldMatrix;
#endif // _DEBUG


private:
    _float4 m_vClearColor;

public:
    static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArrayCount=1, _bool isComputeShader = false);
    virtual void Free() override;
};
END