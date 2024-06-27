#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CRenderer :
    public CBase
{
public:
    enum RENDERER_STATE { RENDER_PRIORITY, RENDER_SHADOWOBJ, RENDER_NONBLENDER, RENDER_NONLIGHT, RENDER_BLENDER, RENDER_EFFECT, RENDER_UI, RENDER_END };

private:
    CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CRenderer() = default;

public:
    void Set_HDR(_bool isHDR) { m_isHDR = isHDR; }
    void Set_HDRLight(_float fLight) { m_fHDRLight = fLight; } // HDR 밝기 조절(빛 세기)
    void Set_SSAO(_bool isSSAO) { m_isSSAO = isSSAO; }
    void Set_SSAORadiuse(_float fRadiuse) { m_fSSAORadiuse = fRadiuse; }
    void Set_SSAOBlur(_float fBlur) { m_fSSAOBlur = fBlur; }
    void Set_SSAOBias(_float fBias) { m_fSSAOBiae = fBias; }

public:
    _bool isHDR() { return m_isHDR; }
    _bool isSSAO() { return m_isSSAO; }
    _float Get_HDRLight() { return m_fHDRLight; }
    _float Get_SSAORadiuse() { return m_fSSAORadiuse; }
    _float Get_SSAOBlur() { return m_fSSAOBlur; }
    _float Get_SSAOBias() { return m_fSSAOBiae; }

public:
    HRESULT Initialize();
    void Add_Renderer(RENDERER_STATE eRenderState, class CGameObject* pGameObject);
    void Draw();
    void Clear();

#ifdef _DEBUG
public:
    _bool isDebugView() { return m_isDebugView; }
    void Set_DebugView(_bool isDebugView) { m_isDebugView = isDebugView; }

public:
    HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif

private:
    void Render_Priority();
    void Render_ShadowObjects();
    void Render_NonBlender();

    /* SSAO */
    void Render_SSAO();
    void Render_SSAOBlur();

    void Render_LightAcc(); // Light 연산 + SSAO 합
    void Render_CopyBackBuffer();
    void Render_DeferredResult();

    /* HDR*/
    void Render_Luminance();
    void Render_HDR();
    void Render_CopyLuminance(); 
    void Render_AvgLuminance();
    void Redner_LuminanceResult();

    void Render_NonLight();//이펙트 시작
    void Render_Bloom();//블러
    void Render_FinalEffectBlend();//블러 합치기
    void Render_Blender();//기존 논블렌드 렌더
    void Render_Effect();// 파티클렌더 
    void Render_FinlaOIT();// 파티클 최종병합
    void Render_UI();

private:
    HRESULT Ready_SSAONoiseTexture();

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
    ID3D11ShaderResourceView* m_pSSAONoiseView = { nullptr };
    //ID3D11DepthStencilView* m_pLuminanceStencilView = { nullptr };

private:
    _bool m_isHDR = { false };
    _bool m_isSSAO = { false };
    _float m_fHDRLight = { 1.f };
    _float m_fSSAORadiuse = { 0.003f };
    _float m_fSSAOBlur = { 2.f };
    _float m_fSSAOBiae = { 0.025f };
    vector<_float3> m_vSSAOKernal;

#ifdef _DEBUG
    _bool m_isDebugView = { true };
#endif // _DEBUG


public:
    static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
END

