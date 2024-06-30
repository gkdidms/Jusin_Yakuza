
#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;

matrix g_CamProjMatrix, g_CamViewMatrix;

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
Texture2D g_RMTexture;
Texture2D g_SpecularTexture;
Texture2D g_DepthTexture;
Texture2D g_MetallicTexture;
Texture2D g_BackBufferTexture;
Texture2D g_LightDepthTexture;
Texture2D g_ToneMappingTexture;
Texture2D g_LuminanceTexture;
Texture2D g_CopyLuminanceTexture;
Texture2D g_AmbientTexture;
//블러용
Texture2D g_EffectTexture;
//데칼용
Texture2D g_DecalTexture;

Texture2D g_GlassDiffuseTexture;
Texture2D g_GlassNormalTexture;
Texture2D g_GlassDepthTexture;
Texture2D g_GlassRMTexture;
Texture2D g_GlassMetallicTexture;

Texture2D g_BlurTexture;
Texture2D g_ResultTexture;
Texture2D g_AccumTexture;
Texture2D g_AccumAlpha;

float g_fOutlineAngle = 0.8f;
float g_fSpecularIntensity = 0.5f;


//HDR
float3 Luminance = float3(0.2125f, 0.7154f, 0.0721f);
float fDelta = { 0.0001f };
bool g_isFinished = { false };
float g_fLumVar;

//SSAO
bool g_isSSAO = { false };
float g_fRadiuse = { 0.003f };
float4 g_SSAORandoms[64];
Texture2D g_SSAONoisesTexture;

float g_fSSAOBise = { 0.025f };
const float2 g_NoiseScale = float2(1280.f / 4.f, 720.f / 4.f);
float3 g_Randoms[16] =
{
    float3(0.0363531820, 0.0195892137, 0.0910752937),
    float3(0.0271668006, -0.0817698613, 0.0511824936),
    float3(-0.0558262058, -0.0809410140, 0.0225508530),
    float3(-0.0607699566, 0.0768491626, 0.0282955095),
    float3(0.0684265420, -0.0224859975, 0.0743483230),
    float3(-0.0639691651, -0.0760242194, 0.0354552418),
    float3(0.0966500565, 0.0333450735, 0.0345177278),
    float3(0.0650642961, 0.0458409302, 0.0770356283),
    float3(0.0283417776, 0.0408770181, 0.102645352),
    float3(-0.0685298145, -0.0951056033, 0.0116156377),
    float3(-0.00153590832, -0.118244737, 0.0298856888),
    float3(-0.0690319762, 0.0818628073, 0.0675078705),
    float3(-0.0371511765, 0.0524620637, 0.114877217),
    float3(-0.104534455, -0.0193042718, 0.0866346955),
    float3(-0.0627479777, 0.0276224166, 0.125569463),
    float3(-0.0750860944, -0.122410446, 0.0413474962)
};

//PBR
float fGamma = { 2.2f };

//블룸(가우시안)
float g_fTexW = 1280.0f;
float g_fTexH = 720.0f;

static const float g_fWeight[13] =
{
    0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
	/*0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1, 
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561*/
};

float g_fTotal = 2.f;

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

struct PS_OUT_GAMEOBJECT
{
    vector vColor : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRM : SV_TARGET3;
    vector vMetallic : SV_Target4;
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
    vector vSpecular : SV_TARGET1;
    //vector vAmbient : SV_TARGET1;
};

float3x3 Get_TBN(float3 vNormal, float2 vTexcoord)
{   
    float3 vRandomVec = g_SSAONoisesTexture.Sample(PointSampler, vTexcoord * g_NoiseScale).xyz;
    vRandomVec = vRandomVec * 2.f - 1.f;
    matrix matWV = mul(g_WorldMatrix, g_CamViewMatrix);
    vRandomVec = normalize(mul(vector(vRandomVec, 0.f), matWV)).xyz;
    
    float3 tangent = normalize(vRandomVec - vNormal * dot(vRandomVec, vNormal));
    float3 bitangent = cross(vNormal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, vNormal);
    
    return TBN;
}

float4 SSAO(float3x3 TBN, float3 vPosition)
{
    float fOcclusion = 0.f;
    
    for (int i = 0; i < 64; ++i)
    {  
        float3 vSample = vPosition + mul(g_SSAORandoms[i % 64].xyz, TBN) * g_fRadiuse; // 뷰스페이스
       
        vector vOffset = vector(vSample, 1.f);
        vOffset = mul(vOffset, g_CamProjMatrix);
        vOffset.xyz /= vOffset.w;
        vOffset.xy = vOffset.xy * float2(0.5f, -0.5f) + float2(0.5f, -0.5f);
        
        vector vOccNorm = g_DepthTexture.Sample(LinearSampler, vOffset.xy);
        
        	/* 뷰스페이스 상의 위치를 구한다. */
        vector vOccPosition;
        vOccPosition.x = vOffset.x * 2.f - 1.f;
        vOccPosition.y = vOffset.y * -2.f + 1.f;
        vOccPosition.z = vOccNorm.x; /* 0 ~ 1 */
        vOccPosition.w = 1.f;
        
        vOccPosition = vOccPosition * (vOccNorm.y * g_fFar);
        
        vOccPosition = mul(vOccPosition, g_ProjMatrixInv);
        
        //float rangeCheck = smoothstep(0.0, 1.0, g_fRadiuse / abs(vPosition.z - vOccPosition.z));
        fOcclusion += (vOccPosition.z >= vSample.z + g_fSSAOBise ? 1.0 : 0.0);
    }
    
    float4 vAmbient = fOcclusion / 64.f;
    
    return vAmbient;
}

PS_OUT PS_MAIN_SSAO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    
    if (vNormalDesc.a != 0.f)
    {
        Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    }
    else
    {
        //뷰스페이스 위치로 옮기기
        vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
        vNormal = normalize(mul(vNormal, g_CamViewMatrix));
        
        //뷰행렬 상의 위치 구하기
        vector vPosition;
        
        vPosition.x = In.vTexcoord.x * 2.f - 1.f;
        vPosition.y = In.vTexcoord.y * -2.f + 1.f;
        vPosition.z = vDepthDesc.x; /* 0 ~ 1 */
        vPosition.w = 1.f;

        vPosition = vPosition * (vDepthDesc.y * g_fFar);
        vPosition = mul(vPosition, g_ProjMatrixInv);
        
        float4 fAmbient = SSAO(Get_TBN(vNormal.xyz, In.vTexcoord), vPosition.xyz);

        Out.vColor = 1.f - fAmbient;
    }
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float vAmbientDesc = g_AmbientTexture.Sample(LinearSampler, In.vTexcoord).r;
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    
    vector vAmbient = g_vLightAmbient * g_vMtrlAmbient;
    if (g_isSSAO)
        vAmbient *= vAmbientDesc;
    
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f) + vAmbient);
    
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

    vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
    vector vLook = vWorldPos - g_vCamPosition;
    
    vector vSpecularMap = g_RMTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 10.f) * (1.f - vSpecularMap.r);
    
    //if (g_isSSAO)
    //   Out.vShade *= vAmbientDesc;
    
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
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    float fMetallic = 0.0f; // 기본값
    fMetallic = g_MetallicTexture.Sample(LinearSampler, In.vTexcoord).g; // Metallic 값 샘플링
    
    // 기존
    Out.vColor = vDiffuse * vShade + vSpecular * g_fSpecularIntensity;
    
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

    vector vAccumColor = g_AccumTexture.Sample(PointSampler, In.vTexcoord);
    float vAccumAlpha = g_AccumAlpha.Sample(PointSampler, In.vTexcoord).r;
    
    float vResult = g_ResultTexture.Sample(PointSampler, In.vTexcoord).r;
    
      // 최종 출력 계산(알파*가중치)를 빼주는작업= 모두 함친 색이 나 옴
    //vector FinalColor = float4(vAccumColor.xyz / vAccumColor.a, (1-vAccumAlpha));
    vector FinalColor = float4(vAccumColor.xyz , (1-vAccumAlpha));

    //Out.vColor = vResult+FinalColor;
    Out.vColor = FinalColor;

    return Out;
}

PS_OUT_GAMEOBJECT PS_INCLUDE_GLASS(PS_IN In)
{

    PS_OUT_GAMEOBJECT Out = (PS_OUT_GAMEOBJECT) 0;

    vector vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassDiffuseColor = g_GlassDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vNonBlendNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassNormal = g_GlassNormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vNonBlendDepth = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassDepth = g_GlassDepthTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vNonBlendRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassRM = g_GlassRMTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vNonBlendMetallic = g_MetallicTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassMetallic = g_GlassMetallicTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    if (vNonBlendDepth.r < vGlassDepth.r)
    {
        // Nonblend
        Out.vColor = vDiffuseColor;
        Out.vNormal = vNonBlendNormal;
        Out.vDepth = vNonBlendDepth;
        Out.vRM = vNonBlendRM;
        Out.vMetallic = vNonBlendMetallic;
    }
    else
    {
        // Glass
        Out.vColor = vGlassDiffuseColor;
        Out.vNormal = vGlassNormal;
        Out.vDepth = vGlassDepth;
        Out.vRM = vGlassRM;
        Out.vMetallic = vGlassMetallic;
    }
    
    

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
        SetBlendState(BS_Blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_OIT_RESULT();   
    }
    
    pass IncludeGlassRender //16 - diffuse와 Glass 합치기
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_INCLUDE_GLASS();
    }
}

