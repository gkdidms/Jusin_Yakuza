
#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix , g_WorldInvMatrix, g_ViewInvMatrix;
textureCUBE	 g_Texture;
Texture2D g_Texture2D;
Texture2D g_DepthTexture;

//Decal
float2 g_RenderResolution = float2(1280, 720);

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

struct VS_OUT_DECAL
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
    float3 vDecalPos : TEXCOORD1;
};

/* 정점 셰이더 :  /* 
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
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

VS_OUT_DECAL VS_MAIN_DECAL(VS_IN In)
{
    VS_OUT_DECAL Out = (VS_OUT_DECAL) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vDecalPos = mul(float4(In.vPosition, 1.f), matWVP);

    return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vTexcoord : TEXCOORD0;
};

struct PS_IN_DECAL
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
    float3 vDecalPos : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
//	Out.vColor = 1.f;

	return Out;
}

PS_OUT PS_DECAL(PS_IN_DECAL In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    //float2 vScreenPos = In.vPosition.xy / g_RenderResolution;
    ////vScreenPos.x = In.vPosition.x * 0.5 + 0.5;
    ////vScreenPos.y = -In.vPosition.y * 0.5 + 0.5;
   
    //// PositionTarget 에서 현재 호출된 픽셀쉐이더랑 동일한 지점에 접근해서 좌표값을 확인
    //float4 vViewDepth = g_DepthTexture.Sample(LinearSampler, vScreenPos);
    
    //// Deferred 단계에서 그려진게 없을경우
    //if (0 == vViewDepth.x)
    //{
    //    discard;
    //}
    
    //float4 vViewPos = g_tex_1.Sample(g_sam_0, vScreenUV);
    
    
    
    //float4 vPos = float4(vScreenPos.xy * 2.f - 1.f, vViewDepth.x, 1);
    
    //matrix mat_0 = g_ViewInvMatrix * g_WorldInvMatrix;
    
    //// Cube 볼륨메쉬의 로컬 공간으로 데려간다.
    //float3 vLocal = mul(vPos, mat_0).xyz;
    //vLocal += 0.5f;
    ////if (vLocal.x < 0.f || 1.f < vLocal.x ||
    ////    vLocal.y < 0.f || 1.f < vLocal.y ||
    ////    vLocal.z < 0.f || 1.f < vLocal.z)
    ////{
    ////    discard;
    ////}
    
    
    //Out.vColor = g_Texture2D.Sample(LinearSampler, vLocal.xy);
    
    //Out.vColor = 1;
    
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

    pass DefaultPassCube
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_DECAL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DECAL();
    }



}

