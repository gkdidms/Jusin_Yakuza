


cbuffer PBR_Buffer : register(b0)
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjMatrix;
    matrix ViewMatrixInv;
    matrix ProjMatrixInv;
    matrix CamViewMatrix;
    matrix CamProjMatrix;
    
    float4 vCamPosition;
    float fFar;
    float fPadding[3];
};

cbuffer Light_Buffer : register(b1)
{
    vector vLightDirection;

    vector vLightDiffuse;
    vector vLightAmbient;
    vector vLightSpecular;
};

Texture2D g_DepthTexture : register(t0);
Texture2D g_NormalTexture : register(t1);
Texture2D g_SurfaceTexture : register(t2);
Texture2D g_DiffuseTexture : register(t3);

RWTexture2D<float4> g_OutputTexture : register(u0);

struct MRM_OUT
{
    float fRed;
    float fBlue;
};

struct COMBINE_OUT
{
    float fMetalness;
    float fRoughness;
};

float3 GammaToLinear(float3 color, float fGamma)
{
    return pow(color, fGamma);
}

float3 LinearToGamma(float3 color, float fGamma)
{
    return pow(color, 1.0f / fGamma);
}

//F
float3 FresnelSchlick(float cosTheta, float3 R0)
{
    return R0 + (1.f - R0) * pow(1.f - cosTheta, 5.f);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

//D
float NormalDistributionGGXTR(float3 vNormal, float3 h, float a, float PI)
{
    float a2 = a * a;
    float NdotH = saturate(dot(vNormal, h));
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = NdotH2 * (a2 - 1.f) + 1.f;
    denom = PI * denom * denom;
    
    return nom / denom;
}

float3 SchlickApprox(float3 F0, float3 L, float3 H)
{
    float dotLH = saturate(dot(L, H));
    return F0 + (1.0f - F0) * pow(1.0f - dotLH, 5.0f);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = 3.14159265359f * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float num = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float epsilon = 1e-6f;

float4 BRDF(float4 vPosition, int2 vTexcoord, float4 vNormal, float4 vDepthDesc, float3 vLook)
{
    float PI = { 3.14f };
    float fGamma = { 2.2f };
    
    float3 vAlbedo = g_DiffuseTexture.Load(int3(vTexcoord, 0)).xyz;
    vector Combine = g_SurfaceTexture.Load(int3(vTexcoord, 0));
    
    /* Li */
    //float vDistance = length(normalize(vCamPosition - vPosition));
    //float vAttenuation = 1.f / (vDistance * vDistance);
    //float3 vLi = vLightDiffuse * vAttenuation;
    
    float3 F0 = 0.4;
    float fMetalic = Combine.r;
    //F0 = lerp(F0, vAlbedo, fMetalic);
    
    //vector vLightDir = reflect(normalize(g_vLightDir), vNormal); //g_vLightDir * -1.f;
    float3 vLightDir = reflect(normalize(vLightDirection), vNormal).xyz;
    float3 vHalfway = normalize(vLook + vLightDir);
    float3 vRadiance = vLightDiffuse;
    
    //BRDF
    float vRoughness = Combine.g;
    float D = NormalDistributionGGXTR(vNormal.xyz, vHalfway, vRoughness, PI); //r : Roughness
    float G = GeometrySmith(vNormal.xyz, vLook, vLightDir, vRoughness);
    float3 F = SchlickApprox(F0, vLightDir, vHalfway);
    
    float3 nominator = D * G * F;
    
    float WoDotN = max(dot(vLook, vNormal.xyz), 0.f);
    float WiDotN = saturate(dot(vLightDir, vNormal.xyz));
    float denominator = (WoDotN * WiDotN * 4.f) + 0.001f;
    
    float3 vSpecular = (nominator / (denominator)) * Combine.z;
    
    float3 vKS = F;
    float3 vKD = 1.f - vKS;
    vKD *= 1.f - fMetalic;
    
    float3 vDiffuse = vKD * vAlbedo / PI;
    vDiffuse = (vDiffuse + vSpecular) * vRadiance;
    
    return vector(vDiffuse, 1.f);
}

[numthreads(16, 16, 1)]
void GS_MAIN(uint3 id : SV_DispatchThreadID)
{
    int2 vUV = id.xy;
    int2 vTexcoord = vUV / int2(1280, 720);
    
    vector vNormal = g_NormalTexture.Load(int3(vUV, 0));
    vNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);
    vector vDepthDesc = g_DepthTexture.Load(int3(vUV, 0));
    vector vWorldPos;

    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * fFar);

	/* 뷰스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, ViewMatrixInv);

    //vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
    float3 vLook = normalize(vCamPosition - vWorldPos).xyz;
    
    vector vResult = BRDF(vWorldPos, vUV, normalize(vNormal), vDepthDesc, vLook);
    g_OutputTexture[vUV] = vResult;

}