#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CRenderer :
    public CBase
{
public:
    enum RENDERER_STATE {
        RENDER_PRIORITY,
        RENDER_SHADOWOBJ,
        RENDER_NONBLENDER,
        RENDER_DECAL,
        RENDER_GLASS,
        RENDER_PUDDLE,
        RENDER_NONLIGHT_NONBLUR,
        RENDER_NONLIGHT,
        RENDER_BLENDER,
        RENDER_EFFECT,
        RENDER_UI,
        RENDER_END
    };

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
    void Set_PBR(_bool isPBR) { m_isPBR = isPBR; }
    void Set_BOF(_bool isBOF) { m_isBOF = isBOF; }
    void Set_Shadow(_bool isShadow) { m_isShadow = isShadow; }
    void Set_RimLight(_bool isRimLight) { m_isRimLight = isRimLight; }
    void Set_RadialBlur(_bool isRadialBlur) { m_isRadialBlur = isRadialBlur; }
    void Set_MotionBlur(_bool isMotionBlur) { m_isMotionBlur = isMotionBlur; }

public:
    _bool isHDR() { return m_isHDR; }
    _bool isSSAO() { return m_isSSAO; }
    _bool isPBR() { return m_isPBR; }
    _bool isBOF() { return m_isBOF; }
    _bool isShadow() { return m_isShadow; }
    _bool isRimLight() { return m_isRimLight; }
    _bool isRadialBlur() { return m_isRadialBlur; }
    _bool isMotionBlur() { return m_isMotionBlur; }
    _float Get_HDRLight() { return m_fHDRLight; }
    _float Get_SSAORadiuse() { return m_fSSAORadiuse; }
    _float Get_SSAOBlur() { return m_fSSAOBlur; }
    _float Get_SSAOBias() { return m_fSSAOBiae; }
    int     Get_RenderState() { return m_iRenderState; }

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

    /* Map */
    void Render_Decal();
    void Render_Glass();
    void Render_Puddle();

    /* SSAO */
    void Render_SSAO();
    void Render_SSAOBlur();

    void Render_LightAcc(); // Light 연산 + SSAO 합 + PBR
    void Render_CopyBackBuffer();
    void Render_DeferredResult();

    /* BOF */
    void Render_DeferredBlur();
    void Render_BOF();

    /* HDR*/
    void Render_Luminance();
    void Render_HDR();
    void Render_CopyLuminance();
    void Render_AvgLuminance();
    void Render_LuminanceResult();

    /* NonLight인데 blur 안 들어감 */
    void Render_NonLight_NonBlur();

    /* Effect */
    void Render_RimLight();
    void Render_NonLight();//이펙트 시작
    void Render_Bloom();//블러
    void Render_FinalEffectBlend();//블러 합치기
    void Render_Blender();//기존 논블렌드 렌더
    void Render_Effect();// 파티클렌더 
    void Render_FinlaOIT();// 파티클 최종병합
    void Render_Distortion();//디스토션 파티클

    /*PostProcessing*/
    void Render_RadialBlur();
    void Render_Crack();


    /*최종 렌더 (빼지마세요)*/
    void Render_FinalResult();

    void Render_UI();

private:
    HRESULT Ready_Targets();
    HRESULT Ready_MRTs();
    HRESULT Ready_LightDepth();
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

#ifdef _DEBUG
            private:
                _bool m_isHDR = { false };
                _bool m_isSSAO = { false };
                _bool m_isPBR = { false };
                _bool m_isBOF = { false };
                _bool m_isShadow = { false };
                _bool m_isRimLight = { true };
                _bool m_isRadialBlur = { false };
                _bool m_isMotionBlur = { false };

#else
            private:
                _bool m_isHDR = { true };
                _bool m_isSSAO = { true };
                _bool m_isPBR = { true };
                _bool m_isBOF = { true };
                _bool m_isShadow = { true };
                _bool m_isRimLight = { true };
                _bool m_isRadialBlur = { false };
                _bool m_isMotionBlur = { false };
#endif // DEBUG

    _float m_fHDRLight = { 1.f };

    _float m_fSSAORadiuse = { 0.032f };
    _float m_fSSAOBlur = { 2.f };
    _float m_fSSAOBiae = { 0.025f };
    _float4* m_vSSAOKernal;

    int      m_iRenderState = { RENDER_END };

#ifdef _DEBUG
    _bool m_isDebugView = { false };
#endif // _DEBUG


public:
    static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
END