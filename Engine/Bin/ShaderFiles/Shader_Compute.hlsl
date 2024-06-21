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

// 컴퓨트 셰이더 메인 함수
[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    
    
    // 컴퓨트 셰이더 코드
    // 상수 버퍼의 BlendSort 구조체 데이터 사용
}