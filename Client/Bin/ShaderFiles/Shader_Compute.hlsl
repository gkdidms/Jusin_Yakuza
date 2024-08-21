struct COMPUTEMATRIX
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float4 vDirection;
    float2 vLifeTime;
    float vRectSize;
    float4 vCamPos;
};

//�̰� ���� ���ε��� �� �ٽ� �ѹ� �ѷ�����.
RWStructuredBuffer<COMPUTEMATRIX> g_OutMatrix : register(u0);

groupshared COMPUTEMATRIX shared_data[256];

[numthreads(16, 16, 1)]
void CS_Main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID)
{
    // ������ �׷� ������ ���� �ε��� ���
    uint index = GTid.y * 16 + GTid.x;

    // �Է� ���ۿ��� ������ �ε�
    shared_data[index] = g_OutMatrix[DTid.x * 256 + index];

    // ������ �׷� ���ο��� ���� ���� ����
    for (uint i = 1; i < 256; i *= 2)
    {
        if ((index % (2 * i)) == 0)
        {
            if (index + i < 256 && any(abs(shared_data[index].vTranslation.xyz - shared_data[index].vCamPos.xyz) < abs(shared_data[index + i].vTranslation.xyz - shared_data[index + i].vCamPos.xyz)))
            {
                COMPUTEMATRIX temp = shared_data[index];
                shared_data[index] = shared_data[index + i];
                shared_data[index + i] = temp;
            }
        }
        GroupMemoryBarrierWithGroupSync();
    }

    // ���ĵ� �����͸� ��� ���ۿ� ����
    g_OutMatrix[DTid.x * 256 + index] = shared_data[index];
}