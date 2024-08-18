/*
float g_fRadiuse = { 0.003f };
float4 g_SSAORandoms[64];
Texture2D g_SSAONoisesTexture;

float g_fSSAOBise = { 0.025f };
const float2 g_NoiseScale = float2(1280.f / 4.f, 720.f / 4.f);

float g_fFar = { 3000.f };

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

cbuffer MatrixBuffer : register(b0)
{
    matrix g_WorldMatrix;
    matrix g_ProjMatrixInv;
    matrix g_CamViewMatrix;
    matrix g_CamProjMatrix;
};

Texture2D<float4> DepthTexture : register(t0);
Texture2D<float4> NormalTexture : register(t1);
Texture2D<float4> SSAONoisesTexture : register(t2);

RWTexture2D<float4> OutputTexture : register(u0);

float3x3 Get_TBN(float3 vNormal, float2 vTexcoord)
{
    float3 vRandomVec = SSAONoisesTexture.Load(int3(vTexcoord * g_NoiseScale), 0).xyz;
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
        float3 vSample = vPosition + mul(g_SSAORandoms[i % 64].xyz, TBN) * g_fRadiuse; // ºä½ºÆäÀÌ½º
       
        vector vOffset = vector(vSample, 1.f);
        vOffset = mul(vOffset, g_CamProjMatrix);
        vOffset.xyz /= vOffset.w;
        vOffset.xy = vOffset.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
        
        vector vOccNorm = DepthTexture.Sample(LinearSampler, vOffset.xy);
        
        vector vOccPosition;
        vOccPosition.x = vOffset.x * 2.f - 1.f;
        vOccPosition.y = vOffset.y * -2.f + 1.f;
        vOccPosition.z = vOccNorm.x;
        vOccPosition.w = 1.f;
        
        vOccPosition = vOccPosition * (vOccNorm.y * g_fFar);
        
        vOccPosition = mul(vOccPosition, g_ProjMatrixInv);
        
        //float rangeCheck = smoothstep(0.0, 1.0, g_fRadiuse / abs(vPosition.z - vOccPosition.z));
        fOcclusion += (vOccPosition.z >= vSample.z + g_fSSAOBise ? 1.0 : 0.0);
    }
    
    float4 vAmbient = fOcclusion / 64.f;
    
    return vAmbient;
}

[numthreads(16, 16, 1)]
void GS_MAIN(uint3 dispatchThreadID : SV_DispatchThreadID, int3 GroupIndex : SV_GroupThreadID)
{

}
*/