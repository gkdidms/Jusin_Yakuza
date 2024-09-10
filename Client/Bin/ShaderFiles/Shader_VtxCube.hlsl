#include "Engine_Shader_Defines.hlsli"
#include "Shader_Client_Defines.hlsli"

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vTexcoord : TEXCOORD0;
};


/* ���� ���̴� :  /* 
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);		
	Out.vTexcoord = In.vTexcoord;

	return Out;
}


VS_OUT VS_ENVIRONMENT(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vPosition;

    return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vTexcoord : TEXCOORD0;
};



struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = 0.f;

	return Out;
}

PS_OUT PS_DECAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vScreenPos = In.vPosition.xy / g_RenderResolution;
   
    // PositionTarget ���� ���� ȣ��� �ȼ����̴��� ������ ������ �����ؼ� ��ǥ���� Ȯ��
    float4 vViewDepth = g_DepthTexture.Sample(LinearSampler, vScreenPos);
    
    // Deferred �ܰ迡�� �׷����� �������
    if (0 == vViewDepth.x)
    {
        discard;
    }
    
    vector vLocalPos;
    vLocalPos.x = vScreenPos.x * 2.f - 1.f;
    vLocalPos.y = vScreenPos.y * -2.f + 1.f;
    vLocalPos.z = vViewDepth.x; /* 0 ~ 1 */
    vLocalPos.w = 1.f;
    
    vLocalPos = vLocalPos * (vViewDepth.y * g_fFar);
    
    vLocalPos = mul(vLocalPos, g_ProjMatrixInv);

	/* ���彺���̽� ���� ��ġ�� ���Ѵ�. */
    vLocalPos = mul(vLocalPos, g_ViewMatrixInv);
    
    vLocalPos = mul(vLocalPos, g_WorldMatrixInv);
    
    // Cube �����޽��� ���� �������� ��������
    vLocalPos += 0.5f;
    
    if (vLocalPos.x < 0.f || 1.f < vLocalPos.x ||
        vLocalPos.y < 0.f || 1.f < vLocalPos.y ||
        vLocalPos.z < 0.f || 1.f < vLocalPos.z)
    {
        discard;
    }
    
    
    vector vDecalColor = g_Texture.Sample(LinearSampler, vLocalPos.xy);

    if(vDecalColor.a < 0.3)
        discard;
    
    
    
    Out.vColor = vDecalColor;
    
    
    return Out;
}


// Occulusion�� ����

struct VS_IN_Depth
{
    float3 vPosition : POSITION;
    float3 vTexcoord : TEXCOORD0;
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
	pass DefaultPass
	{	
		SetRasterizerState(RS_Cull_CW);
		SetDepthStencilState(DSS_None_Test_None_Write, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass DecalCubePass
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DECAL();
    }

    
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

