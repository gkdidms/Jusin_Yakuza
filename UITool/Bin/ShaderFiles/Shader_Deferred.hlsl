#include "Shader_HDR.hlsl"

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}


VS_OUT_REVERSE VS_MAIN_REVERSE(VS_IN In)
{
    VS_OUT_REVERSE Out = (VS_OUT_REVERSE) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    float3 vPosReverse = In.vPosition;
    vPosReverse.y *= -1;
    
    Out.vPositionReverse = mul(float4(vPosReverse, 1.f), matWVP);

    return Out;
}

struct PS_OUT_GAMEOBJECT
{
    vector vColor : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRM : SV_TARGET3;
    vector vRS : SV_Target4;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vLightMap : SV_TARGET1;
    vector vSpecularRM : SV_TARGET2;
    vector vSpecularMulti : SV_TARGET3;
    vector vSpecular : SV_TARGET4;
};

PS_OUT PS_MAIN_SSAO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    
    if (vNormalDesc.a != 0.f)
    {
        Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    }
    else
    {
        //뷰스페이스 위치로 옮기기
        vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
        vNormal = normalize(mul(vNormal, g_CamViewMatrix));
        
        //뷰행렬 상의 위치 구하기
        vector vPosition;
        
        vPosition.x = In.vTexcoord.x * 2.f - 1.f;
        vPosition.y = In.vTexcoord.y * -2.f + 1.f;
        vPosition.z = vDepthDesc.x; /* 0 ~ 1 */
        vPosition.w = 1.f;

        vPosition = vPosition * (vDepthDesc.y * g_fFar);
        vPosition = mul(vPosition, g_ProjMatrixInv);
        
        float4 fAmbient = SSAO(Get_TBN(vNormal.xyz, In.vTexcoord), vPosition.xyz);

        Out.vColor = 1.f - fAmbient;
    }
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float vAmbientDesc = g_AmbientTexture.Sample(LinearSampler, In.vTexcoord).r;
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    
    vector vAmbient = g_vLightAmbient * g_vMtrlAmbient;
    if (g_isSSAO)
        vAmbient *= vAmbientDesc;
    
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f) + vAmbient);
    //Grass
    vector vGlassNormalDesc = g_GlassNormalTexture.Sample(LinearSampler,In.vTexcoord);
    vector vGlassNormal = vector(vGlassNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    Out.vSpecularRM = BRDF(In.vPosition, In.vTexcoord, normalize(vNormal), vDepthDesc);
    Out.vSpecularMulti = vector(BRDF_MULTI(In.vPosition, In.vTexcoord, normalize(vNormal), vDepthDesc), 1.f);
    Out.vLightMap = g_vLightDiffuse;
    
    if (vGlassNormalDesc.a == 0.f)
    {
        vector vGlassDepthDesc = g_GlassDepthTexture.Sample(PointSampler, In.vTexcoord);
    
        vector vWorldPos;

        vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
        vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
        vWorldPos.z = vGlassDepthDesc.x;
        vWorldPos.w = 1.f;

        vWorldPos = vWorldPos * (vGlassDepthDesc.y * g_fFar);
    
        vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
        vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

        vector vReflect = reflect(normalize(g_vLightDir) , normalize(vGlassNormal));
        vector vLook = normalize(g_vCamPosition - vWorldPos);
    
        Out.vSpecular = pow(max(dot(normalize(vReflect), normalize(vLook)), 0.f), 30.f);
    }
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);

	/* 뷰스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


    vector vLightDir = normalize(vWorldPos - g_vLightPos);
    float fDistance = length(vLightDir);

    float fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

    Out.vShade = g_vLightDiffuse *
		saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
    
    Out.vLightMap = g_vLightDiffuse * fAtt;
    
    return Out;
}

PS_OUT PS_MAIN_COPY_BACKBUFFER_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;
    
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpeculer = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (g_isPBR)
    {
        vector vSpeculerRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
        vector vMetallicMulti = g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
        vector vLightmap = g_LightMapTexture.Sample(LinearSampler, In.vTexcoord);
    // 기존
        Out.vColor = (vDiffuse * vMetallicMulti) * vShade + (vSpeculerRM + vSpeculer) * vLightmap;
    }
    else
    {
        Out.vColor = vDiffuse * vShade;
    }
    
    if (g_isShadow)
    {
        vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
        vector vWorldPos;

        vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
        vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
        vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
        vWorldPos.w = 1.f;

        vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);

	/* 뷰스페이스 상의 위치를 구한다. */
        vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
        vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

        vector vLightPos = mul(vWorldPos, g_LightViewMatrix);
        vLightPos = mul(vLightPos, g_LightProjMatrix);
        vLightPos = vector(vLightPos.xyz / vLightPos.w, 1.f);
        
        float2 vTexcoord;
        vTexcoord.x = vLightPos.x * 0.5f + 0.5f;
        vTexcoord.y = vLightPos.y * -0.5f + 0.5f;
        float fDepth = vLightPos.z;
        
       // float percentLit = 0.0f;
        
        /*const float2 offsets[9] =
        {
            float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
            float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
            float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
        };
        */
        
       // vector vLightDepthDesc = g_LightDepthTexture.Sample(PointSampler, vTexcoord);
        vector vPassiveLightDepthDesc = g_PassiveLightDepthTexture.Sample(PointSampler, vTexcoord);
        
        //float fLightOldDepth = vLightDepthDesc.x * 1000.f;
        float fPassiveLightOldDepth = vPassiveLightDepthDesc.x * 1000.f;
        
        //if (fLightOldDepth + 0.1f < vLightPos.w)
        //    Out.vColor = vector(Out.vColor.rgb * 0.5f, 1.f);
        
        if (fPassiveLightOldDepth + 0.1f < vLightPos.w)
            Out.vColor = vector(Out.vColor.rgb * 0.5f, 1.f);
    }
    
    return Out;
}

PS_OUT PS_MAIN_DEFERRED_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord);
   
    Out.vColor = vDiffuse;

    return Out;
}

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
    
    float vResult = g_ResultTexture.Sample(PointSampler, In.vTexcoord).r;
    
      // 최종 출력 계산(알파*가중치)를 빼주는작업= 모두 함친 색이 나 옴
    //vector FinalColor = float4(vAccumColor.xyz / vAccumColor.a, (1-vAccumAlpha));
    vector FinalColor = float4(vAccumColor.xyz , (1-vAccumAlpha));

    //Out.vColor = vResult+FinalColor;
    Out.vColor = FinalColor;

    return Out;
}

PS_OUT_GAMEOBJECT PS_INCLUDE_GLASS(PS_IN In)
{

    PS_OUT_GAMEOBJECT Out = (PS_OUT_GAMEOBJECT) 0;

    vector vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassDiffuseColor = g_GlassDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vNonBlendNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassNormal = g_GlassNormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vNonBlendDepth = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassDepth = g_GlassDepthTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vNonBlendRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassRM = g_GlassRMTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vNonBlendRS = g_RSTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassRS = g_GlassRSTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    if (vNonBlendDepth.r < vGlassDepth.r)
    {
        // Nonblend
        Out.vColor = vDiffuseColor;
        Out.vNormal = vNonBlendNormal;
        Out.vDepth = vNonBlendDepth;
        Out.vRM = vNonBlendRM;
        Out.vRS = vNonBlendRS;
    }
    else
    {
        // Glass
        Out.vColor = vGlassDiffuseColor;
        Out.vNormal = vGlassNormal;
        Out.vDepth = vGlassDepth;
        Out.vRM = vGlassRM;
        Out.vRS = vGlassRS;
    }
    
    return Out;
}

//BOF
PS_OUT PS_MAIN_BOF(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuseDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDiffuseBlurDesc = g_BackBlurTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);

	/* 뷰스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    float fDistance = length(g_vCamPosition.xyz - vWorldPos.xyz);
    
    Out.vColor = lerp(vDiffuseDesc, vDiffuseBlurDesc, fDistance / g_fFar * 100.f);
    
    return Out;
}


PS_OUT PS_ADD_PUDDLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vPuddle = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if(0 == vPuddle.r && 0 == vPuddle.g && 0 == vPuddle.b)
        discard;
    
    Out.vColor = vPuddle;

    
    return Out;
}


technique11 DefaultTechnique
{

	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
    pass DebugRender // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass Copy_BackBuffer // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COPY_BACKBUFFER_RESULT();
    }

    pass Deferred_Result // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_RESULT();
    }

    pass Luminance // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE_SUM();
    }

    pass Luminance_Roof // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE_SUM_LOOP();
    }

    pass Luminance_Final // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE();
    }

    pass CopyLuminance // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COPYLUMINANCE();
    }

    pass ToneMapping // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TONEMAPPING();
    }

    pass Luminance_1x1 // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE_SUM_FINAL();
    }

    pass Blur_X //11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass Blur_Y //12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_REVERSE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    pass Result //13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RESULT();
    }


    pass SSAO //14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSAO();

    }
    pass OITRender //15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_OIT_RESULT();   
    }
    
    pass IncludeGlassRender //16 - diffuse와 Glass 합치기
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_INCLUDE_GLASS();
    }

    pass BOF //17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BOF();
    }


    pass DiffusePuddleAdd //18
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ADD_PUDDLE();
    }
}

