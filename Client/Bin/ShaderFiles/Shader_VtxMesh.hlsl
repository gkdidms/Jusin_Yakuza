
#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_WorldMatrixInv, g_ViewMatrixInv, g_ProjMatrixInv;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_RMTexture;
Texture2D g_RefractionTexture;
Texture2D g_RSTexture;
Texture2D g_DepthTexture;
Texture2D g_NoiseTexture;

float g_fObjID;
float g_fRefractionScale = { 0.001f };

float g_fFar = { 3000.f };
float g_fTimeDelta;
float g_fSpeed = 2.f;
bool g_bExistNormalTex;
bool g_bExistRMTex;
bool g_bExistRSTex;

vector g_vCamPosition;

float2 g_RenderResolution = float2(1280, 720);

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
    Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);


    return Out;
}

/* TriangleList인 경우 : 정점 세개를 받아서 w나누기를 각 정점에 대해서 수행한다. */
/* 뷰포트(윈도우좌표로) 변환. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를, 정점을 선형보간하여 만든다. -> 픽셀이 만들어졌다!!!!!!!!!!!! */


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
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
	
    // 투명할 경우(0.1보다 작으면 투명하니) 그리지 않음
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    float3 vNormal;
    if (true == g_bExistNormalTex)
    {
        // 매핑되는 texture가 있을때
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
        vNormal = mul(vNormal.xyz, WorldMatrix);
    }
    else
    {
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        // 텍스처 없을때
        vNormal = mul(In.vNormal.xyz, WorldMatrix);
    }
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    
    // specularTex와 metalic 같은 rm 사용 - bool 값 같이 사용하기
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
        // Normal texture 있으면 vTexcoord 다시
        float3 normal;
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
        normal = vNormalDesc.xyz * 2.f - 1.f;
        vRefractTexCoord = vRefractTexCoord + (normal.xy * g_fRefractionScale);
    }

    // Refract - 유리 뒤에 비치는 씬
    float4 vRefractColor = g_RefractionTexture.Sample(LinearSampler, vRefractTexCoord);
    float4 vGlassTexColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vFinalColor = lerp(vRefractColor, vGlassTexColor, 0.5f);
    
    Out.vDiffuse = vFinalColor;
    
	
    // 투명할 경우(0.1보다 작으면 투명하니) 그리지 않음
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    float3 vNormal;
    if (true == g_bExistNormalTex)
    {
        // 매핑되는 texture가 있을때
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
        vNormal = mul(vNormal.xyz, WorldMatrix);
    }
    else
    {
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        // 텍스처 없을때
        vNormal = mul(In.vNormal.xyz, WorldMatrix);
    }
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    
    // specularTex와 metalic 같은 rm 사용 - bool 값 같이 사용하기
    if (true == g_bExistRMTex)
    {
        Out.vRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    }
    
    
    return Out;
}



PS_OUT PS_PUDDLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 물 웅덩이 모양 잡기
    vector vMaskTexture = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if(0.3 > vMaskTexture.r)
        discard;
    
    float2 vRefractTexCoord;
    vRefractTexCoord.x = In.vProjPos.x / In.vProjPos.w / 2.0f + 0.5f;
    vRefractTexCoord.y = -In.vProjPos.y / In.vProjPos.w / 2.0f + 0.5f;
    
    // 물 움직이는거
    float2 vTexUV = float2(In.vTexcoord.x + g_fTimeDelta * g_fSpeed, In.vTexcoord.y);
    vector normal = g_NormalTexture.Sample(LinearSampler, vTexUV);
    // 물 표현(물 밑 투영)
    vector vRefractionColor = g_RefractionTexture.Sample(LinearSampler, vRefractTexCoord + 0.02 * normal.xy);

    float2 vScreenPos = In.vPosition.xy / g_RenderResolution;
    float4 vViewDepth = g_DepthTexture.Sample(LinearSampler, vScreenPos);
    
    vector vWorldPos;
    vWorldPos.x = vScreenPos.x * 2.f - 1.f;
    vWorldPos.y = vScreenPos.y * -2.f + 1.f;
    vWorldPos.z = vViewDepth.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * (vViewDepth.y * 3000.f);
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv); // world
    
    float3 viewDir = normalize(vWorldPos.xyz - g_vCamPosition.xyz);
    // 물 법선
    float3 vNormalDir = g_NormalTexture.Sample(LinearSampler, vTexUV);
    
    
    float3 reflectedDir = reflect(viewDir, vNormalDir);
    
    float4 vReflectPos = vWorldPos + float4(reflectedDir.xyz, 0);
    
    
    float bReflect = true;
    

    // 반사 강도 조절
    float reflectAngle = dot(normalize(reflectedDir), normalize(vNormalDir));
    
    float4 vReflectScreenPos = mul(vReflectPos, g_ViewMatrix);
    vReflectScreenPos = mul(vReflectScreenPos, g_ProjMatrix);
    vReflectScreenPos /= vReflectScreenPos.w;
    vReflectScreenPos.xy = vReflectScreenPos.xy * 0.5 + 0.5;
    
    vector vDepth = g_DepthTexture.Sample(LinearSampler, vReflectScreenPos.xy);
    
    vector vCalculateWorld =0;
    vCalculateWorld.x = vReflectScreenPos.x * 2.f - 1.f;
    vCalculateWorld.y = vCalculateWorld.y * -2.f + 1.f;
    vCalculateWorld.z = vDepth.x; /* 0 ~ 1 */
    vCalculateWorld.w = 1.f;
    
    vCalculateWorld = vCalculateWorld * (vDepth.y * 3000.f);
    
    vCalculateWorld = mul(vCalculateWorld, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
    vCalculateWorld = mul(vCalculateWorld, g_ViewMatrixInv);
    
    float fDistance = length(g_vCamPosition.xyz - vCalculateWorld.xyz);
    if (fDistance > 10 || vCalculateWorld.y <= vWorldPos.y)
    {
        bReflect = false;
    }

    if(true == bReflect)
    {
        
        vector reflectColor = g_RefractionTexture.Sample(PointSampler, vReflectScreenPos.xy + 0.02 * normal.xy);
    
        Out.vDiffuse = reflectColor;
    }
    else
    {
        Out.vDiffuse = vRefractionColor;
    }
    

	
    // 투명할 경우(0.1보다 작으면 투명하니) 그리지 않음
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    float3 vNormal;
    if (true == g_bExistNormalTex)
    {
        // 매핑되는 texture가 있을때
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, vTexUV);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
        vNormal = mul(vNormal.xyz, WorldMatrix);
    }
    else
    {
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        // 텍스처 없을때
        vNormal = mul(In.vNormal.xyz, WorldMatrix);
    }
    
    // 반사에서 제외
    //Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vNormal = 0;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    
    // specularTex와 metalic 같은 rm 사용 - bool 값 같이 사용하기
    if (true == g_bExistRMTex)
    {
        Out.vRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    }
    
    
    return Out;
}


technique11 DefaultTechnique
{
	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass GlassDoorPass
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_GLASSDOOR();
    }


    pass PuddlePass
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_PUDDLE();
    }
}

