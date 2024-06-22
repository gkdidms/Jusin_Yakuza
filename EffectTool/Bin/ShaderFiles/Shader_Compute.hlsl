struct VTXMATRIX
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float4 vDirection;
    float2 vLifeTime;
    float vRectSize;
};

struct BlendSort
{
    VTXMATRIX vMatrix;
    float ViewZ;
};

//�̰� ���� ���ε��� �� �ٽ� �ѹ� �ѷ�����.
StructuredBuffer<BlendSort> g_BlendSorts;
RWStructuredBuffer<BlendSort> g_OutBlendSorts;

// ��ǻƮ ���̴� ���� �Լ�
[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
     //��� ���ۿ��� BlendSort �迭 ��������
    BlendSort blendSorts[256];
   // blendSorts[DTid.x] = g_BlendSorts[DTid.x];

     //���� ���� �˰����� ����Ͽ� ViewZ �� �������� ����
    for (uint i = 0; i < 256; i++)
    {
        for (uint j = 0; j < 256 - i; j++)
        {
            if (blendSorts[j].ViewZ > blendSorts[j + 1].ViewZ)
            {
                // �� BlendSort ����ü swap
                BlendSort temp = blendSorts[j];
                blendSorts[j] = blendSorts[j + 1];
                blendSorts[j + 1] = temp;
            }
        }
    }

    // ���ĵ� BlendSort �迭�� �ٽ� ��� ���ۿ� ����
    //g_OutBlendSorts[DTid.x] = blendSorts[DTid.x];
}
