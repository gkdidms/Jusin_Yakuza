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

// ��ǻƮ ���̴� ���� �Լ�
[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    
    
    // ��ǻƮ ���̴� �ڵ�
    // ��� ������ BlendSort ����ü ������ ���
}