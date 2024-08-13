struct GS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    uint4 vBlendIndices : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHTS;
};

struct GS_OUT
{
    float4 vPosition : POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vTangent : TANGENT;
    float4 vDiffuse : TEXCOORD1;
    float4 vNormalDesc : TEXCOORD2;
    float4 vSurface : TEXCOORD3;
    float4 vOEShader : TEXCOORD4;
    float4 vSpeculer : TEXCOORD5;
};

matrix g_BoneMatrices[512] : register(b0);

cbuffer TextureBuffer : register(b1)
{
    int g_isRS;
    int g_isRD;
    int g_isRM;
    int g_isRT;
    int g_isNormal;
    int g_isMulti;

    
    float2 TextureSize[7];
}

cbuffer MaterialBuffer : register(b2)
{
    int g_isUVShader = { false };

    float Opacity = { 0.f };
    float fSpecularPower = { 0.f };

    float AssetShader = { 0.f };
    float DisableRDRT = { 0.f };
    float Engine = { 0.f };
    float IsOEClothShader = { 0.f };
    float SkinShader = { 0.f };
    float Rough = { 0.f };
    float IsY3Shader = { 0.f };
    float Imperfection = { 0.f };
    float SPShader = { 0.f };

    float g_fRTX = { 0.f };
    float g_fRTY = { 0.f };
    float g_fRDRMRS_X = { 0.f };
    float g_fRDRMRS_Y = { 0.f };
    float f_fImperfection_UV = { 0.f };
    float f_fEngine_UV = { 0.f };
    
    float padding1;
    float padding2;

    vector vSpecularColor;
}

StructuredBuffer<GS_IN> g_InputBuffer : register(t0);
RWStructuredBuffer<GS_OUT> g_OutputBuffer : register(u0);

Texture2D<float4> g_DiffuseTexture : register(t1);
Texture2D<float4> g_NormalTexture : register(t2);
Texture2D<float4> g_RTTexture : register(t3);
Texture2D<float4> g_RMTexture : register(t4);
Texture2D<float4> g_RSTexture : register(t5);
Texture2D<float4> g_RDTexture : register(t6);
Texture2D<float4> g_MultiTexture : register(t7);

RWTexture2D<float4> g_OutDiffuseTexture : register(u1);
RWTexture2D<float4> g_OutNormalTexture : register(u2);
RWTexture2D<float4> g_OutSurfaceTexture : register(u3);
RWTexture2D<float4> g_OutOEShaderTexture : register(u4);
RWTexture2D<float4> g_OutSpecularTexture : register(u5);

float2 g_WindowSize = { 1280.f, 720.f };