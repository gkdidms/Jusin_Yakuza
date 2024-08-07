#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_fProgress;
float2 g_lifeAlpha;
float g_iSpriteIndex;
float2 g_fUVCount;
float g_fCurTime;

Texture2D g_Texture;
Texture2D g_DiffuseTexture;
bool g_isAttach;

float g_fFar = 3000.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
        //m_pInstance
    row_major matrix TransformMatrix : WORLD;
    float4 vDir : COLOR0;
    float2 vLifeTime : COLOR1;
    float2 vRectSize : COLOR2;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
};

struct VS_NONBLENDER_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;

    float2 vLifeTime : COLOR0;
    float4 vProjPos : COLOR1;
    float4 vNormal : NORMAL;
};

/* ���� ���̴� :  /* 
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP, matVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    if(g_isAttach)
    {
        vector Right = In.TransformMatrix._11_12_13_14 * In.vRectSize.x;
        vector Up = In.TransformMatrix._21_22_23_24 * In.vRectSize.x;
        vector Look = In.TransformMatrix._31_32_33_34 * In.vRectSize.x;
        vector Pos = In.TransformMatrix._41_42_43_44;
        matrix Transform = matrix(Right, Up, Look, Pos);
    
    
        vector vPosition = mul(float4(In.vPosition, 1.f), Transform); //�����̵�.
    
        Out.vPosition = mul(vPosition, matWVP);

        Out.vTexcoord = In.vTexcoord;
        Out.vLifeTime = In.vLifeTime;
    }
    else
    {
        vector Right = In.TransformMatrix._11_12_13_14 * In.vRectSize.x;
        vector Up = In.TransformMatrix._21_22_23_24 * In.vRectSize.x;
        vector Look = In.TransformMatrix._31_32_33_34 * In.vRectSize.x;
        vector Pos = In.TransformMatrix._41_42_43_44;
        matrix Transform = matrix(Right, Up, Look, Pos);
    
    
        vector vPosition = mul(float4(In.vPosition, 1.f), Transform); //�����̵�.
    
        Out.vPosition = mul(vPosition, matVP);

        Out.vTexcoord = In.vTexcoord;
        Out.vLifeTime = In.vLifeTime;
    }



    return Out;
}

VS_NONBLENDER_OUT VS_NONBLENDER(VS_IN In)
{
    VS_NONBLENDER_OUT Out = (VS_NONBLENDER_OUT) 0;

    matrix matWV, matWVP, matVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    
    if(g_isAttach)
    {
        vector Right = In.TransformMatrix._11_12_13_14 * In.vRectSize.x;
        vector Up = In.TransformMatrix._21_22_23_24 * In.vRectSize.x;
        vector Look = In.TransformMatrix._31_32_33_34 * In.vRectSize.x;
        vector Pos = In.TransformMatrix._41_42_43_44;
        matrix Transform = matrix(Right, Up, Look, Pos);
    

        vector vPosition = mul(float4(In.vPosition, 1.f), Transform); //�����̵�.
    
        Out.vPosition = mul(vPosition, matWVP);
        Out.vTexcoord = In.vTexcoord;
        Out.vLifeTime = In.vLifeTime;
        Out.vProjPos = Out.vPosition;
        Out.vNormal = Look;
    }
    else
    {
        vector Right = In.TransformMatrix._11_12_13_14 * In.vRectSize.x;
        vector Up = In.TransformMatrix._21_22_23_24 * In.vRectSize.x;
        vector Look = In.TransformMatrix._31_32_33_34 * In.vRectSize.x;
        vector Pos = In.TransformMatrix._41_42_43_44;
        matrix Transform = matrix(Right, Up, Look, Pos);
    
    
        vector vPosition = mul(float4(In.vPosition, 1.f), Transform); //�����̵�.
    
        Out.vPosition = mul(vPosition, matVP);
        Out.vTexcoord = In.vTexcoord;
        
        Out.vLifeTime = In.vLifeTime;
        Out.vProjPos = Out.vPosition;
        Out.vNormal = Look;
    }
   


    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
};

struct PS_NOBLENDER_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
    float4 vProjPos : COLOR1;
    float4 vNormal : NORMAL;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vAlpha : SV_TARGET1;
    vector vDistortion : SV_TARGET2;
};

struct PS_NOBLENDER_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRM : SV_TARGET3;
    vector vRS : SV_Target4;
    vector vMulti : SV_Target5;
    vector vRD : SV_Target6;
};


//������Ʈ�� ������ �׸��� ������ �ȼ��̳� �Ȱ���
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //�������� ��ǥ

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float3 ColorN = vParticle.rgb;
    
    float AlphaN = vParticle.a;
        
    AlphaN *= lerp(g_lifeAlpha.x, g_lifeAlpha.y, In.vLifeTime.y / In.vLifeTime.x);
    
    float fWeight = abs(PointPosition.z); //����ȭ�� z ���� ������(0~1)    

    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
    Out.vDistortion = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return Out;
}

PS_NOBLENDER_OUT PS_NONBLEDNER(PS_NOBLENDER_IN In)
{
    PS_NOBLENDER_OUT Out = (PS_NOBLENDER_OUT) 0;

    float4 PointPosition = In.vPosition; //������ǥ

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = vector(vParticle.xyz, 1.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass //0
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NonBlenderPass //1
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_NONBLENDER();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_NONBLEDNER();
    }
}

