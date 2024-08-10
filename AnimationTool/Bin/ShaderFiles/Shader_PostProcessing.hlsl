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