#include "Shader_HDR.hlsl"

float4 Blur_X(float2 vTexCoord)
{
    float4 vOut = (float4) 0;

    float2 vUV = (float2) 0;

    for (int i = -6; i < 7; ++i)
    {
        vUV = vTexCoord + float2(1.f / g_fTexW * i, 0);
        vOut += g_fWeight[6 + i] * g_EffectTexture.Sample(ClampSampler, vUV);
    }

    vOut /= g_fTotal;

    return vOut;
}

float4 Blur_Y(float2 vTexCoord)
{
    float4 vOut = (float4) 0;

    float2 vUV = (float2) 0;

    for (int i = -6; i < 7; ++i)
    {
        vUV = vTexCoord + float2(0, 1.f / g_fTexH * i);
        vOut += g_fWeight[6 + i] * g_EffectTexture.Sample(ClampSampler, vUV);
    }

    vOut /= g_fTotal;

    return vOut;
}

PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = Blur_X(In.vTexcoord);
	
    return Out;
}

PS_OUT_REVERSE PS_MAIN_BLUR_Y(PS_IN_REVERSE In)
{
    PS_OUT_REVERSE Out = (PS_OUT_REVERSE) 0;

    Out.vColor = Blur_Y(In.vTexcoord);
    
    float2 reverseTexcoord;
    reverseTexcoord.x = In.vPositionReverse.x / In.vPositionReverse.w / 2.f + 0.5f;
    reverseTexcoord.y = -In.vPositionReverse.y / In.vPositionReverse.w / 2.f + 0.5f;
    
    Out.vReverseColor = Blur_Y(reverseTexcoord);

    return Out;
}

PS_OUT PS_MAIN_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vResult = g_ResultTexture.Sample(LinearSampler, In.vTexcoord);

    vector vBlur = g_BlurTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vEffect = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);
   
    
    Out.vColor = vResult + vBlur + vEffect;
    
    return Out;
}


PS_OUT PS_OIT_RESULT(PS_IN In)
{

    PS_OUT Out = (PS_OUT) 0;

    vector vAccumColor = g_AccumTexture.Sample(PointSampler, In.vTexcoord);
    float vAccumAlpha = g_AccumAlpha.Sample(PointSampler, In.vTexcoord).r;
    
  //  float vResult = g_ResultTexture.Sample(PointSampler, In.vTexcoord).r;
    
      // ���� ��� ���(����*����ġ)�� ���ִ��۾�= ��� ��ģ ���� �� ��
    //vector FinalColor = float4(vAccumColor.xyz / vAccumColor.a, (1-vAccumAlpha));
    vector FinalColor = float4(vAccumColor.xyz, (1 - vAccumAlpha));

    //Out.vColor = vResult+FinalColor;
    Out.vColor = FinalColor;

    return Out;
}

PS_OUT PS_RIMLIGHT(PS_IN In)//���� ���� ���ؾߵ�
{
    PS_OUT Out = (PS_OUT) 0;

    vector BaseNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);//���� ���
    BaseNormal = vector(BaseNormal.xyz * 2.f - 1.f, 0.f);
    
    vector BaseDepth = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);

    vector vCamDir = g_vCamPosition; //���� ī�޶�
   
    vector RimColor = vector(1.0f, 0.0f, 1.0f, 1.0f);
    
    if (0.05f <= BaseDepth.z)
        RimColor = vector(0.6f, 0.85f, 0.91f, 1.f);
    if (0.15f <= BaseDepth.z)
        RimColor = vector(0.92f, 0.05f, 0.90f, 1.f);
    if (0.25f <= BaseDepth.z)
        RimColor = vector(0.95f, 0.92f, 0.18, 1.f);


    float fRimpower =1.f;
    
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = BaseDepth.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (BaseDepth.y * g_fFar);
    float fProjZ = vWorldPos.z;

	        /* �佺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	        /* ���彺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
       
    
    vector vRim = normalize(vCamDir - vWorldPos);
    
    if(0.05f<BaseDepth.z)
    {
        float RangeAlpha = BaseDepth.w;
        float fRim = saturate(dot(BaseNormal, vRim));
        vector FinColor= float4(pow(1.f - fRim, fRimpower) * RimColor);
     //FinColor.a *= RangeAlpha;
        Out.vColor = FinColor;
    }
    else
    {
        Out.vColor = vector(0.f, 0.f, 0.f, 0.f);
    }
   
    
    return Out;
}

PS_OUT PS_DISTORTION(PS_IN In)//���� ���� ���ؾߵ�
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDist = g_Distortion.Sample(PointSampler, In.vTexcoord);
    
    float Weight = vDist.a * vDist.b; //����ġ

        
    float2 Texcoord = float2(In.vTexcoord.x + Weight, In.vTexcoord.y + Weight);

    vector BackBuffer = g_ResultTexture.Sample(LinearSampler, Texcoord);

    
    Out.vColor = BackBuffer;
    
    return Out;
}


float4 Blur_X_SIGN(float2 vTexCoord)
{
    float4 vOut = (float4) 0;

    float2 vUV = (float2) 0;

    for (int i = -6; i < 7; ++i)
    {
        vUV = vTexCoord + float2(1.f / g_fTexW * i, 0);
        
        float4 fColor = g_EffectTexture.Sample(ClampSampler, vUV);
        
        
        vOut += g_fWeight[6 + i] * g_EffectTexture.Sample(ClampSampler, vUV);
        
        
    }

    vOut /= g_fTotal;

    return vOut;
}

float4 Blur_Y_SIGN(float2 vTexCoord)
{
    float4 vOut = (float4) 0;

    float2 vUV = (float2) 0;

    for (int i = -6; i < 7; ++i)
    {
        float4 fColor = g_EffectTexture.Sample(ClampSampler, vUV);
        
        
        vUV = vTexCoord + float2(0, 1.f / g_fTexH * i);
        vOut += g_fWeight[6 + i] * g_EffectTexture.Sample(ClampSampler, vUV);
        
        
    }
   

    vOut /= g_fTotal;

    return vOut;
}

PS_OUT PS_MAIN_BLUR_X_SIGN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = Blur_X_SIGN(In.vTexcoord);
	
    return Out;
}

PS_OUT_REVERSE PS_MAIN_BLUR_Y_SIGN(PS_IN_REVERSE In)
{
    PS_OUT_REVERSE Out = (PS_OUT_REVERSE) 0;

    Out.vColor = Blur_Y_SIGN(In.vTexcoord);
    
    float2 reverseTexcoord;
    reverseTexcoord.x = In.vPositionReverse.x / In.vPositionReverse.w / 2.f + 0.5f;
    reverseTexcoord.y = -In.vPositionReverse.y / In.vPositionReverse.w / 2.f + 0.5f;
    
    Out.vReverseColor = Blur_Y_SIGN(reverseTexcoord);

    return Out;
}