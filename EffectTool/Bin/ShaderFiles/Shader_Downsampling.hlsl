
Texture2D<float4> InputTexture : register(t0); // t : ��ǲ ���۷� ���ڴ�. 0 ~ 9 ���� (�����Ǵ� �ѹ���?)
RWTexture2D<float4> OutputTexture : register(u0); // u : �ƿ�ǲ ����, 0 ~ 9 

[numthreads(16, 16, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 vTexSize;
    InputTexture.GetDimensions(vTexSize.x, vTexSize.y);
    
    uint2 outputCoord = dispatchThreadID.xy; // �ȼ��� ������ȣ 

    float4 colorSum = float4(0, 0, 0, 0);

    // 2x2 ������ �ȼ��� ���ȭ�Ͽ� �ٿ���ø� 
    for (int x = 0; x < 2; ++x)
    {
        for (int y = 0; y < 2; ++y)
        {
            uint2 vOffset = outputCoord * 2 + uint2(x, y);
            if (vOffset.x < vTexSize.x && vOffset.y < vTexSize.y)
            {
                colorSum += InputTexture.Load(int3(vOffset, 0));

            }
        }
    }

    // ��հ� ����
    OutputTexture[outputCoord] = colorSum / 4.0; 
}