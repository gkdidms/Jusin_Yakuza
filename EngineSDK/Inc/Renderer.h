#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CRenderer :
    public CBase
{

public:
    enum RENDERER_STATE { RENDER_PRIORITY, RENDER_SHADOWOBJ, RENDER_NONBLENDER, RENDER_NONLIGHT, RENDER_BLENDER, RENDER_UI, RENDER_END };

private:
    CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CRenderer() = default;

public:
    void Set_HDR(_bool isHDR) { m_isHDR = isHDR; }

public:
    _bool Get_HDR() { return m_isHDR; }

public:
    HRESULT Initialize();
    void Add_Renderer(RENDERER_STATE eRenderState, class CGameObject* pGameObject);
    void Draw();
    void Clear();

#ifdef _DEBUG
public:
    HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif

private:
    void Render_Priority();
    void Render_ShadowObjects();
    void Render_NonBlender();
    void Render_LightAcc();
    void Render_CopyBackBuffer();
    void Render_DeferredResult();
    void Render_Luminance();
    void Render_HDR();
    void Render_CopyLuminance(); 
    void Render_AvgLuminance();
    void Redner_LuminanceResult();

    void Render_NonLight();
    void Render_Blender();
    void Render_UI();

#ifdef _DEBUG
private:
    list<class CComponent*>	m_DebugComponents;

private:
    void Render_Debug();
#endif // DEBUG


private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

    list<class CGameObject*> m_RenderObject[RENDER_END];

private:
    class CShader* m_pShader = { nullptr };
    class CVIBuffer_Rect* m_pVIBuffer = { nullptr };

    _float4x4 m_WorldMatrix;
    _float4x4 m_ViewMatrix;
    _float4x4 m_ProjMatrix;

    ID3D11DepthStencilView* m_pLightDepthStencilView = { nullptr };
    //ID3D11DepthStencilView* m_pLuminanceStencilView = { nullptr };

private:
    _bool m_isHDR = { true };

public:
    static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
END

