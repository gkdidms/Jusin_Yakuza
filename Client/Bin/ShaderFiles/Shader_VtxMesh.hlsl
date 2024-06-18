#include "Engine_Shader_Defines.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_NoiseTexture;

float g_fFar;
float g_fTimeDelta;
float g_fOut = { 0.5f };

vector vOutlineColor = { 0.9f, 0.7f, 0.2f, 1.f };
//vector vOutlineColor = { 1.f, 0.f, 0.f, 1.f };
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vTangent : TANGENT;
    float4 vBiNormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vTangent : TANGENT;
    float4 vBiNormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vDiffuse.a < 0.1f)
        Out.vDiffuse.a = 1.f;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_DISCARD_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
    
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_DISSOLVE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord).r;
    
    if (Out.vDiffuse.a < 0.1f)
        Out.vDiffuse.a = 1.f;
    
    float fTime = g_fTimeDelta * g_fOut;
    float fAlpha;
    if (vNoise <= fTime)
        fAlpha = 1.f;
    else
        fAlpha = 0.f;
    
    if (fAlpha == 0.f)
        discard;
    
    float fOutline;
    if (vNoise <= fTime * 0.9f)
        fOutline = 0;
    else
        fOutline = 1;
    
    if (fOutline == 1.f)
        Out.vDiffuse = vector(fOutline * vOutlineColor.rgb, 1.f);
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
    
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_DISSOLVE_INV_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord).r;
   
    if (Out.vDiffuse.a < 0.1f)
        Out.vDiffuse.a = 1.f;
    
    float fTime = g_fTimeDelta * g_fOut;
    float fAlpha;
    if (vNoise >= fTime)
        fAlpha = 1.f;
    else
        fAlpha = 0.f;
    
    if (fAlpha == 0.f)
        discard;
    
    float fOutline;
    if (vNoise >= fTime * 0.9f)
        fOutline = 0;
    else
        fOutline = 1;
    
    if (fOutline == 1.f)
        Out.vDiffuse = vector(fOutline * vOutlineColor.rgb, 1.f);
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
    
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_DISSOLVE_AHPHA_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord).r;
    
    float fTime = g_fTimeDelta * g_fOut;
    float fAlpha;
    if (vNoise <= fTime)
        fAlpha = 1.f;
    else
        fAlpha = 0.f;
    
    if (fAlpha == 0.f || Out.vDiffuse.a < 0.1f)
        discard;
    
    float fOutline;
    if (vNoise <= fTime * 0.9f)
        fOutline = 0;
    else
        fOutline = 1;
    
    if (fOutline == 1.f)
        Out.vDiffuse = vector(fOutline * vOutlineColor.rgb, 1.f);
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
    
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_WINDFIELD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = float2(In.vTexcoord.x + g_fTimeDelta * 0.2f, In.vTexcoord.y);
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass DefaultPass2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISCARD_MAIN();
    }

    pass Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_MAIN();
    }

    pass Dissolve_Inv
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_INV_MAIN();
    }

    pass Dissolve_Alpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_AHPHA_MAIN();
    }

    pass WindField
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WINDFIELD();
    }
}