/* Header */
/* 상수 테이블과 struct 모음*/

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;

matrix g_CamProjMatrix, g_CamViewMatrix;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;
vector g_vCamDir;
float g_fFar;

Texture2D g_Texture;
Texture2D g_PriorityTexture;
Texture2D g_NormalTexture;
Texture2D g_DiffuseTexture;
Texture2D g_ShadeTexture;
Texture2D g_RMTexture;
Texture2D g_RSTexture;
Texture2D g_SpecularTexture;
Texture2D g_DepthTexture;
Texture2D g_MetallicTexture;
Texture2D g_BackBufferTexture;
Texture2D g_LightDepthTexture;
Texture2D g_ToneMappingTexture;
Texture2D g_LuminanceTexture;
Texture2D g_CopyLuminanceTexture;
Texture2D g_AmbientTexture;
//블러용
Texture2D g_EffectTexture;
//데칼용
Texture2D g_DecalTexture;

Texture2D g_BlurTexture;
Texture2D g_ResultTexture;
Texture2D g_AccumTexture;
Texture2D g_AccumAlpha;

Texture2D g_GlassDiffuseTexture;
Texture2D g_GlassNormalTexture;
Texture2D g_GlassDepthTexture;
Texture2D g_GlassRMTexture;
Texture2D g_GlassMetallicTexture;
Texture2D g_GlassRSTexture;

//HDR
float3 Luminance = float3(0.2125f, 0.7154f, 0.0721f);
float fDelta = { 0.0001f };
bool g_isFinished = { false };
float g_fLumVar;


/*블룸(가우시안) */
float g_fTexW = 1280.0f;
float g_fTexH = 720.0f;

static const float g_fWeight[13] =
{
    0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
	/*0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1, 
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561*/
};

float g_fTotal = 2.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

