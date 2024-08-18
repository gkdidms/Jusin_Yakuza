struct GS_IN
{
    float3 vPosition; // 12����Ʈ
    float padding1; // 4����Ʈ �е� (���� ����)

    float3 vTexcoord; // 12����Ʈ
    float padding2; // 4����Ʈ �е� (���� ����)

};


cbuffer ObjectData : register(b0)
{
    matrix worldViewProjMatrix; // ����-��-���� ���
    float g_fFar; // ī�޶��� Far plane �Ÿ�
};

StructuredBuffer<GS_IN> g_InputBuffer : register(t0); // GS_IN ������ ����ü �迭
Texture2D<float4> depthTexture : register(t1); // 3x2 Depth �ؽ�ó
RWStructuredBuffer<int> g_OutputBuffer : register(u0); // ���ü� �Ǵ� �� �ȼ� �� ���

// 8���� �����带 �� �׷�
[numthreads(8, 1, 1)]
void GS_MAIN(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint index = dispatchThreadID.x;

    // �ε����� ���� ũ�⺸�� ū ��� ������ ��ȯ
    if (index >= g_InputBuffer.Length)
    {
        return;
    }

    // ���� ������ ��������
    GS_IN cubeVertex = g_InputBuffer[index];
    float4 worldPos = float4(cubeVertex.vPosition, 1.0f);
    float4 projectedPos = mul(worldPos, worldViewProjMatrix);

    // NDC �������� ���� ���� ����
    float ndcDepth = projectedPos.z / projectedPos.w;

    // �ؽ�ó ��ǥ�迡���� ���� ��ġ ��� (3x2 �ؽ�ó�� ���� ���)
    uint2 texCoord = uint2(index % 3, index / 3);

    // �ؽ�ó ��ǥ�� ��ȿ���� Ȯ��
    if (texCoord.x >= 3 || texCoord.y >= 2)
    {
        g_OutputBuffer[index] = 0; // �߸��� ��ǥ������ ������ ����
        return;
    }

    // Depth Texture���� �ش� ��ǥ�� ���� ���� ��������
    float4 depthValue = depthTexture.Load(int3(texCoord, 0));

    // ���� ��ǥ�迡���� ���� �� ��
    // ����ȭ: Far Plane ����� �����ϰ� ������ �񱳷� ����
    float depthInTexture = depthValue.x; // NDC ���� ��
    bool isVisible = (ndcDepth <= depthInTexture);

    // ���ü� ����� int ������ ��� (1: ����, 0: ������)
    g_OutputBuffer[index] = isVisible ? 1 : 0;
    
    g_OutputBuffer[index] = 1;
}