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
Texture2D<float4> depthTexture : register(t1); // 1280x720 Depth �ؽ�ó
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

    // ��ũ�� ���� ��ǥ�� ��ȯ
    float2 screenPos = (projectedPos.xy / projectedPos.w) * 0.5f + 0.5f; // NDC to [0,1] range
    screenPos *= float2(1280, 720); // [0,1] range to [0,1280] x [0,720]

    // �ؽ�ó ��ǥ�� ������ ��ȯ
    uint2 texCoord = uint2(screenPos);

    // �ؽ�ó ��ǥ�� ��ȿ���� Ȯ��
    if (texCoord.x >= 1280 || texCoord.y >= 720)
    {
        g_OutputBuffer[index] = 0; // �߸��� ��ǥ������ ������ ����
        return;
    }

    // Depth Texture���� �ش� ��ǥ�� ���� ���� ��������
    float4 depthValue = depthTexture.Load(int3(texCoord, 0));

    // �ؽ�ó���� ������ ���� ���� NDC ������ ���� ���ɼ��� ����.
    // ���� �̸� ���� ��ǥ�� ���̿� ���ϱ� ���� Far Plane�� ���� ����ȭ��.
    float depthInTexture = depthValue.x * g_fFar; // ����ȭ�� ���� ��
    bool isVisible = (ndcDepth <= depthInTexture);

    // ���ü� ����� int ������ ��� (1: ����, 0: ������)
    g_OutputBuffer[index] = isVisible ? 1 : 0;
}
