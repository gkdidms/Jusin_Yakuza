#include "Shader_Neo.hlsli"

[numthreads(8, 8, 1)]
void GS_MAIN(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // 전체 정점 인덱스 계산
    uint globalIndex = dispatchThreadID.x;
    
    GS_IN In = g_InputBuffer[globalIndex];
    
    // 텍스쳐 연산 진행
    uint2 RTTexcoord, RDTexcoord, RSTexcoord, RMTexcoord, MultiTexcoord, NormalTexcoord, DiffuseTexcoord;
    uint2 Texcoord = In.vTexcoord * g_WindowSize;
    
    DiffuseTexcoord = In.vTexcoord * TextureSize[0];
    RTTexcoord = UV_RT(In.vTexcoord * TextureSize[2]);
    RDTexcoord = UV_RDRMRS(In.vTexcoord * TextureSize[5]);
    RSTexcoord = UV_RDRMRS(In.vTexcoord * TextureSize[4]);
    RMTexcoord = UV_RDRMRS(In.vTexcoord * TextureSize[3]);
    NormalTexcoord = In.vTexcoord * TextureSize[1];
    MultiTexcoord = In.vTexcoord * TextureSize[6];
    
    float4 vDiffuseDesc = g_DiffuseTexture.Load(int3(DiffuseTexcoord, 0));
    float4 vMulti = g_isMulti ? g_MultiTexture.Load(int3(MultiTexcoord, 0)) : float4(0.f, 1.f, 0.f, 1.f);
    float4 vRD = g_isRD ? g_RDTexture.Load(int3(RDTexcoord, 0)) : vector(1.f, 1.f, 1.f, 1.f);
    float4 vRS = g_isRS ? g_RSTexture.Load(int3(RSTexcoord, 0)) : vector(1.f, 1.f, 1.f, 1.f);
    float4 vRM = g_isRM ? g_RMTexture.Load(int3(RMTexcoord, 0)) : vector(0.5f, 1.f, 0.5f, 1.f);
    float4 vRT = g_isRT ? g_RTTexture.Load(int3(RTTexcoord, 0)) : vector(0.5f, 0.5f, 1.f, 0.5f);
    
    //노말 벡터 구하기
    vector vNormalDesc = g_isNormal ? g_NormalTexture.Load(int3(NormalTexcoord, 0)) : vector(0.5f, 0.5f, 0.5f, 0.f);
    
    //Neo Shader
    float fFactor = RepeatingPatternBlendFactor(vMulti);
    vector vDiffuse = DiffusePortion(vDiffuseDesc, vRS, vRD, fFactor, In.vTexcoord);
    
    COMBINE_OUT Result = Neo_MetallicAndGlossiness(vMulti, vRM); // Metallic, Rouhness 최종
    vDiffuse = Get_Diffuse(vMulti.a, vDiffuse); // Diffuse 최종
    
    //Tangent Normal 구하기 
    vNormalDesc = Get_Normal(vNormalDesc, vRT, fFactor);
    
    OE_SPECULAR OEResult = Neo_OE_Specular(vMulti, vRM, vRS);
    float fMixMultiFactor = lerp(vMulti.y, 1.f, AssetShader);
    float fDeffuseFactor = vDiffuseDesc.a * 1.f;
    
    g_OutDiffuseTexture[Texcoord] = vDiffuse;
    g_OutNormalTexture[Texcoord] = vector(vNormalDesc.xyz, 0.f);
    g_OutSurfaceTexture[Texcoord] = vector(Result.fMetalness, Result.fRoughness, Result.fSpeclure, 0);
    g_OutOEShaderTexture[Texcoord] = vector(OEResult.fRouhness, OEResult.fMixShaderFactor, fMixMultiFactor, fDeffuseFactor);
    g_OutSpecularTexture[Texcoord] = vector(OEResult.vSpecular, 0.f);
}