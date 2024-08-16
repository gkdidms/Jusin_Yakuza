
Texture2D<float4> InputTexture : register(t0); // t : ��ǲ ���۷� ���ڴ�. 0 ~ 9 ���� (�����Ǵ� �ѹ���?)
RWTexture2D<float4> OutputTexture : register(u0); // u : �ƿ�ǲ ����, 0 ~ 9 

[numthreads(16, 16, 1)]
void GS_MAIN(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 outputCoord = dispatchThreadID.xy; // �ȼ��� ������ȣ 

    float4 colorSum = float4(0, 0, 0, 0);

    // 2x2 ������ �ȼ��� ���ȭ�Ͽ� �ٿ���ø� 
    colorSum += InputTexture.Load(int3(outputCoord * 2 + uint2(0, 0), 0));
    colorSum += InputTexture.Load(int3(outputCoord * 2 + uint2(1, 0), 0));
    colorSum += InputTexture.Load(int3(outputCoord * 2 + uint2(0, 1), 0));
    colorSum += InputTexture.Load(int3(outputCoord * 2 + uint2(1, 1), 0));

    // ��հ� ����
    OutputTexture[outputCoord] = colorSum / 4.0; 
}