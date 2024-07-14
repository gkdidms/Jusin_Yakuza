/* PBR */
#include "Shader_SSAO.hlsl"

float fGamma = { 2.2f };
float PI = { 3.14f };

float3 GammaToLinear(float3 color)
{
    return pow(color, fGamma);
}

float3 LinearToGamma(float3 color)
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
float NormalDistributionGGXTR(float3 vNormal, float3 h, float a)
{
    float a2 = a * a;
    float NdotH = saturate(dot(vNormal, h));
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = NdotH2 * (a2 - 1.f) + 1.f;
    denom = PI * denom * denom;
    
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

//G
float GeometrySmith(float3 n, float3 v, float3 l, float k)
{
    float NdotV = saturate(dot(n, v));
    float NdotL = saturate(dot(n, l));
    
    float ggx2 = GeometrySchlickGGX(NdotV, k);
    float ggx1 = GeometrySchlickGGX(NdotL, k);
    
    return ggx1 * ggx2;
}


float4 BRDF(float4 vPosition, float2 vTexcoord, float4 vNormal, float4 vDepthDesc)
{
    vector vWorldPos;

    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);

	/* 뷰스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    //vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
    float3 vLook = normalize(g_vCamPosition - vWorldPos).xyz;
    
    float3 vAlbedo = g_DiffuseTexture.Sample(LinearSampler, vTexcoord).xyz;
    //vAlbedo = GammaToLinear(vAlbedo);
    
    /* Li */
    float vDistance = length(normalize(g_vLightPos - vPosition));
    float vAttenuation = 1.f / (vDistance * vDistance);
    float3 vLi = g_vLightDiffuse * vAttenuation;

    float3 vRM = g_RMTexture.Sample(LinearSampler, vTexcoord).xyz; /* RM : Roughness, Metalic */
    float3 vRS = g_RSTexture.Sample(LinearSampler, vTexcoord).xyz; 
    float3 vMix = lerp(vRS, vRM, 0.5f);
    float3 F0 = 0.04f;
    
    float fMetalic = vMix.g;
    F0 = lerp(F0, vAlbedo, fMetalic); // vRM.g : Metalic
    
    vector vLightDir = normalize(vWorldPos - g_vLightPos)  * -1.f;
    float3 vHalfway = normalize(vLook + vLightDir);
    float3 vRadiance = g_vLightDiffuse;
    
    //BRDF
    float vRoughness = vMix.r;
    float D = NormalDistributionGGXTR(vNormal.xyz, vHalfway, vRoughness); //r : Roughness
    float G = GeometrySmith(vNormal.xyz, vLook, vLightDir, vRoughness);
    float3 F = FresnelSchlickRoughness(max(dot(vHalfway, vLook), 0.f), F0, vRoughness);
    
    float3 nominator = D * F * G;
    
    float WoDotN = saturate(dot(vLook, vNormal.xyz));
    float WiDotN = saturate(dot(vLightDir, vNormal.xyz));
    float denominator = (WoDotN * WiDotN * 4);
    
    float3 vSpecular = (nominator / (denominator));
    
    //  Energy Conservation
    float3 kS = F; //  reflection energy
    float3 kD = 1.0f - kS; //  refraction energy
    kD *= 1.0 - fMetalic; //  if metallic nullify kD
    
    //  Calculate radiance
    //vAlbedo = LinearToGamma(vAlbedo);
    //float3 vResult = (((vAlbedo/ PI) + vSpecular) * vLi);
    
    //vResult = LinearToGamma(vResult);
    

    return vector(vSpecular * vLi * WiDotN, 0.f);
}

float3 BRDF_MULTI(float4 vPosition, float2 vTexcoord, float4 vNormal, float4 vDepthDesc)
{
    vector vWorldPos;

    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);

	/* 뷰스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    //vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
    float3 vLook = normalize(g_vCamPosition - vWorldPos).xyz;
    
    float3 vAlbedo = g_DiffuseTexture.Sample(LinearSampler, vTexcoord).xyz;
    vAlbedo = GammaToLinear(vAlbedo);
    
    /* Li */
    float vDistance = length(normalize(g_vLightPos - vPosition));
    float vAttenuation = 1.f / (vDistance * vDistance);
    float3 vLi = g_vLightDiffuse * vAttenuation;

    float3 vMulti = g_MultiDiffuseTexture.Sample(LinearSampler, vTexcoord).xyz; /* RM : Roughness, Metalic */
    float3 F0 = 0.04f;
    
    float fMetalic = 1.f - vMulti.g;
    F0 = lerp(F0, vAlbedo, fMetalic); // vRM.r : Metalic
    
    float3 vLightDir = normalize(g_vLightDir) * -1.f;
    float3 vHalfway = normalize(vLook + vLightDir);
    float3 vRadiance = g_vLightDiffuse;
    
    //BRDF
    //float vRoughness = vMulti.r;
    //float D = NormalDistributionGGXTR(vNormal.xyz, vHalfway, vRoughness); //g : Roughness
    //float G = GeometrySmith(vNormal.xyz, vLook, vLightDir, vRoughness);
    float3 F = FresnelSchlick(max(dot(vHalfway, vLook), 0.f), F0);
    
    /*
    float3 nominator = D * G * F;
    
    float WoDotN = saturate(dot(vLook, vNormal.xyz));
    float WiDotN = saturate(dot(vLightDir, vNormal.xyz));
    float denominator = (4 * WoDotN * WiDotN);
    
    float3 vSpecular = (nominator / (denominator + 0.0001f));
    */
    //  Energy Conservation
    float3 kS = F; //  reflection energy
    float3 kD = 1.0f - kS; //  refraction energy
    kD *= 1.0 - fMetalic; //  if metallic nullify kD

    //Calculate radiance
    //float3 vResult = (kD * vAlbedo);
    //LinearToGamma(vResult);
    
    if (kD.r < 0.01f)
        kD = 1.f;
    
    return kD;
}
