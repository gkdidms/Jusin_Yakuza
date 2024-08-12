#include "Shader_Effect.hlsl"

int g_iSample = 10;
PS_OUT PS_MAIN_RADIALBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBlurColor = vector(0.f, 0.f, 0.f, 0.f);
    
    float2 vCenter = float2(0.5f, 0.5f);
    float fPower = 0.01f;
    
    float2 vDir = In.vTexcoord - vCenter;
    float f = 1.f / float(g_iSample);
    
    for (int i = 0; i < g_iSample; ++i)
    {
        vBlurColor += g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord - fPower * vDir * float(i)) * f;
    }
    
    Out.vColor = vector(vBlurColor.xyz, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_SCREENCRACK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBlurColor = vector(0.f, 0.f, 0.f, 0.f);
    
    float2 vCenter = float2(0.5f, 0.5f);
    float fPower = 0.01f;
    
    float2 vDir = In.vTexcoord - vCenter;

    vBlurColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vector(vBlurColor.xyz, 1.f);
    
    return Out;
}

float3 RGBToHSL(float3 color)
{
    float fMax = max(max(color.r, color.g), color.b);
    float fMin = min(min(color.r, color.g), color.b);
    float h = 0.0;
    float s = 0.0;
    float l = (fMax + fMin) / 2.0;

    if (fMax != fMin)
    {
        float d = fMax - fMin;
        s = l > 0.5 ? d / (2.0 - fMax - fMin) : d / (fMax + fMin);

        if (fMax == color.r)
            h = (color.g - color.b) / d + (color.g < color.b ? 6.0 : 0.0);
        else if (fMax == color.g)
            h = (color.b - color.r) / d + 2.0;
        else
            h = (color.r - color.g) / d + 4.0;

        h /= 6.0;
    }

    return float3(h, s, l);
}

PS_OUT PS_MAIN_INVERTCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vector(1.f - vColor.xyz, 1.f);
    
    return Out;
}

float g_fVignetteIntensity = 0.5f;
float g_fVignetteSmootness = 0.2f; // 가장자리의 어두운 영역이 부드럽게 확장되는 정도

PS_OUT PS_MAIN_VIGNETTE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vector(1.f - vColor.xyz, 1.f);
    
    float2 vPosition = In.vTexcoord * 2.f - 1.f;
    float vDistance = length(vPosition);
    
    float vVignette = smoothstep(g_fVignetteIntensity, g_fVignetteIntensity + g_fVignetteSmootness, vDistance);
    
    vColor.rgb *= (1.f - vVignette);
    
    Out.vColor = vector(vColor.rgb, 1.f);
    
    return Out;
}