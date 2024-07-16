#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vCamPosition;

Texture2D g_ToneTexture;
Texture2D g_UVAnimTexture;
Texture2D g_BaseAlphaTexture;
Texture2D g_FluidTexture;

float g_fProgress;
float2 g_lifeAlpha;
float g_iSpriteIndex;
float2 g_fUVCount;
float g_fCurTime;

float g_NearZ = 0.01f;
float g_FarZ = 3000.f;
//vLifeTime.x ����ð� /vLifeTime.y ����ð� / vLifeTime.z �����ε���
struct VS_IN
{
    //m_pVB
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
    //m_pInstance
    row_major matrix TransformMatrix : WORLD;
    float4 vDir : COLOR0;
    float2 vLifeTime : COLOR1;
    float2 vRectSize : COLOR2;
};

struct VS_OUT
{
    float3 vPosition : POSITION;
    float2 vPSize : TEXCOORD0;
    float4 vDir : COLOR0;
    float2 vLifeTime : COLOR1;
    float2 vRectSize : COLOR2;
};

/* ���� ���̴� :  /* 
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */

VS_OUT VS_LOCAL(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

    //matrix matWV, matWVP;

  //  matWV = mul(g_WorldMatrix, g_ViewMatrix);
  //  matWVP = mul(matWV, g_ProjMatrix);
    

    Out.vPosition = vPosition.xyz;
    Out.vPSize = In.vPSize;
    Out.vDir = In.vDir;
    Out.vLifeTime = In.vLifeTime;
    Out.vRectSize = In.vRectSize;

    return Out;
}

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix); //�����̵�.

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    

    Out.vPosition = mul(vPosition, g_WorldMatrix).xyz; //�����
    Out.vPSize = In.vPSize;
    Out.vDir = normalize(mul(In.vDir, g_WorldMatrix));
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
    float2 vRectSize : COLOR2;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vAlphaTex : TEXCOORD1;
    
    float2 vLifeTime : COLOR0;
    float LinearZ : COLOR1;
};

float4x4 RotationMatrix(float3 axis, float angle)
{
    float c, s;
    sincos(angle, s, c);

    float3 a = normalize(axis);
    float3x3 rotation =
    {
        a.x * a.x * (1 - c) + c, a.x * a.y * (1 - c) - a.z * s, a.x * a.z * (1 - c) + a.y * s,
        a.y * a.x * (1 - c) + a.z * s, a.y * a.y * (1 - c) + c, a.y * a.z * (1 - c) - a.x * s,
        a.z * a.x * (1 - c) - a.y * s, a.z * a.y * (1 - c) + a.x * s, a.z * a.z * (1 - c) + c
    };

    return float4x4(
        rotation[0], 0,
        rotation[1], 0,
        rotation[2], 0,
        float3(0, 0, 0), 1
    );
}

//��ƼŬ�� �� �ϳ��� �׸��� �ȼ��� �Ѿ�� �簢�� �Ѱ� ������ �ȼз� ��
[maxvertexcount(6)] //���⼺ x
void GS_DEAFULT(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vTexcoord = float2(0.f, 0.f);
        Out[i].vLifeTime = float2(0.f, 0.f);
        Out[i].LinearZ = float(0.f);
    }
	    
    float3 vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * In[0].vRectSize.x * 0.5f;

    matrix rotate = RotationMatrix(vLook, In[0].vRectSize.y);
    
    vLook = normalize(mul(vector(vLook, 0.f), rotate)).xyz;
    vRight = mul(vector(vRight, 0.f), rotate).xyz;
    vUp = mul(vector(vUp, 0.f), rotate).xyz;
    
    
    
    
    
    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f); //������ǥ
        //-1~1 x,y,z
    vector CamPos = mul(PointPosition, g_ViewMatrix); //����ǥ(��ġ)
    Out[0].LinearZ = CamPos.z; //����ȭ�� z ���� ������(0~1)
    
    float2 uvWeight = float2(1 / g_fUVCount.x, 1 / g_fUVCount.y);
    
    float SpriteIndex = lerp(0.f , (g_fUVCount.x* g_fUVCount.y)-1.f , (In[0].vLifeTime.y / In[0].vLifeTime.x));
    
    float2 uv = float2(float(floor(SpriteIndex) % g_fUVCount.x) / g_fUVCount.x, floor(SpriteIndex / g_fUVCount.x) / g_fUVCount.y);

    
    
    vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[0].vTexcoord = uv;
    Out[0].vAlphaTex = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[1].vTexcoord = float2(uv.x + uvWeight.x,uv.y);
    Out[1].vAlphaTex = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[2].vTexcoord = float2(uv.x + uvWeight.x, uv.y + uvWeight.y);
    Out[2].vAlphaTex = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[3].vTexcoord = float2(uv.x, uv.y + uvWeight.y);
    Out[3].vAlphaTex = float2(0.f, 1.f);
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

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vAlphaTex : TEXCOORD1;
    float2 vLifeTime : COLOR0;
    float LinearZ : COLOR1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vAlpha : SV_TARGET1;
};

//������Ʈ�� ������ �׸��� ������ �ȼ��̳� �Ȱ���
PS_OUT PS_MAIN_NOCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //������ǥ

    float fWeight = abs(PointPosition.z); //����ȭ�� z ���� ������(0~1)    
    
    float2 LifeAlpha = g_lifeAlpha;

    vector Tone = g_ToneTexture.Sample(PointSampler, In.vTexcoord);

    float FlowPow = 1.0f;//�ְ� ���� 0~1����
    float FlowSpeed = 1.0f; //�帧 �ӵ�
    
    //��������Ʈ �� �ð��̶� �ٸ�
    float TimeA = frac(In.vLifeTime.y * FlowSpeed);
    float TimeB = frac(In.vLifeTime.y + 0.5f * FlowSpeed);
    
    float2 ResultTime = float2(TimeA * 0.2f, TimeB * 0.3f);

    // Noise �ؽ�ó�� ����Ͽ� UV ��ǥ ��ȭ ���
    
    float2 Dist = (g_FluidTexture.Sample(PointSampler, In.vAlphaTex) * 2.0f - 1.0f)*FlowPow;
   // float2 flowUV = In.vAlphaTex + (g_FluidTexture.Sample(PointSampler, In.vAlphaTex + ResultTime).xy * 2.0 - 1.0) * FlowPow;


    float BaseAlphaA = g_BaseAlphaTexture.Sample(PointSampler, In.vAlphaTex + Dist * TimeA).a;
    float BaseAlphaB = g_BaseAlphaTexture.Sample(PointSampler, In.vAlphaTex + Dist * TimeB).a;

    float mixlerp = abs(frac(In.vLifeTime.y) * 2.f - 1.f);
    
    float BaseAlpha = lerp(BaseAlphaA, BaseAlphaB, mixlerp);//flow ���̴� �ݺ�
    
    vector UVSprite = g_UVAnimTexture.Sample(LinearSampler, In.vTexcoord);
    
    float Alphafactor = frac(In.vLifeTime.y / In.vLifeTime.x);
    
    float lerpAlpha = lerp(LifeAlpha.x, LifeAlpha.y, Alphafactor);
  
    vector FinalColor = vector(Tone.rgb, BaseAlpha);
    FinalColor.a *= lerpAlpha;

    // FinalColor = vector(flowUV, flowUV);

    float3 ColorN = FinalColor.rgb;
    
    float AlphaN = FinalColor.a;
        
  //  AlphaN *= lerp(g_lifeAlpha.x, g_lifeAlpha.y, In.vLifeTime.y / In.vLifeTime.x);
    
    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
        
    return Out;
}


technique11 DefaultTechnique
{
    pass Fluid //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_LOCAL();
        GeometryShader = compile gs_5_0 GS_DEAFULT();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOCOLOR();
    }

}

