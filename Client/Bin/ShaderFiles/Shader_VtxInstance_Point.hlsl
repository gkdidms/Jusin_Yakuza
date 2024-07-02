#include "Engine_Shader_Defines.hlsli"

/* ������Ʈ ���̺�(������̺�) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;


Texture2D g_Texture;
Texture2D g_ResultTexture;
Texture2D g_AccumColor;
Texture2D g_AccumAlpha;

vector g_vStartColor, g_vEndColor;
vector g_vCamPosition;
float2 g_lifeAlpha;
float g_fRadian;
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
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);//�����̵�.

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    

    Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;//�����
    Out.vPSize = In.vPSize;
    Out.vDir = In.vDir;
    Out.vLifeTime = In.vLifeTime;
    Out.vRectSize = In.vRectSize;

    return Out;
}

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

    float2 vLifeTime : COLOR0;
    float LinearZ : COLOR1;
};

//��ƼŬ�� �� �ϳ��� �׸��� �ȼ��� �Ѿ�� �簢�� �Ѱ� ������ �ȼз� ��
[maxvertexcount(6)]//���⼺ x
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
	
    float3 vDirection = In[0].vDir.xyz;
    
    vector vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * In[0].vRectSize.x* 0.5f;

    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f);//������ǥ
        //-1~1 x,y,z
    vector CamPos = mul(PointPosition, g_ViewMatrix);//����ǥ(��ġ)
    Out[0].LinearZ = CamPos.z; //����ȭ�� z ���� ������(0~1)
    
        
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

[maxvertexcount(6)]//���⼺o �⺻ ȸ�� o ���� ȸ�����Ѱ��ִ� ������ ��� ���������ָ�ɵ�
void GS_DIRSCALE(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
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
    //���⿡ ȸ���� �ش�.
   
    vector vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);
    float3 vRight = normalize(cross(vDirection, vLook.xyz)) * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * In[0].vRectSize.x * 0.5f;

    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f); //������ǥ
        //-1~1 x,y,z
    vector CamPos = mul(PointPosition, g_ViewMatrix); //����ǥ(��ġ)
    Out[0].LinearZ = CamPos.z; //����ȭ�� z ���� ������(0~1)
    
        
    vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[1].vTexcoord = float2(0.f, 1.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[3].vTexcoord = float2(1.f, 0.f);
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
/* TriangleList�� ��� : ���� ������ �޾Ƽ� w�����⸦ �� ������ ���ؼ� �����Ѵ�. */
/* ����Ʈ(��������ǥ��) ��ȯ. */
/* �����Ͷ����� : �������� �ѷ��׿��� �ȼ��� ������, ������ ���������Ͽ� �����. -> �ȼ��� ���������!!!!!!!!!!!! */

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


[maxvertexcount(6)] //���⼺o �⺻ ȸ�� o ���� ȸ�����Ѱ��ִ� ������ ��� ���������ָ�ɵ�
void GS_ROTSCALE(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
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

    float3 vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);
    float3 vRight = (cross(vDirection, vLook.xyz));
    float3 vUp = (cross(vLook.xyz, vRight));

    
   float LookLength= length(vLook);
    
    matrix rotate = RotationMatrix(vLook,g_fRadian);
    
    vLook = normalize(mul(vector(vLook, 0.f), rotate)).xyz;
    vRight = normalize(mul(vector(vRight, 0.f), rotate)).xyz * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    vUp = normalize(mul(vector(vUp, 0.f), rotate)).xyz * In[0].vPSize.y * In[0].vRectSize.x * 0.5f;
    

  //  float3x3 wpos = g_WorldMatrix._11_12_13_21_41_42_43;

    float4 vPosition;
   // matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
   // matrix matWVP = mul(matWV, g_ProjMatrix);
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f); //������ǥ
        //-1~1 x,y,z
    vector CamPos = mul(PointPosition, g_ViewMatrix); //����ǥ(��ġ)
    Out[0].LinearZ = CamPos.z; //����ȭ�� z ���� ������(0~1)

    
    //���û� �簢�� �� �����.
    vPosition = float4((In[0].vPosition + vRight + vUp), 1.f);
    Out[0].vPosition = mul(vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    vPosition = float4((In[0].vPosition - vRight + vUp) , 1.f);
    Out[1].vPosition = mul(vPosition, matVP);
    Out[1].vTexcoord = float2(0.f, 1.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    vPosition = float4((In[0].vPosition - vRight - vUp) , 1.f);
    Out[2].vPosition = mul(vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    vPosition = float4((In[0].vPosition + vRight - vUp) , 1.f);
    Out[3].vPosition = mul(vPosition, matVP);
    Out[3].vTexcoord = float2(1.f, 0.f);
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

//������Ʈ�� ������ �׸��� ������ �ȼ��̳� �Ȱ���
PS_OUT PS_MAIN_NOCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //������ǥ

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float3 ColorN = vParticle.rgb;
    
    float AlphaN = vParticle.a;
        
    AlphaN *= lerp(g_lifeAlpha.x, g_lifeAlpha.y, In.vLifeTime.y / In.vLifeTime.x);
    
    float fWeight = abs(PointPosition.z); //����ȭ�� z ���� ������(0~1)    

    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
        
    return Out;
}

PS_OUT PS_MAIN_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //������ǥ

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);

    float4 LerpColor = lerp(g_vStartColor, g_vEndColor, In.vLifeTime.y / In.vLifeTime.x);
    
    vParticle *= LerpColor;
    
    float3 ColorN = vParticle.rgb;
    
    float AlphaN = vParticle.a;

    float fWeight = abs(PointPosition.z); //����ȭ�� z ���� ������(0~1)    

    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
        
    return Out;
}

PS_OUT PS_ROTANIM_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //������ǥ

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);

    float4 LerpColor = lerp(g_vStartColor, g_vEndColor, In.vLifeTime.y / In.vLifeTime.x);
    
    vParticle *= LerpColor;
    
    float3 ColorN = vParticle.rgb;
    
    float AlphaN = vParticle.a;

    float fWeight = abs(PointPosition.z); //����ȭ�� z ���� ������(0~1)    

    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
        
    return Out;

}

PS_OUT PS_ANIM_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //������ǥ

    float BaseAlpha = g_Texture.Sample(LinearSampler, In.vTexcoord).g;

    vector vParticle = vector(1.f, 1.f, 1.f, BaseAlpha);
    
    float4 LerpColor = lerp(g_vStartColor, g_vEndColor, In.vLifeTime.y / In.vLifeTime.x);
    
    vParticle.rgba *= LerpColor;
    
    float3 ColorN = vParticle.rgb;
    
    float AlphaN = vParticle.a;

    float fWeight = abs(PointPosition.z); //����ȭ�� z ���� ������(0~1)    

    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
        
    return Out;

}

technique11 DefaultTechnique
{
	
	/* Ư�� �������� ������ �� �����ؾ��� ���̴� ����� ��Ʈ���� ���̰� �ִ�. */
     pass NOCOLOR    //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_LOCAL();
        GeometryShader = compile gs_5_0 GS_DIRSCALE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOCOLOR();
    }

    pass COLOR //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_LOCAL();
        GeometryShader = compile gs_5_0 GS_DIRSCALE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }
    
    pass ROTATE//2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_LOCAL();
        GeometryShader = compile gs_5_0 GS_ROTSCALE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }

    pass ROTANIM//3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_LOCAL();
        GeometryShader = compile gs_5_0 GS_ROTSCALE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ANIM_COLOR();
    }
    pass ANIM //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_LOCAL();
        GeometryShader = compile gs_5_0 GS_DIRSCALE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ANIM_COLOR();

    }
}

