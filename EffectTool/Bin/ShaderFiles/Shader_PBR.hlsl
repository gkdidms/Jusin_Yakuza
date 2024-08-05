/* PBR */
#include "Shader_SSAO.hlsl"

float fGamma = { 2.2f };
float PI = { 3.14f };

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
    float vRoughness = vMulti.b;
    //float D = NormalDistributionGGXTR(vNormal.xyz, vHalfway, vRoughness); //g : Roughness
    //float G = GeometrySmith(vNormal.xyz, vLook, vLightDir, vRoughness);
    float3 F = FresnelSchlickRoughness(max(dot(vHalfway, vLook), 0.f), F0, vRoughness);
    
   // float3 nominator = D * G * F;
    
    //float WoDotN = saturate(dot(vLook, vNormal.xyz));
    //float WiDotN = saturate(dot(vLightDir, vNormal.xyz));
    //float denominator = (4 * WoDotN * WiDotN);
    
    //float3 vSpecular = (nominator / (denominator + 0.0001f));

    //  Energy Conservation
    float3 kS = F; //  reflection energy
    float3 kD = 1.0f - kS; //  refraction energy
    kD *= 1.0 - fMetalic; //  if metallic nullify kD

    //Calculate radiance
    //float3 vResult = (kD * vAlbedo);
    //LinearToGamma(vResult);
    
    
    
    kD = lerp(float3(1.f, 1.f, 1.f), kD, vMulti.r);
    
    return kD;
}


MRM_OUT Line124(MRM_OUT vMT)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vMT.fRed;
    float fBlue = vMT.fBlue;
    
    //line124
    fRed = (fRed * -1.f) + 1.f;
    fBlue = (fBlue * -1.f) + 1.f;
    
    fRed = fRed + fRed;
    fBlue = fBlue + fBlue;
    
    //line129
    fRed *= -1.f;
    fBlue *= -1.f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

// RM은 line125에서 받아옴
MRM_OUT Line129(MRM_OUT vMT, MRM_OUT vRM)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vMT.fRed;
    float fBlue = vMT.fBlue;
    
    //line129
    fRed *= -1.f;
    fBlue *= -1.f;
    
    fRed = (fRed * vRM.fRed) + 1.f;
    fBlue = (fBlue * vRM.fBlue) + 1.f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

MRM_OUT Line118(MRM_OUT vRM)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vRM.fRed;
    float fBlue = vRM.fBlue;
    
    //line 118
    fRed -= 0.5f;
    fBlue -= 0.5f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

MRM_OUT Line119(MRM_OUT vRM, float fMultiAlpha)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vRM.fRed;
    float fBlue = vRM.fBlue;
    
        //line 119
    fRed = (fRed * fMultiAlpha) + 0.5f;
    fBlue = (fBlue * fMultiAlpha) + 0.5f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}


MRM_OUT Line125(MRM_OUT vRM)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vRM.fRed;
    float fBlue = vRM.fBlue;
    
    fRed = (fRed * -1.f) + 1.f;
    fBlue = (fBlue * -1.f) + 1.f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

MRM_OUT Line126(MRM_OUT vRM, MRM_OUT vMT)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vRM.fRed;
    float fBlue = vRM.fBlue;
    
    fRed *= vMT.fRed;
    fBlue *= vMT.fBlue;
    
    fRed += fRed;
    fBlue += fBlue;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

//참,거짓 반환
MRM_OUT Line127(MRM_OUT vMT)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vMT.fRed;
    float fBlue = vMT.fBlue;
    
    fRed = vMT.fRed > 0.5f ? 1.f : 0.f;
    fBlue = vMT.fBlue > 0.5f ? 1.f : 0.f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

MRM_OUT Line130(MRM_OUT vMT, MRM_OUT vRM, MRM_OUT vFactor)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    Out.fRed = lerp(vMT.fRed, vRM.fRed, vFactor.fRed);
    Out.fBlue = lerp(vMT.fBlue, vRM.fBlue, vFactor.fBlue);
    
    return Out;
}


COMBINE_OUT CombineMTAndRM(vector vMT, vector vRM, float fMetelnessOverride, float fGlossinessOverride)
{
    COMBINE_OUT Out = (COMBINE_OUT) 0;
    
    MRM_OUT vMTDesc = (MRM_OUT) 0;
    vMTDesc.fRed = vMT.r;
    vMTDesc.fBlue = vMT.b;
    
    float fMTAlpha = vMT.a;
    
    MRM_OUT vRMDesc = (MRM_OUT) 0;
    vRMDesc.fRed = vRM.r;
    vRMDesc.fBlue = vRM.b;
    
    MRM_OUT vLine118Result = Line118(vRMDesc);
    MRM_OUT vLine119Result = Line119(vLine118Result, fMTAlpha);
    MRM_OUT vLine126Result = Line126(vLine119Result, vMTDesc);
    MRM_OUT vLine125Result = Line125(vLine119Result);
    MRM_OUT vLine127Result = Line127(vMTDesc);

    MRM_OUT vLine124Result = Line124(vMTDesc);
    MRM_OUT vLine129Result = Line129(vLine124Result, vLine125Result);
    
    MRM_OUT vLine130 = Line130(vLine129Result, vLine126Result, vLine127Result);
    
    vLine130.fBlue = min(vLine130.fBlue, 0.996f);
    vector vResult = vector(vLine130.fRed, 0.f, vLine130.fBlue, 1.f);
    vResult *= 0.5f;

    float glossOverride = max(fGlossinessOverride, 0.f);
    
    vector vMax1 = lerp(vResult.r, fMetelnessOverride, fMetelnessOverride);
    vector vMax2 = lerp(vResult.b, fGlossinessOverride, glossOverride);

    Out.fMetalness = vMax1;
    Out.fRoughness = vMax2;
    
    return Out;
}


float Neo_Glossiness(float3 vColor, vector vMulti)
{
    float vMix = lerp(vColor.y, vMulti.x, 1.f);
    float vMix2 = lerp(vColor.z, vMix, 0.5f);
    
    return vMix;
}


COMBINE_OUT Neo_MetallicAndGlossiness(float2 vTexcoord)
{
    COMBINE_OUT Out = (COMBINE_OUT) 0;
    vector vMulti = g_MultiDiffuseTexture.Sample(LinearSampler, vTexcoord);
    vector vRM = g_RMTexture.Sample(LinearSampler, vTexcoord);
    
    COMBINE_OUT MultiAndRM = CombineMTAndRM(vMulti, vRM, 0.f, 0.f);
    
    vector vRefl = vector(0.5f, 1.f, 0.5f, 1.f);
    vector vMultiAndRM = vector(MultiAndRM.fMetalness, 1.f, MultiAndRM.fRoughness, vMulti.a);
    
    COMBINE_OUT ReflCombine = CombineMTAndRM(vMultiAndRM, vRefl, 0.f, 0.f);
    float3 vResult = float3(ReflCombine.fMetalness, 1.f, ReflCombine.fRoughness);
    
    float3 vResultMix = lerp(vMulti.xyz, vResult, 1.f);
    float fRoughness = Neo_Glossiness(vResultMix, vMulti);
    
    float fMix = lerp(vResultMix.r, vMulti.y, 0.f);
    
    float value = fMix; // R 채널을 입력 값으로 사용
    float fromMin = 0.1f;
    float fromMax = 1.f;
    float toMin = 0.f;
    float toMax = 1.f;
    
    // 직접 매핑 계산
    float normalizedValue = (value - fromMin) / (fromMax - fromMin); // 0~1 범위로 변환
    float mappedValue = toMin + normalizedValue * (toMax - toMin); // 새로운 범위로 변환
    
    Out.fMetalness = mappedValue;
    Out.fRoughness = fRoughness;
    
    return Out;
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
    
    float3 F0 = 0.04f;
    
    COMBINE_OUT Combine = Neo_MetallicAndGlossiness(vTexcoord);
    
    float fMetalic = Combine.fMetalness;
    F0 = lerp(F0, vAlbedo, fMetalic); // vRM.g : Metalic
    
    vector vLightDir = reflect(normalize(g_vLightDir), vNormal); //g_vLightDir * -1.f;
    float3 vHalfway = normalize(vLook + vLightDir);
    float3 vRadiance = g_vLightDiffuse;
    
    //BRDF
    float vRoughness = Combine.fRoughness;
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
    

    return vector(vSpecular, 0.f);
}
