cbuffer SSAOBuffer : register(b0)
{
    float4 SSAORandoms[64];
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjMatrix;
    matrix ViewMatrixInv;
    matrix ProjMatrixInv;
    matrix CamViewMatrix;
    matrix CamProjMatrix;
    
    float fFar;
    float fRadiuse;
    float fSSAOBise;
    float buffer1;
}

Texture2D g_SSAONoiseTexture : register(t0);
Texture2D g_DepthTexture : register(t1);
Texture2D g_NormalTexture : register(t2);

RWTexture2D<float4> g_OutputTexture : register(u0);

float3x3 Get_TBN(float3 vNormal, float2 vTexcoord)
{
    float2 g_NoiseScale = float2(1280.f / 4.f, 720.f / 4.f);
    float2 scaledTexcoord = vTexcoord * g_NoiseScale;
    scaledTexcoord = scaledTexcoord * float2(1280.f, 720.f);
    int2 wrappedTexcoord = scaledTexcoord % 4;
    
    float3 vRandomVec = g_SSAONoiseTexture.Load(int3(wrappedTexcoord, 0)).xyz;
    vRandomVec = vRandomVec * 2.f - 1.f;
    matrix matWV = mul(WorldMatrix, ViewMatrix);
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
        float3 vSample = vPosition + mul(SSAORandoms[i].xyz, TBN) * fRadiuse; // 뷰스페이스
       
        float4 vOffset = float4(vSample, 1.f);
        vOffset = mul(vOffset, CamProjMatrix);
        vOffset.xyz /= max(vOffset.w, 1e-6f);
        vOffset.xy *= float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
        vOffset.xy *= float2(1280.f, 720.f);
        float2 vFloatOffset = clamp(vOffset.xy, float2(0.f, 0.f), float2(1280.f, 720.f));
        
        int2 Offset = clamp(int2(vOffset.xy), int2(0, 0), int2(1280, 720));
        vector vOccNorm = g_DepthTexture.Load(int3(Offset, 0));
        
        /* 뷰스페이스 상의 위치를 구한다. */
        vector vOccPosition;
        vOccPosition.x = vFloatOffset.x * 2.f - 1.f;
        vOccPosition.y = vFloatOffset.y * -2.f + 1.f;
        vOccPosition.z = vOccNorm.x; /* 0 ~ 1 */
        vOccPosition.w = 1.f;
        
        vOccPosition *= (vOccNorm.y * fFar);
        
        vOccPosition = mul(vOccPosition, ProjMatrixInv);
        
        if (vOccPosition.z >= vSample.z + fSSAOBise)
            fOcclusion += 1.f;
    }
    
    float4 vAmbient = fOcclusion / 64.f;
    
    return vAmbient;
}

[numthreads(16, 16, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    float2 uv = id.xy;

    vector vNormalDesc = g_NormalTexture.Load(int3(uv, 0));
    vector vDepthDesc = g_DepthTexture.Load(int3(uv, 0));

    if (vNormalDesc.a != 0.f)
    {
        g_OutputTexture[uv] = float4(1.f, 1.f, 1.f, 1.f);
    }
    else
    {
        // 뷰스페이스 위치로 옮기기
        float3 vNormal = normalize(mul(vNormalDesc * 2.f - 1.f, ViewMatrix)).xyz;

        // 뷰 행렬 상의 위치 구하기
        vector vPosition;
        
        float2 vTexcoord = uv / float2(1280.f, 720.f);

        vPosition.x = vTexcoord.x * 2.f - 1.f;
        vPosition.y = vTexcoord.y * -2.f + 1.f;
        vPosition.z = vDepthDesc.x; /* 0 ~ 1 */
        vPosition.w = 1.f;

        vPosition *= (vDepthDesc.y * fFar);
        vPosition = mul(vPosition, ProjMatrixInv);

        float3x3 TBN = Get_TBN(vNormal, vTexcoord);
        float4 fAmbient = SSAO(TBN, vPosition.xyz);

        g_OutputTexture[uv] = 1.f - fAmbient;
    }
}