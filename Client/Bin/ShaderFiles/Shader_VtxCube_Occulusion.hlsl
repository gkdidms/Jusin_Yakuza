#include "Engine_Shader_Defines.hlsli"
#include "Shader_Client_Defines.hlsli"

struct VS_IN
{
    float3 vPosition : POSITION; // 12����Ʈ
    //float padding1 : TEXCOORD0; // 4����Ʈ �е� (���� ����)

    float3 vTexcoord : TEXCOORD1; // 12����Ʈ
    //float padding2 : TEXCOORD2; // 4����Ʈ �е� (���� ����)
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vTexcoord : TEXCOORD0;
};



// Occulusion�� ����

struct VS_IN_Depth
{
    float3 vPosition : POSITION; // 12����Ʈ
    float3 vTexcoord : TEXCOORD0; // 12����Ʈ
};

struct VS_OUT_Depth
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};


struct PS_IN_Depth
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};



/* ���� ���̴� :  /* 
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */
VS_OUT_Depth VS_MAIN_Depth(VS_IN In)
{
    VS_OUT_Depth Out = (VS_OUT_Depth) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = mul(float4(In.vPosition, 1.f), matWVP);

    return Out;
}

struct PS_OUT_Depth
{
    vector vDepth : SV_TARGET0;
};

PS_OUT_Depth PS_DEPTH(PS_IN_Depth In)
{
    PS_OUT_Depth Out = (PS_OUT_Depth) 0;
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0, 0.f);
    
    return Out;
}



technique11 DefaultTechnique
{

    
    pass OcculuderDepthPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_Depth();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DEPTH();
    }

}

