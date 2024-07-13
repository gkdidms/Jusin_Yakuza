#include "Engine_Shader_Defines.hlsli"
#include "Shader_Client_Defines.hlsli"

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    uint4 vBlendIndices : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHTS;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vLocalPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    matrix TransformMatrix = mul(g_BoneMatrices[In.vBlendIndices.x], In.vBlendWeights.x) +
    mul(g_BoneMatrices[In.vBlendIndices.y], In.vBlendWeights.y) +
    mul(g_BoneMatrices[In.vBlendIndices.z], In.vBlendWeights.z) +
    mul(g_BoneMatrices[In.vBlendIndices.w], fWeightW);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), TransformMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    //Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vNormal = normalize(vNormal);//법선
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition, 1.f;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    //Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));
    Out.vTangent = normalize(vector(In.vTangent.xyz, 0.f)); //접선
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);//바이
    
    return Out;
}

struct VS_OUT_LIGHTDEPTH
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

//그림자용
VS_OUT_LIGHTDEPTH VS_MAIN_LIGHTDEPTH(VS_IN In)
{
    VS_OUT_LIGHTDEPTH Out = (VS_OUT_LIGHTDEPTH) 0;

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix TransformMatrix = mul(g_BoneMatrices[In.vBlendIndices.x], In.vBlendWeights.x) +
    mul(g_BoneMatrices[In.vBlendIndices.y], In.vBlendWeights.y) +
    mul(g_BoneMatrices[In.vBlendIndices.z], In.vBlendWeights.z) +
    mul(g_BoneMatrices[In.vBlendIndices.w], fWeightW);

    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

// LightDepth용 GS
struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    
    uint fIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(9)]
void GS_MAIN_LIGHTDEPTH(triangle GS_IN In[3], inout TriangleStream<GS_OUT> Out)
{
    GS_OUT Output[3];
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            float4 vViewPos = mul(In[j].vPosition, g_ViewMatrixArray[i]);
            vViewPos.z += 2.5f;
            Output[j].vPosition = mul(vViewPos, g_ProjMatrixArray[i]);
            Output[j].vProjPos = In[j].vProjPos;
            Output[j].vTexcoord = In[j].vTexcoord;
            Output[j].fIndex = i;
            Out.Append(Output[j]);
        }
        Out.RestartStrip();
    }
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vLocalPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRM : SV_TARGET3;
    vector vRS : SV_Target4;
    vector vMulti : SV_Target5;
    vector vRD : SV_Target6;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMultiDiffuce = g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vTangentDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    //옵젝(물체 기준 좌표계) ->탄젠트 :법선벡터xyz*0.5+0.5=법선맵(0~1)렌더타겟 저장시 이렇게
    //탄젠트(표면 기준 좌표계) ->옵젝 : 법선맵rgb*2 -1 =법선벡터(-1~1)저장된 렌더타겟에서 꺼내 쓸떄.
    
    vector vTangentTexture =vector( vTangentDesc.xyzw * 2.f - 1.f); //탄젠트 노멀(텍스처로 받은 초록색 노멀 값)을 옵젝 노멀로 변경 범위 (-1~1)[접선]
    float3 vLocalTangent = float3(vTangentTexture.y, vTangentTexture.w, 1.f); //argb라고 가정하고 순서변경(텍스처는 y,z 교체가 안되서 들어옴)(접선 생성 완료)

    float3 vLocalNormal = In.vNormal.xyz; //이건 옵젝 노멀임[법선]

    
    vector vWorldTangent = normalize(vector(vLocalTangent.xyz, 0.f));
    vector vWorldNormal = normalize(vector(vLocalNormal.xyz, 0.f));
    
    vector vWorlBinormal = vector(cross(vWorldNormal.xyz, vWorldTangent.xyz), 0.f);

    
    float3x3 WorldMatrix = float3x3(vWorlBinormal.xyz, vWorldNormal.xyz, vWorldTangent.xyz);
    float3 vFinalNormal= mul(vWorldNormal.xyz, WorldMatrix);
    
    vFinalNormal = mul(vector(vWorldNormal.xyz, 0.f), g_WorldMatrix);
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    //RS + RD
    vector vRSRD;
    vector vRDDesc;
    if (g_isRD)
    {
        vRDDesc = g_RDTexture.Sample(LinearSampler, In.vTexcoord * 50.f);
        Out.vRD = vRDDesc;
    }
    
    if (g_isRS)
    {
        vector vRSDesc = g_RSTexture.Sample(LinearSampler, In.vTexcoord * 20.f);
        Out.vRS = vRSDesc;
        vRSDesc = lerp(vRSDesc, vRDDesc, 0.7f);
        Out.vDiffuse = lerp(vDiffuse, vRSDesc, vMultiDiffuce.z);
        
    }
    else
    {
        if (g_isRD)
            Out.vDiffuse = lerp(vRDDesc, vDiffuse, vMultiDiffuce.z);
        else
            Out.vDiffuse = vDiffuse;
    }
    float RimIndex = 0.f;
    if(g_isRimLight)
        RimIndex = 1.f;

    
    Out.vNormal = vector(vFinalNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, RimIndex, 1.f);
    Out.vMulti = vMultiDiffuce;
    
    return Out;
}

PS_OUT PS_BLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMultiDiffuce = g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    vDiffuse.xyz *= vDiffuse.a;
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    //RS + RD
    vector vRSRD;
    vector vRDDesc;
    if (g_isRD)
    {
        vRDDesc = g_RDTexture.Sample(LinearSampler, In.vTexcoord * 50.f);
        Out.vRD = vRDDesc;
    }
    
    if (g_isRS)
    {
        vector vRSDesc = g_RSTexture.Sample(LinearSampler, In.vTexcoord * 20.f);
        Out.vRS = vRSDesc;
        vRSDesc = lerp(vRSDesc, vRDDesc, 0.7f);
        Out.vDiffuse = lerp(vDiffuse, vRSDesc, vMultiDiffuce.z);
        
    }
    else
    {
        if (g_isRD)
            Out.vDiffuse = lerp(vDiffuse, vRDDesc, vMultiDiffuce.z);
        else
            Out.vDiffuse = vDiffuse;
    }
    float RimIndex = 0.f;
    if (g_isRimLight)
        RimIndex = 1.f;
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, RimIndex, 1.f);
    Out.vMulti = vMultiDiffuce;
    

    return Out;
}

struct PS_IN_LIGHTDEPTH
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT_LIGHTDEPTH
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN_LIGHTDEPTH In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;
	
    Out.vLightDepth = vector(In.vProjPos.w / 1000.f, 0.0f, 0.f, 0.f);

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

    pass BlendPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND();
    }

    pass LightDepth
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN_LIGHTDEPTH();
        GeometryShader = compile gs_5_0 GS_MAIN_LIGHTDEPTH();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

}