
#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;
vector g_vCamDir;
float g_fFar;

Texture2D g_Texture;
Texture2D g_PriorityTexture;
Texture2D g_NormalTexture;
Texture2D g_DiffuseTexture;
Texture2D g_ShadeTexture;
Texture2D g_DepthTexture;
Texture2D g_BackBufferTexture;
Texture2D g_LightDepthTexture;
Texture2D g_ToneMappingTexture;
Texture2D g_LuminanceTexture;
Texture2D g_CopyLuminanceTexture;
Texture2D g_AmbientTexture;
//블러용
Texture2D g_EffectTexture;

texture2D g_BlurTexture;
texture2D g_ResultTexture;
texture2D g_AccumTexture;
texture2D g_AccumAlpha;

float g_fOutlineAngle = 0.8f;


//HDR
float3 Luminance = float3(0.2125f, 0.7154f, 0.0721f);
float fDelta = { 0.0001f };
bool g_isFinished = { false };
float g_fLumVar;

//SSAO
bool g_isSSAO = { false };
float g_fRadiuse = { 0.003f };
float3 g_vRandom[16] =
{
    float3(0.2024537f, 0.841204f, -0.9060141f),
    float3(-0.2200423f, 0.6282339f, -0.8275437f),
    float3(0.3677659f, 0.1086345f, -0.4466777f),
    float3(0.8775856f, 0.4617546f, -0.6427765f),
    float3(0.7867433f, -0.141479f, -0.1567597f),
    float3(0.4839356f, -0.8253108f, -0.1563844f),
    float3(0.4401554f, -0.4228428f, -0.3300118f),
    float3(0.0019193f, -0.8048455f, 0.0726584f),
    float3(-0.7578573f, -0.5583301f, 0.2347527f),
    float3(-0.4540417f, -0.252365f, 0.0694318f),
    float3(-0.0483353f, -0.2527294f, 0.5924745f),
    float3(-0.4192392f, 0.2084218f, -0.3672943f),
    float3(-0.8433938f, 0.1451271f, 0.2202872f),
    float3(-0.4037157f, -0.8263387f, 0.4698132f),
    float3(-0.6657394f, 0.6298575f, 0.6342437f),
    float3(-0.0001783f, 0.2834622f, 0.8343929f),
};

//블룸(가우시안)
float g_fTexW = 1280.0f;
float g_fTexH = 720.0f;

static const float g_fWeight[13] =
{
    0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
	/*0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1, 
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561*/
};

static const float g_fTotal = 2.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

/* 정점 셰이더 :  /*
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

/* TriangleList인 경우 : 정점 세개를 받아서 w나누기를 각 정점에 대해서 수행한다. */
/* 뷰포트(윈도우좌표로) 변환. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를, 정점을 선형보간하여 만든다. -> 픽셀이 만들어졌다!!!!!!!!!!!! */
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vAmbient : SV_TARGET1;
};

float4 SSAO(float2 vTexcoord, float fDepth, float3 vNormal, float fViewZ)
{
    float fOcclusion = 0.f;
    
    for (int i = 0; i < 64; ++i)
    {
        float3 vRay = cross(float3(0.f, 0.f, -1.f), g_vRandom[i % 16]); // 랜덤 방향 벡터 생성
        float3 vReflect = normalize(reflect(vRay, vNormal)) * g_fRadiuse;
        vReflect.x *= -1.f;
        float2 vRandowUV = vTexcoord + vReflect.xy;
        
        float fOccNorm = g_DepthTexture.Sample(LinearSampler, vRandowUV).x * g_fFar * fViewZ;
        
        if (fOccNorm <= fDepth + 0.0003f)
            ++fOcclusion;
    }
    
    float4 vAmbient = abs((fOcclusion / 64.f) - 1);
    
    return vAmbient;
}

PS_OUT PS_MAIN_SSAO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    
    float fViewZ = vDepthDesc.y * g_fFar;
    float fDepth = vDepthDesc.x * g_fFar * fViewZ;
    float4 fAmbient = SSAO(In.vTexcoord, fDepth, vNormal.xyz, fViewZ);
    
    Out.vColor = 1 - fAmbient;
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vAmbientDesc = g_AmbientTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * 
    (g_isSSAO ? vAmbientDesc : g_vMtrlAmbient)));
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);

	/* 뷰스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);

    float fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

    Out.vShade = g_vLightDiffuse *
		saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
    
    return Out;
}

PS_OUT PS_MAIN_COPY_BACKBUFFER_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;

    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse * vShade;
    
    return Out;
}

PS_OUT PS_MAIN_DEFERRED_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord);
   
    Out.vColor = vDiffuse;

    return Out;
}

PS_OUT PS_MAIN_LUMINANCE_SUM(PS_IN In) // 휘도 맵 구하기 (처음 시작)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord);
    
    float vResult = log(dot(vDiffuse.xyz, Luminance) + fDelta);
    
    Out.vColor = vResult;
    
    return Out;
}

PS_OUT PS_MAIN_LUMINANCE_SUM_LOOP(PS_IN In) // 휘도 맵 구하기 (루프)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
  
    Out.vColor = vDiffuse;
        
    return Out;
}

PS_OUT PS_MAIN_LUMINANCE_SUM_FINAL(PS_IN In) // 휘도 맵 구하기 (1x1)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    //float vColor = log(dot(vDiffuse.xyz, Luminance) + 0.0001f);
  
    Out.vColor = exp(vDiffuse / 16);
    
    return Out;
}

PS_OUT PS_MAIN_LUMINANCE(PS_IN In) // 현재 프레임의 평균 휘도 맵 구하기 (최종)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector fNew = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    vector fOld = g_CopyLuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vDiffuse = g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord);
    //현재 평균 휘도
    //float fAvgLum = fOld + (fNew - fOld) * (1.f - pow(0.98f, 0.5f));
    //float fAvgLum = fOld.x * 0.5f + fNew.x * 0.5f;
    
    float fSceneLumi = (fOld * 0.5 + fNew * 0.5);
    float fMiddleGrayOfSceneLumi = 1.03 - (2 / (2 + log10(fSceneLumi + 1)));
    float fRgbLumi = dot(vDiffuse.xyz, Luminance) + 0.0001f;

    float fLumiScaled = (fRgbLumi * fMiddleGrayOfSceneLumi) / (fSceneLumi + 0.0001f);
    
    Out.vColor = vector(fLumiScaled, fLumiScaled, fLumiScaled, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_COPYLUMINANCE(PS_IN In) // 이전 휘도를 저장
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector fNew = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = fNew;
    
    return Out;
}


PS_OUT PS_MAIN_TONEMAPPING(PS_IN In) // 감마 콜렉션 & ACES 톤매핑
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vLuminance = g_LuminanceTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vDiffuse = pow(g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord).xyz, 2.2f);
    float A = 2.51f, B = 0.03f, C = 2.43f, D = 0.59f, E = 0.14f;
    vDiffuse = saturate(vDiffuse * (A * vDiffuse + B)) / (vDiffuse * (C * vDiffuse + D) + E);

    Out.vColor = vector(pow(vDiffuse, 1.f / 2.2f), 1.f) * vLuminance * g_fLumVar;
    
    return Out;
}

float4 Blur_X(float2 vTexCoord)
{
    float4 vOut = (float4) 0;

    float2 vUV = (float2) 0;

    for (int i = -6; i < 7; ++i)
    {
        vUV = vTexCoord + float2(1.f / g_fTexW * i, 0);
        vOut += g_fWeight[6 + i] * g_EffectTexture.Sample(ClampSampler, vUV);
    }

    vOut /= g_fTotal;

    return vOut;
}

float4 Blur_Y(float2 vTexCoord)
{
    float4 vOut = (float4) 0;

    float2 vUV = (float2) 0;

    for (int i = -6; i < 7; ++i)
    {
        vUV = vTexCoord + float2(0, 1.f / g_fTexH * i);
        vOut += g_fWeight[6 + i] * g_EffectTexture.Sample(ClampSampler, vUV);
    }

    vOut /= g_fTotal;

    return vOut;
}

PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = Blur_X(In.vTexcoord);
	
    return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = Blur_Y(In.vTexcoord);

    return Out;
}

PS_OUT PS_MAIN_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vResult = g_ResultTexture.Sample(LinearSampler, In.vTexcoord);   

    vector vBlur = g_BlurTexture.Sample(LinearSampler, In.vTexcoord);   
    
    vector vEffect = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vResult + vBlur + vEffect;
    
    return Out;
}


PS_OUT PS_OIT_RESULT(PS_IN In)
{

    PS_OUT Out = (PS_OUT) 0;

    vector vAccumColor = g_AccumTexture.Sample(LinearSampler, In.vTexcoord);
    vector vAccumAlpha = g_AccumAlpha.Sample(PointSampler, In.vTexcoord);
   float vAccumWeight = vAccumAlpha.r;
    
      // 최종 출력 계산
    float4 FinalColor = float4(vAccumColor.xyz / clamp(vAccumColor.a, 1e-3, 3e3), vAccumWeight);
    
    // 투명 객체와 백그라운드 객체 합성
    
    vector vBackground = g_ResultTexture.Sample(PointSampler, In.vTexcoord);


    Out.vColor = vBackground + FinalColor * (1 - vAccumWeight);
    //Out.vColor = vBackground + FinalColor;
    
    return Out;
}

technique11 DefaultTechnique
{

	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
    pass DebugRender // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass Copy_BackBuffer // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COPY_BACKBUFFER_RESULT();
    }

    pass Deferred_Result // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_RESULT();
    }

    pass Luminance // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE_SUM();
    }

    pass Luminance_Roof // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE_SUM_LOOP();
    }

    pass Luminance_Final // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE();
    }

    pass CopyLuminance // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COPYLUMINANCE();
    }

    pass ToneMapping // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TONEMAPPING();
    }

    pass Luminance_1x1 // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE_SUM_FINAL();
    }

    pass Blur_X //11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass Blur_Y //12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    pass Result //13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RESULT();
    }


    pass SSAO //14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSAO();

    }
    pass OITRender //15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_OIT_RESULT();   
    }


}

