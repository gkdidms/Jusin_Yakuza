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

//이거 버퍼 바인딩은 꼭 다시 한번 둘러보기.
StructuredBuffer<BlendSort> g_BlendSorts;
RWStructuredBuffer<BlendSort> g_OutBlendSorts;

// 컴퓨트 셰이더 메인 함수
[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
     //상수 버퍼에서 BlendSort 배열 가져오기
    BlendSort blendSorts[256];
   // blendSorts[DTid.x] = g_BlendSorts[DTid.x];

     //버블 정렬 알고리즘을 사용하여 ViewZ 값 기준으로 정렬
    for (uint i = 0; i < 256; i++)
    {
        for (uint j = 0; j < 256 - i; j++)
        {
            if (blendSorts[j].ViewZ > blendSorts[j + 1].ViewZ)
            {
                // 두 BlendSort 구조체 swap
                BlendSort temp = blendSorts[j];
                blendSorts[j] = blendSorts[j + 1];
                blendSorts[j + 1] = temp;
            }
        }
    }

    // 정렬된 BlendSort 배열을 다시 상수 버퍼에 저장
    //g_OutBlendSorts[DTid.x] = blendSorts[DTid.x];
}
