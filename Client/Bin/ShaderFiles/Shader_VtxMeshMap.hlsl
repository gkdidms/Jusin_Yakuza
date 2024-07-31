#include "Engine_Shader_Defines.hlsli"
#include "Shader_Client_Defines.hlsli"


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vLocalPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

/* ���� ���̴� :  /* 
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);


    return Out;
}

struct VS_OUT_LIGHTDEPTH
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT_LIGHTDEPTH VS_MAIN_LIGHTDEPTH(VS_IN In)
{
    VS_OUT_LIGHTDEPTH Out = (VS_OUT_LIGHTDEPTH) 0;

    Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;

}

// LightDepth�� GS
struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    
    uint fIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(9)]
void GS_MAIN_LIGHTDEPTH(triangle GS_IN In[3], inout TriangleStream<GS_OUT> Out)
{
    GS_OUT Output[3] = (GS_OUT[3]) 0;
    
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            float4 vPosition = mul(In[j].vPosition, mul(g_ViewMatrixArray[i], g_ProjMatrixArray[i]));
            Output[j].vPosition = vPosition;
            Output[j].vProjPos = vPosition;
            Output[j].vTexcoord = In[j].vTexcoord;
            Output[j].fIndex = i;
            Out.Append(Output[j]);
        }
        Out.RestartStrip();
    }
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vLocalPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRM : SV_TARGET3;
    vector vRS : SV_Target4;
    vector vMulti : SV_Target5;
    vector vRD : SV_Target6;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMultiDiffuce = g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
   //  ������ ���(0.1���� ������ �����ϴ�) �׸��� ����
    //if (vDiffuse.a < 0.1f)
     //   discard;
    
     //RS + RD
    vector vRSRD;
    
    if (g_isRS)
    {
        vector vRSDesc = g_RSTexture.Sample(LinearSampler, In.vTexcoord);
        Out.vRS = vRSDesc;
        Out.vDiffuse = lerp(vDiffuse, vRSDesc, vMultiDiffuce.z);
    }
    else
        Out.vDiffuse = vDiffuse;
    
    float3 vNormal;
    if (true == g_bExistNormalTex)
    {
        // ���εǴ� texture�� ������
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
        vNormal = vector(vNormalDesc.w, vNormalDesc.y, 1.f, 0.f);
    
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
        vNormal = mul(vNormal.xyz, WorldMatrix);
    }
    else
    {
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        // �ؽ�ó ������
        vNormal = mul(In.vNormal.xyz, WorldMatrix);
    }
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    Out.vMulti = vMultiDiffuce;
    
    
    // specularTex�� metalic ���� rm ��� - bool �� ���� ����ϱ�
    if (true == g_bExistRMTex)
    {
        Out.vRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    }
    
    if (true == g_bExistRSTex)
    {
        Out.vRS = g_RSTexture.Sample(LinearSampler, In.vTexcoord);
    }
    
    
    return Out;
}

PS_OUT PS_GLASSDOOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vRefractTexCoord;
    vRefractTexCoord.x = In.vProjPos.x / In.vProjPos.w / 2.0f + 0.5f;
    vRefractTexCoord.y = -In.vProjPos.y / In.vProjPos.w / 2.0f + 0.5f;


    if (true == g_bExistNormalTex)
    {
        // Normal texture ������ vTexcoord �ٽ�
        float3 normal;
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
        normal = vNormalDesc.xyz * 2.f - 1.f;
        vRefractTexCoord = vRefractTexCoord + (normal.xy * g_fRefractionScale);
    }

    // Refract - ���� �ڿ� ��ġ�� ��
    float4 vRefractColor = g_RefractionTexture.Sample(LinearSampler, vRefractTexCoord);
    float4 vGlassTexColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vFinalColor = lerp(vRefractColor, vGlassTexColor, 0.5f);
    
    Out.vDiffuse = vFinalColor;
    
	
    // ������ ���(0.1���� ������ �����ϴ�) �׸��� ����
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    float3 vNormal;
    if (true == g_bExistNormalTex)
    {
        // ���εǴ� texture�� ������
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
        vNormal = mul(vNormal.xyz, WorldMatrix);
    }
    else
    {
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        // �ؽ�ó ������
        vNormal = mul(In.vNormal.xyz, WorldMatrix);
    }
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    
    // specularTex�� metalic ���� rm ��� - bool �� ���� ����ϱ�
    if (true == g_bExistRMTex)
    {
        Out.vRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    }
    
    
    return Out;
}



PS_OUT PS_MAIN_AlphaMask(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    


    Out.vDiffuse = vDiffuse;
    
    
    return Out;
}

PS_OUT PS_MAIN_LightMask_Alpha(PS_IN In)
{
    // ����� �ڸ��°�
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = vDiffuse;
    
    return Out;
}


PS_OUT DEFAULT_SIGN_PASS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMultiDiffuce = g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector emissiveColor = vDiffuse * 0.4;
   
    vDiffuse += emissiveColor;
    Out.vDiffuse = vDiffuse;
    
    // //RS + RD
    //vector vRSRD;
    
    //if (g_isRS)
    //{
    //    vector vRSDesc = g_RSTexture.Sample(LinearSampler, In.vTexcoord);
    //    Out.vRS = vRSDesc;
    //     = lerp(vDiffuse, vRSDesc, vMultiDiffuce.z);
    //}
    //else
    //    Out.vDiffuse = vDiffuse;
    
    //float3 vNormal;
    //if (true == g_bExistNormalTex)
    //{
    //    // ���εǴ� texture�� ������
    //    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    //    vNormal = vNormalDesc.xyz * 2.f - 1.f;
    //    vNormal = vector(vNormalDesc.w, vNormalDesc.y, 1.f, 0.f);
    
    //    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    //    vNormal = mul(vNormal.xyz, WorldMatrix);
    //}
    //else
    //{
    //    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    //    // �ؽ�ó ������
    //    vNormal = mul(In.vNormal.xyz, WorldMatrix);
    //}
    
    //Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    //Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    //Out.vMulti = vMultiDiffuce;
    
    
    //// specularTex�� metalic ���� rm ��� - bool �� ���� ����ϱ�
    //if (true == g_bExistRMTex)
    //{
    //    Out.vRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    //}
    
    //if (true == g_bExistRSTex)
    //{
    //    Out.vRS = g_RSTexture.Sample(LinearSampler, In.vTexcoord);
    //}
    
    
    return Out;
}

PS_OUT PS_MAIN_Lamp(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector emissive;
    if (true == g_bExistRMTex)
    {
        emissive = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    }
    else
    {
        emissive = vDiffuse.r;
    }
   
    vector finalColor = saturate(vDiffuse + (emissive.r) * float4(1, 0.5, 0, 1) * 0.5);
    
    if (finalColor.r == 1 && finalColor.g == 1 && finalColor.b == 1)
        finalColor = float4(1, 0.5, 0, 1);
    
  
    Out.vDiffuse = vDiffuse;
    
    return Out;
}

PS_OUT PS_DECAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);  
   
    if(vDiffuse.a < 0.3)
        discard;
    
    Out.vDiffuse = vDiffuse;
    
    return Out;
}

PS_OUT PS_BloomWhite(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    
   
    if (vDiffuse.r + vDiffuse.g + vDiffuse.b > 1.8)
    {
        // �ణ�� bloom �ǰԲ�
        Out.vDiffuse = vDiffuse * 0.5;
    }
    else
    {
        discard;
    }
    
    return Out;
  

}


PS_OUT PS_MaskEmissive(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector emissiveColor;
    
    emissiveColor.rgb = vDiffuse.rgb * 0.4;
    
    emissiveColor.a = vDiffuse.a;
    
    if (vDiffuse.r < 0.3 && vDiffuse.g < 0.3 && vDiffuse.b < 0.3 && vDiffuse.a < 0.3)
        discard;
    
    Out.vDiffuse = vDiffuse + emissiveColor;
    
    return Out;
  

}



struct PS_IN_LIGHTDEPTH
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT_LIGHTDEPTH
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN_LIGHTDEPTH In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;

    Out.vLightDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 0.f);
    
    return Out;
}




technique11 DefaultTechnique
{
    pass DefaultPass //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass GlassDoorPass //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_GLASSDOOR();
    }

    pass BlackMaskPass //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_AlphaMask();
    }


    pass ALPHALightMaskPass //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LightMask_Alpha();
    }

    pass SIGNPASS //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 DEFAULT_SIGN_PASS();
    }

    pass LampPass //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Lamp();
    }

    pass DECALPASS //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DECAL();
    }

    pass BloomWhite //7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BloomWhite();
    }

    pass maskEmissive //8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MaskEmissive();
    }

   
    pass LightDepth // - construction , Construction�� render light depth���� �������ֱ�
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_LIGHTDEPTH();
        GeometryShader = compile gs_5_0 GS_MAIN_LIGHTDEPTH();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }


}