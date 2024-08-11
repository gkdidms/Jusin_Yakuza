struct GS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    uint4 vBlendIndices : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHTS;
};

struct GS_OUT
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    uint4 vBlendIndices : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHTS;
};

cbuffer BoneBuffer : register(b0)
{
    matrix BoneMatrixArray[512];
};

StructuredBuffer<GS_IN> g_InputBuffer;
RWStructuredBuffer<GS_OUT> g_OutputBuffer;

[numthreads(16, 1, 1)]
void GS_MAIN(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint iIndex = dispatchThreadID.x; // ���� �ε���
    
    GS_IN In = g_InputBuffer[iIndex];
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    matrix TransformMatrix = mul(BoneMatrixArray[In.vBlendIndices.x], In.vBlendWeights.x) +
    mul(BoneMatrixArray[In.vBlendIndices.y], In.vBlendWeights.y) +
    mul(BoneMatrixArray[In.vBlendIndices.z], In.vBlendWeights.z) +
    mul(BoneMatrixArray[In.vBlendIndices.w], fWeightW);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), TransformMatrix);
    
    g_OutputBuffer[iIndex].vPosition = vPosition.xyz;
    g_OutputBuffer[iIndex].vNormal = vNormal.xyz;
    g_OutputBuffer[iIndex].vTangent = In.vTangent;
    g_OutputBuffer[iIndex].vTexcoord = In.vTexcoord;
    g_OutputBuffer[iIndex].vBlendIndices = In.vBlendIndices;
    g_OutputBuffer[iIndex].vBlendWeights = In.vBlendWeights;
    
}