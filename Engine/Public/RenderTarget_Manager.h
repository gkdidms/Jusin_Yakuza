#pragma once
#include "Base.h"

BEGIN(Engine)
class CRenderTarget_Manager :
    public CBase
{
private:
    CRenderTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CRenderTarget_Manager() = default;

public:
    ID3D11Texture2D* Get_TextureBuffer(const wstring& strTargetTag);

public:
    HRESULT Initialize();
    HRESULT Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isCompute, _uint iArrayCount);
    HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag);
    HRESULT Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSView, _bool isClear);
    HRESULT End_MRT();
    HRESULT Bind_RenderTargetSRV(const wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
    void Bind_ComputeRenderTargetSRV(const wstring& strTargetTag, _uint iSlot);
    void Bind_ComputeRenderTargetUAV(const wstring& strTargetTag, _uint iSlot);

    HRESULT Copy_Resource(const wstring& strTargetTag, ID3D11Texture2D* pDesc);
    HRESULT Create_Texture(const wstring& strTargetTag, const wstring& strSaveFilePath);
    HRESULT Clear_RenderTarget(const wstring& strTargetTag);

#ifdef _DEBUG
    HRESULT Ready_Debug(const wstring strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
    HRESULT Render_Debug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, _bool isArray);
#endif // _DEBUG

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    ID3D11RenderTargetView* m_pBackBufferView = { nullptr };
    ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };

    map<const wstring, class CRenderTarget*> m_RenderTargets;
    map<const wstring, list<class CRenderTarget*>>		m_MRTs;

private:
    class CRenderTarget* Find_RenderTarget(const wstring& strRenderTarget);
    list<class CRenderTarget*>* Find_MRT(const wstring& strMRTTag);

public:
    static CRenderTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};
END