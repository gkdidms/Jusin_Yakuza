/* PBR */
#include "Shader_PBR.hlsl"

PS_OUT PS_MAIN_LUMINANCE_SUM(PS_IN In) // »÷µµ ∏  ±∏«œ±‚ (√≥¿Ω Ω√¿€)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord);
    
    float vResult = log(dot(vDiffuse.xyz, Luminance) + fDelta);
    
    Out.vColor = vResult;
    
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
    //float vColor = log(dot(vDiffuse.xyz, Luminance) + 0.0001f);
  
    Out.vColor = exp(vDiffuse / 16);
    
    return Out;
}

PS_OUT PS_MAIN_LUMINANCE(PS_IN In) // «ˆ¿Á «¡∑π¿”¿« ∆Ú±’ »÷µµ ∏  ±∏«œ±‚ (√÷¡æ)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector fNew = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    vector fOld = g_CopyLuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vDiffuse = g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord);
    //«ˆ¿Á ∆Ú±’ »÷µµ
    //float fAvgLum = fOld + (fNew - fOld) * (1.f - pow(0.98f, 0.5f));
    //float fAvgLum = fOld.x * 0.5f + fNew.x * 0.5f;
    
    float fSceneLumi = (fOld * 0.5 + fNew * 0.5);
    float fMiddleGrayOfSceneLumi = 1.03 - (2 / (2 + log10(fSceneLumi + 1)));
    float fRgbLumi = dot(vDiffuse.xyz, Luminance) + 0.0001f;

    float fLumiScaled = (fRgbLumi * fMiddleGrayOfSceneLumi) / (fSceneLumi + 0.0001f);
    
    Out.vColor = vector(fLumiScaled, fLumiScaled, fLumiScaled, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_COPYLUMINANCE(PS_IN In) // ¿Ã¿¸ »÷µµ∏¶ ¿˙¿Â
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector fNew = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = fNew;
    
    return Out;
}

PS_OUT PS_MAIN_TONEMAPPING(PS_IN In) // ∞®∏∂ ƒ›∑∫º« & ACES ≈Ê∏≈«Œ
{
    PS_OUT Out = (PS_OUT) 0;
    
    //vector vLuminance = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vDiffuse = pow(g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord).xyz, 2.2f);
    float A = 2.51f, B = 0.03f, C = 2.43f, D = 0.59f, E = 0.14f;
    vDiffuse = saturate(vDiffuse * (A * vDiffuse + B)) / (vDiffuse * (C * vDiffuse + D) + E);

    Out.vColor = vector(pow(vDiffuse, 1.f / 2.2f), 1.f); // * vLuminance * g_fLumVar;
    
    return Out;
}