Texture2D<float> InputTexture : register(t0); // t : ��ǲ ���۷� ���, 0 ~ 9 ����
RWTexture2D<float> OutputTexture : register(u0); // u : �ƿ�ǲ ����, 0 ~ 9 

[numthreads(16, 16, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // Depth�� ������ �ϸ� �ȵ�
    
    uint2 outputCoord = dispatchThreadID.xy; // �ȼ��� ������ȣ 

    // �ʱⰪ�� ū ������ ���� (���̰��� ���� 0.0�� ī�޶� �����, 1.0�� �ָ� �ִ� ��)
    float minDepth = InputTexture.Load(int3(outputCoord * 2 + uint2(0, 0), 0)).x;

    // 2x2 ������ �ȼ� �� �ּ� Depth ���� ����
    minDepth = min(minDepth, InputTexture.Load(int3(outputCoord * 2 + uint2(0, 0), 0)).x);
    minDepth = min(minDepth, InputTexture.Load(int3(outputCoord * 2 + uint2(1, 0), 0)).x);
    minDepth = min(minDepth, InputTexture.Load(int3(outputCoord * 2 + uint2(0, 1), 0)).x);
    minDepth = min(minDepth, InputTexture.Load(int3(outputCoord * 2 + uint2(1, 1), 0)).x);

    // ���� �ּ� Depth ���� Output Texture�� ����
    OutputTexture[outputCoord] = minDepth;
}