
#include "Engine_Shader_Defines.hlsli"

/* ������Ʈ ���̺�(������̺�) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_WorldMatrixInv, g_ViewMatrixInv, g_ProjMatrixInv, g_ReflectViewMatrix;
matrix g_ViewMatrixArray[3], g_ProjMatrixArray[3];

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_RMTexture;
Texture2D g_RefractionTexture;
Texture2D g_RSTexture;
Texture2D g_DepthTexture;
Texture2D g_NoiseTexture;
Texture2D g_BlurReverseTexture;
Texture2D g_MultiDiffuseTexture;
Texture2D g_RDTexture;
Texture2D g_ReflectionTexture;

float g_fObjID;
float g_fRefractionScale = { 0.001f };

float g_fFar = { 3000.f };
float g_fTimeDelta;
float g_fSpeed = 2.f;
bool g_bExistNormalTex;
bool g_bExistRMTex;
bool g_bExistRSTex;
bool g_bReflExist;

vector g_vCamPosition;

float2 g_RenderResolution = float2(1280, 720);

bool g_isRS;
bool g_isRD;

int g_iCount = { 0 };
const int g_iMaxCasCade = { 3 };

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
    float4 vLocalPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

/* ���� ���̴� :  /* 
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */
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
    Out.vLocalPos = float4(In.vPosition, 1.f);
    Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);


    return Out;
}

struct VS_OUT_LIGHTDEPTH
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT_LIGHTDEPTH VS_MAIN_LIGHTDEPTH(VS_IN In)
{
    VS_OUT_LIGHTDEPTH Out = (VS_OUT_LIGHTDEPTH) 0;
    
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;

}

// LightDepth�� GS
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

[maxvertexcount(28)]
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
    
   //  ������ ���(0.1���� ������ �����ϴ�) �׸��� ����
    //if (vDiffuse.a < 0.1f)
     //   discard;
    
     //RS + RD
    vector vRSRD;
    
    if (g_isRS)
    {
        vector vRSDesc = g_RSTexture.Sample(LinearSampler, In.vTexcoord);
        Out.vRS = vRSDesc;
        Out.vDiffuse = lerp(vDiffuse, vRSDesc, vMultiDiffuce.z);
    }
    else
        Out.vDiffuse = vDiffuse;
    
    float3 vNormal;
    if (true == g_bExistNormalTex)
    {
        // ���εǴ� texture�� ������
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
        vNormal = mul(vNormal.xyz, WorldMatrix);
    }
    else
    {
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        // �ؽ�ó ������
        vNormal = mul(In.vNormal.xyz, WorldMatrix);
    }
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    Out.vMulti = vMultiDiffuce;
    
    
    // specularTex�� metalic ���� rm ��� - bool �� ���� ����ϱ�
    if (true == g_bExistRMTex)
    {
        Out.vRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    }
    
    if (true == g_bExistRSTex)
    {
        Out.vRS = g_RSTexture.Sample(LinearSampler, In.vTexcoord);
    }
    
    
    return Out;
}

PS_OUT PS_GLASSDOOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vRefractTexCoord;
    vRefractTexCoord.x = In.vProjPos.x / In.vProjPos.w / 2.0f + 0.5f;
    vRefractTexCoord.y = -In.vProjPos.y / In.vProjPos.w / 2.0f + 0.5f;


    if (true == g_bExistNormalTex)
    {
        // Normal texture ������ vTexcoord �ٽ�
        float3 normal;
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
        normal = vNormalDesc.xyz * 2.f - 1.f;
        vRefractTexCoord = vRefractTexCoord + (normal.xy * g_fRefractionScale);
    }

    // Refract - ���� �ڿ� ��ġ�� ��
    float4 vRefractColor = g_RefractionTexture.Sample(LinearSampler, vRefractTexCoord);
    float4 vGlassTexColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vFinalColor = lerp(vRefractColor, vGlassTexColor, 0.5f);
    
    Out.vDiffuse = vFinalColor;
    
	
    // ������ ���(0.1���� ������ �����ϴ�) �׸��� ����
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    float3 vNormal;
    if (true == g_bExistNormalTex)
    {
        // ���εǴ� texture�� ������
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
        vNormal = mul(vNormal.xyz, WorldMatrix);
    }
    else
    {
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        // �ؽ�ó ������
        vNormal = mul(In.vNormal.xyz, WorldMatrix);
    }
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    
    // specularTex�� metalic ���� rm ��� - bool �� ���� ����ϱ�
    if (true == g_bExistRMTex)
    {
        Out.vRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    }
    
    
    return Out;
}

PS_OUT PS_PUDDLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // �� ������ ��� ���
    vector vMaskTexture = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if(0.3 > vMaskTexture.r)
        discard;
    
    
    float2 vRefractTexCoord;
    vRefractTexCoord.x = In.vProjPos.x / In.vProjPos.w / 2.0f + 0.5f;
    vRefractTexCoord.y = -In.vProjPos.y / In.vProjPos.w / 2.0f + 0.5f;
    
    // �� �����̴°�
    float2 vTexUV = float2(In.vTexcoord.x + g_fTimeDelta * g_fSpeed, In.vTexcoord.y);
    vector normal = g_NormalTexture.Sample(LinearSampler, vTexUV);
    // �� ǥ��(�� �� ����)
    vector vRefractionColor = g_RefractionTexture.Sample(LinearSampler, vRefractTexCoord + 0.02 * normal.xy);

    
    float2 vScreenPos = In.vPosition.xy / g_RenderResolution;
    float4 vViewDepth = g_DepthTexture.Sample(LinearSampler, vScreenPos.xy);
    
    vector vWorldPos;
    vWorldPos.x = vScreenPos.x * 2.f - 1.f;
    vWorldPos.y = vScreenPos.y * -2.f + 1.f;
    vWorldPos.z = vViewDepth.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * (vViewDepth.y * 3000.f);
    
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv); // world
    
    float3 viewDir = normalize(vWorldPos.xyz - g_vCamPosition.xyz);

    float3 reflectedDir = normalize(reflect(viewDir, In.vNormal.xyz));
    
    float fLength = length(g_vCamPosition.xyz - vWorldPos.xyz);
    
    float4 vReflectPos = vWorldPos + float4(reflectedDir.xyz, 0) * 1;
    vReflectPos = float4(vReflectPos.xyz, 1);
    vReflectPos.y -= 1;
    
    float bReflect = true;
    
    float4 vReflectScreenPos = mul(vReflectPos, g_ViewMatrix);
    vReflectScreenPos = mul(vReflectScreenPos, g_ProjMatrix);
    vReflectScreenPos /= vReflectScreenPos.w;
    vReflectScreenPos.x = vReflectScreenPos.x * 0.5 + 0.5;
    vReflectScreenPos.y = vReflectScreenPos.y * -0.5 + 0.5;
    
    vector vDepth = g_DepthTexture.Sample(LinearSampler, vReflectScreenPos.xy);
    
    vector vCalculateWorld = 0;
    vCalculateWorld.x = vReflectScreenPos.x * 2.f - 1.f;
    vCalculateWorld.y = vReflectScreenPos.y * -2.f + 1.f;
    vCalculateWorld.z = vDepth.x; /* 0 ~ 1 */
    vCalculateWorld.w = 1.f;
    
    vCalculateWorld = vCalculateWorld * (vDepth.y * 3000.f);
    
    vCalculateWorld = mul(vCalculateWorld, g_ProjMatrixInv);

	/* ���彺���̽� ���� ��ġ�� ���Ѵ�. */
    vCalculateWorld = mul(vCalculateWorld, g_ViewMatrixInv);
    
    float fDistance = length(g_vCamPosition.xyz - vCalculateWorld.xyz);
    if (fDistance > 10)
    {
        bReflect = false;
    }

    if(true == bReflect)
    {
        vector reflectColor = g_BlurReverseTexture.Sample(PointSampler, vReflectScreenPos.xy + 0.02 * normal.xy);

        Out.vDiffuse = reflectColor;
    }
    else
    {
        Out.vDiffuse = vRefractionColor;
    }
    

	
    // ������ ���(0.1���� ������ �����ϴ�) �׸��� ����
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    float3 vNormal;
    if (true == g_bExistNormalTex)
    {
        // ���εǴ� texture�� ������
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, vTexUV);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
        vNormal = mul(vNormal.xyz, WorldMatrix);
    }
    else
    {
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        // �ؽ�ó ������
        vNormal = mul(In.vNormal.xyz, WorldMatrix);
    }
    
    // �ݻ翡�� ����
    //Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vNormal = 0;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    
    // specularTex�� metalic ���� rm ��� - bool �� ���� ����ϱ�
    if (true == g_bExistRMTex)
    {
        Out.vRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    }
    
    
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
    pass DefaultPass //0
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

    pass GlassDoorPass //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_GLASSDOOR();
    }


    pass PuddlePass //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_PUDDLE();
    }
    

    pass LightDepth //3 - construction , Construction�� render light depth���� �������ֱ�
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_MAIN_LIGHTDEPTH();
        GeometryShader = compile gs_5_0 GS_MAIN_LIGHTDEPTH();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

   
}

