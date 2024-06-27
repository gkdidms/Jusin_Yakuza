#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Texture;
Texture2D g_ResultTexture;
Texture2D g_AccumColor;
Texture2D g_AccumAlpha;

vector g_vStartColor, g_vEndColor;
vector g_vCamPosition;

float g_NearZ = 0.01f;
float g_FarZ = 3000.f;

struct VS_IN
{
    //m_pVB
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
    //m_pInstance
    row_major matrix TransformMatrix : WORLD;
    float4 vDir : COLOR0;
    float2 vLifeTime : COLOR1;
    float vRectSize : COLOR2;
};

struct VS_OUT
{
    float3 vPosition : POSITION;
    float2 vPSize : TEXCOORD0;
    float4 vDir : COLOR0;
    float2 vLifeTime : COLOR1;
    float vRectSize : COLOR2;
};

/* 정점 셰이더 :  /* 
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    

    Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;//월드상
    Out.vPSize = In.vPSize;
    Out.vDir = In.vDir;
    Out.vLifeTime = In.vLifeTime;
    Out.vRectSize = In.vRectSize;

    return Out;
}

struct GS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : TEXCOORD0;

    float4 vDir : COLOR0;
    float2 vLifeTime : COLOR1;
    float vRectSize : COLOR2;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;

    float2 vLifeTime : COLOR0;
    float LinearZ : COLOR1;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vTexcoord = float2(0.f, 0.f);
        Out[i].vLifeTime = float2(0.f, 0.f);
        Out[i].LinearZ = float(0.f);
    }
	
    vector vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;

    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;

    vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;

    vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;

    
    Triangles.Append(Out[0]);
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.RestartStrip();

    Triangles.Append(Out[0]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[3]);
    Triangles.RestartStrip();
}

//파티클의 점 하나를 그리고 픽셀로 넘어간다 사각형 한개 생성후 픽셸로 감
[maxvertexcount(6)]
void GS_CUSTOM(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vTexcoord = float2(0.f, 0.f);
        Out[i].vLifeTime = float2(0.f, 0.f);
        Out[i].LinearZ = float(0.f);
    }
	
    float3 vDirection = In[0].vDir.xyz;
    
    vector vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);
    float3 vRight = normalize(cross(vDirection, vLook.xyz)) * In[0].vPSize.x * In[0].vRectSize * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * In[0].vRectSize* 0.5f;

    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f);//월드좌표
        //-1~1 x,y,z
    vector CamPos = mul(PointPosition, g_ViewMatrix);//뷰좌표(위치)
   // CamPos = mul(PointPosition, g_ProjMatrix);//투영 (w나누기전)
    
   // CamPos = CamPos / CamPos.w;//정규화된 윈도우 좌표계
    Out[0].LinearZ = CamPos.z; //정규화된 z 값을 가져옴(0~1)
    
        
    vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;

    Triangles.Append(Out[0]);
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.RestartStrip();

    Triangles.Append(Out[0]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[3]);
    Triangles.RestartStrip();
}
/* TriangleList인 경우 : 정점 세개를 받아서 w나누기를 각 정점에 대해서 수행한다. */
/* 뷰포트(윈도우좌표로) 변환. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를, 정점을 선형보간하여 만든다. -> 픽셀이 만들어졌다!!!!!!!!!!!! */


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
    float LinearZ : COLOR1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vAlpha : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a < 0.1f)
        discard;

    Out.vColor.a = (In.vLifeTime.x - In.vLifeTime.y) / In.vLifeTime.x;

    Out.vColor.rgb = float3(1.f, 1.f, 1.f);

    return Out;
}

PS_OUT PS_MAIN_SPREAD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector Color = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Color.a < 0.1f)
        discard;
    
    Out.vColor = Color;
    
    
	/*if (Out.vColor.a < 0.1f || 
		In.vLifeTime.y > In.vLifeTime.x)
		discard;*/
    //Out.vColor.a *= In.vLifeTime.x - In.vLifeTime.y;

    //Out.vColor.rgb = float3(1.f, 1.f, 1.f);


    return Out;
}

//지오메트리 어차피 그리는 순서나 픽셀이나 똑같다
PS_OUT PS_MAIN_SPREADCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //월드좌표

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    vector BackColor = g_AccumColor.Sample(PointSampler, In.vTexcoord);

    float4 LerpColor = lerp(g_vStartColor, g_vEndColor, In.vLifeTime.y / In.vLifeTime.x);
    
    vParticle *= LerpColor;
    
    float3 ColorN = vParticle.rgb;
    
    float AlphaN = vParticle.a;

    float fWeight = abs(PointPosition.z); //정규화된 z 값을 가져옴(0~1)    

    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
        
    return Out;
}


technique11 DefaultTechnique
{
	
	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
    pass Loop //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NonLoop //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPREAD();
    }

    pass Direction  //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_CUSTOM();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPREAD();
    }

    pass NoDirection    //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPREAD();
    }
    
    pass DirectionColor //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_CUSTOM();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPREADCOLOR();
    }

    pass WeightBlend //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_CUSTOM();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPREADCOLOR();
    }
}

