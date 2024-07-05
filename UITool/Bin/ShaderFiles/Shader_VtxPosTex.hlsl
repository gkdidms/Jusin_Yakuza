#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Texture;

float g_fProgress;

float4 g_vColor;

float3 g_vLifeTime;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a <= 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_ALPHABLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a <= 0.1f)
        discard;
    return Out;
}

PS_OUT PS_COLOR_ALPHABLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector BaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector vMergeColor = g_vColor;
    
    Out.vColor = vMergeColor * BaseColor;
    if (Out.vColor.a <= 0.1f)
        discard;
    return Out;
}

PS_OUT PS_COLOR_ALPHABLEND_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector BaseColor = g_Texture.Sample(PointSampler, In.vTexcoord);
    vector BaseAlpha = 0.f;
    vector MergeColor = g_vColor;
    vector FinalColor = vector(0.f, 0.f, 0.f, 0.f);
    
    if (g_vLifeTime.x < g_vLifeTime.y || g_vLifeTime.x > g_vLifeTime.z)
        discard;
    else
    {
        FinalColor = BaseColor * MergeColor;
        float MiddleTime =(g_vLifeTime.z - g_vLifeTime.y) / 2.f;
        float CurrentTime = (g_vLifeTime.x - g_vLifeTime.y);
        
        if (MiddleTime > CurrentTime)
        {
            BaseAlpha = CurrentTime / MiddleTime;
            FinalColor.a *= BaseAlpha;
        }
        else
        {
            BaseAlpha = (CurrentTime - MiddleTime) / MiddleTime;
            FinalColor.a *= 1.f-BaseAlpha;
        }
    }
    
    Out.vColor = FinalColor;
    
    if (Out.vColor.a <= 0.1f)
        discard;
    
    return Out;
}



PS_OUT PS_BACKBUFFER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a <= 0.1f)
        discard;
    
    return Out;
}

technique11 DefaultTechnique
{

    pass DefaultPass//0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Blend_Texture//1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBlend_Texture//2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ALPHABLEND();
    }

    pass BackBuffer//3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BACKBUFFER();
    }

    pass Color_AlphaBlend_Texture//4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_ALPHABLEND();
    }

    pass Color_AlphaBlend_Effect_Texture //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_ALPHABLEND_EFFECT();
    }
}