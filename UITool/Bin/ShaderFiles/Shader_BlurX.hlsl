uint2 g_WinSize = { 1280, 720 };

static const float g_fWeight[13] =
{
    0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
	/*0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1, 
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561*/
};

float g_fTotal = 2.f;

Texture2D<float4> InputTexture : register(t0);
RWTexture2D<float4> OutputTexture : register(u0);

[numthreads(16, 16, 1)]
void GS_MAIN(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 outputCoord = dispatchThreadID.xy;

    float4 color = float4(0, 0, 0, 0);
    
    for (int i = -6; i < 7; ++i)
    {
        int2 offsetCoord = outputCoord + int2(i, 0);
        
        // 경계 클램핑 처리
        offsetCoord.x = clamp(offsetCoord.x, 0, int(g_WinSize.x - 1));
        offsetCoord.y = clamp(offsetCoord.y, 0, int(g_WinSize.y - 1));

        color += g_fWeight[6 + i] * InputTexture.Load(int3(offsetCoord, 0));
    }
    
    color /= g_fTotal;
    
    OutputTexture[outputCoord] = color;
}