uint2 g_WinSize = { 1280, 720 };

static const float g_fWeight[13] =
{
    0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
	/*0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1, 
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561*/
};

float g_fTotal = 2.f;

groupshared float4 gCache[272];

Texture2D<float4> InputTexture : register(t0);
RWTexture2D<float4> OutputTexture : register(u0);

[numthreads(256, 1, 1)]
void GS_MAIN(uint3 dispatchThreadID : SV_DispatchThreadID, int3 GroupIndex : SV_GroupThreadID)
{
    uint2 outputCoord = dispatchThreadID.xy;
    
    float width, height;
    InputTexture.GetDimensions(width, height);
    int2 ImageSize = int2(width, height);
    
    gCache[GroupIndex.x + 6] = InputTexture.Load(int3(outputCoord, 0));
    
    if (GroupIndex.x < 6)
    {
        int x = max(dispatchThreadID.x - 6, 0);
        outputCoord.x = (x / ImageSize.x);
        gCache[GroupIndex.x] = InputTexture.Load(int3(outputCoord, 0));
    }
    if (GroupIndex.x >= 251)
    {
        int x = min(dispatchThreadID.x + 6, ImageSize.x);
        outputCoord.x = (x / ImageSize.x);
        gCache[GroupIndex.x + 10] = InputTexture.Load(int3(outputCoord, 0));
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    float4 color = float4(0, 0, 0, 0);
    for (int i = -6; i < 7; ++i)
    {
        int offsetCoord = GroupIndex.x + i + 6;
        
        color += g_fWeight[6 + i] * gCache[offsetCoord];
    }
    
    color /= g_fTotal;
    
    OutputTexture[outputCoord] = color;
}