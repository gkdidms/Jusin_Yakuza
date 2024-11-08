/* PBR */
#include "Shader_PBR.hlsl"

PS_OUT PS_MAIN_LUMINANCE_SUM(PS_IN In) // »÷µµ ∏  ±∏«œ±‚ (√≥¿Ω Ω√¿€)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord);
    
    float vResult = log(dot(vDiffuse.xyz, Luminance) + fDelta);
    
    Out.vColor = vDiffuse;
    
    return Out;
}

PS_OUT PS_MAIN_LUMINANCE_SUM_LOOP(PS_IN In) // »÷µµ ∏  ±∏«œ±‚ (∑Á«¡)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse;
        
    return Out;
}

PS_OUT PS_MAIN_LUMINANCE_SUM_FINAL(PS_IN In) // »÷µµ ∏  ±∏«œ±‚ (1x1)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    float vColor = log(dot(vDiffuse.xyz, Luminance) + 0.0001f);
  
    Out.vColor = exp(vColor / 16);
    
    return Out;
}

PS_OUT PS_MAIN_LUMINANCE(PS_IN In) // «ˆ¿Á «¡∑π¿”¿« ∆Ú±’ »÷µµ ∏  ±∏«œ±‚ (√÷¡æ)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector fNew = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    float vColor = log(dot(fNew.xyz, Luminance) + 0.0001f);
    fNew = exp(vColor / 16);
    
    vector fOld = g_CopyLuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    
    float fAvgLum = fOld + (fNew - fOld) * (1.f - pow(0.98f, 0.5f));
    
    Out.vColor = vector(fAvgLum, fAvgLum, fAvgLum, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_COPYLUMINANCE(PS_IN In) // ¿Ã¿¸ »÷µµ∏¶ ¿˙¿Â
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector fNew = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    float vColor = log(dot(fNew.xyz, Luminance) + 0.0001f);
    fNew = exp(vColor / 16);
    
    Out.vColor = fNew;
    
    return Out;
}

float3 ToneMap(float3 x)
{
    return saturate(x * (A * x + B)) / (x * (C * x + D) + E);
}

PS_OUT PS_MAIN_TONEMAPPING(PS_IN In) // ∞®∏∂ ƒ›∑∫º« & ACES ≈Ê∏≈«Œ
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vLuminance = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vDiffuse = pow(g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord).xyz, 2.2f);
    
    float fExposure = g_fLumVar / (vLuminance + 0.001f);

    vDiffuse = ToneMap(vDiffuse.xyz * fExposure);
    
    Out.vColor = vector(pow(vDiffuse, 1.f / 2.2f), 1.f);
    
    return Out;
}