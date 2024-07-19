#include "Loader.h"

#include "GameInstance.h"

#pragma region Player
#include "Player.h"
#include "RushYakuza.h"

#include "SocketCollider.h"
#include "SocketEffect.h"
#pragma endregion

#pragma region Monster
#include "RushYakuza.h"
#include "WPAYakuza.h"
#include "Shakedown.h"
#include "Kuze.h"
#include "Yoneda.h"
#pragma endregion


#pragma region BTNode
#include "AI_RushYakuza.h"
#include "AI_WPAYakuza.h"
#include "AI_Shakedown.h"
#include "AI_Kuze.h"
#pragma endregion

#pragma region Camera
#include "PlayerCamera.h"
#include "DebugCamera.h"
#include "CineCamera.h"
#pragma endregion

#pragma region Map
#include "Terrain.h"
#include "Construction.h"
#include "MapCollider.h"
#include "SkyDome.h"
#include "MapColliderObj.h"
#include "Decal.h"
#include "LightConstruction.h"
#pragma endregion

#pragma region MyRegion
#include "LevelTrigger.h"
#pragma endregion


#pragma region Effect
#include "Particle_Point.h"
#include "TRailEffect.h"
#include "Aura.h"
#pragma endregion

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice },
	m_pContext{ pContext },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Loading()))
		return 1;

	CoUninitialize();

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT			hr{};

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_OFFICE_1F:
		hr = Loading_For_Office_1F();
		break;
	case LEVEL_OFFICE_2F:
		hr = Loading_For_Office_2F();
		break;
	case LEVEL_OFFICE_BOSS:
		hr = Loading_For_Office_Boss();
		break;
	case LEVEL_DOGIMAZO:
		hr = Loading_For_Dogimazo();
		break;
	case LEVEL_DOGIMAZO_STAIRS:
		hr = Loading_For_Dogimazo_Stairs();
		break;
	case LEVEL_DOGIMAZO_LOBBY:
		hr = Loading_For_Dogimazo_Lobby();
		break;
	case LEVEL_DOGIMAZO_BOSS:
		hr = Loading_For_Dogimazo_Boss();
		break;
	case LEVEL_TEST:
		hr = Loading_For_Test();
	}

	LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

/* 공통적인 저장 객체를 넣어주는 함수. */
HRESULT CLoader::Loading_Default()
{
#pragma region Texture
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/T_Sand_06_A.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region Effect_Texture
	/* Prototype_Component_Texture_Sphere */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Sphere"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/lm000.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/001_trc_n.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

	/* Prototype_Component_Texture_Test */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Test"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/9923j.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HitSpark */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_HitSpark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/015_trc_na.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraAnim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0076_aura_g.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraFluid */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraFluid"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0309_fluidFlowA01.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraBase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0291_smoke_jA8L8.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraTone */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraTone"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/auratone24.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HitBase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_HitBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0233_radialpat_d.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_SmokeBase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_SmokeBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_o_2208_smoke_j.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraToneRush */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraToneRush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/auratone22.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraToneDestroy */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraToneDestroy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/auratone17.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GuardParticle */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_GuardParticle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/001_trc_na.dds"), 1))))
		return E_FAIL;
	/* Prototype_Component_Texture_GuardSmoke */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_GuardSmoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0351_2d_moya.dds"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region BTNode
	lstrcpy(m_szLoadingText, TEXT("행동트리 원형 를(을) 로딩 중 입니다."));
	/* For.Prototype_BTNode_RushYakuza*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_RushYakuza"),
		CAI_RushYakuza::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_WPAYakuza*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_WPAYakuza"),
		CAI_WPAYakuza::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_Shakedown*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_Shakedown"),
		CAI_Shakedown::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_Kuze*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_Kuze"),
		CAI_Kuze::Create())))
		return E_FAIL;
#pragma endregion

#pragma region Component
	lstrcpy(m_szLoadingText, TEXT("컴포넌트 원형 를(을) 로딩 중 입니다."));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Terrain_Flat"), CVIBuffer_Terrain_Flat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Anim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation.dat", false))))
		return E_FAIL;
	/*if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Anim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation.fbx", true))))
		return E_FAIL;*/

		/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Collider"), CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Instance_Point */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region Shader
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));
	/* For.Prototype_Component_Shader_VtxAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_Mesh */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_Mesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxMeshSky */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxMeshSky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshSky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxMeshLight */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxMeshLight"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshLight.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_Aura*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_Aura"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Aura.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;
#pragma endregion


	return S_OK;
}

HRESULT CLoader::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Office_1F()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_0.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/"));

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map0"));

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Model_Bone_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NonAnim/Bone_Sphere/Bin/Square.dat", PreTransformMatrix, true))))
		return E_FAIL;
#pragma endregion

#pragma region GameObject
	lstrcpy(m_szLoadingText, TEXT("객체원형를(을) 로딩 중 입니다."));

	Add_GameObject_Particle_On_Path(TEXT("../../Client/Bin/DataFiles/Particle/"));

#pragma region Camera
	/* For.Prototype_GameObject_DebugCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_DebugCamera"), CDebugCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PlayerCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_PlayerCamera"), CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CCineCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CCineCamera"), CCineCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Player
	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SoketCollider */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_SoketCollider"),
		CSocketCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SoketEffect */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_SoketEffect"),
		CSocketEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Monster
	/* For.Prototype_GameObject_RushYakuza */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_RushYakuza"), CRushYakuza::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WPAYakuza */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_WPAYakuza"), CWPAYakuza::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Shakedown */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Shakedown"), CShakedown::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Kuze */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Kuze"), CKuze::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion



#pragma region Map
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Construction */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Construction"),
		CConstruction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Decal */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Decal"),
		CDecal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapCollider */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_MapCollider"),
		CMapCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SkyDome */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_SkyDome"),
		CSkyDome::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapColliderObj */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_MapColliderObj"),
		CMapColliderObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LightConstruction */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_LightConstruction"),
		CLightConstruction::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_LevelTrigger */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_LevelTrigger"),
		CLevelTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Office_2F()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_1.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/"));

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map0"));

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Model_Bone_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NonAnim/Bone_Sphere/Bin/Square.dat", PreTransformMatrix, true))))
		return E_FAIL;
#pragma endregion

#pragma region GameObject
	lstrcpy(m_szLoadingText, TEXT("객체원형를(을) 로딩 중 입니다."));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Office_Boss()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_2.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/"));

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map0"));

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Model_Bone_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NonAnim/Bone_Sphere/Bin/Square.dat", PreTransformMatrix, true))))
		return E_FAIL;
#pragma endregion

#pragma region GameObject
	lstrcpy(m_szLoadingText, TEXT("객체원형를(을) 로딩 중 입니다."));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Dogimazo()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_3.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/"));

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map1"));

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Model_Bone_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NonAnim/Bone_Sphere/Bin/Square.dat", PreTransformMatrix, true))))
		return E_FAIL;
#pragma endregion

#pragma region GameObject
	lstrcpy(m_szLoadingText, TEXT("객체원형를(을) 로딩 중 입니다."));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Dogimazo_Stairs()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_4.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/"));

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map1"));

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Model_Bone_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NonAnim/Bone_Sphere/Bin/Square.dat", PreTransformMatrix, true))))
		return E_FAIL;
#pragma endregion

#pragma region GameObject
	lstrcpy(m_szLoadingText, TEXT("객체원형를(을) 로딩 중 입니다."));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Dogimazo_Lobby()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_5.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/"));

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map1"));

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Model_Bone_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NonAnim/Bone_Sphere/Bin/Square.dat", PreTransformMatrix, true))))
		return E_FAIL;
#pragma endregion

#pragma region GameObject
	lstrcpy(m_szLoadingText, TEXT("객체원형를(을) 로딩 중 입니다."));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Dogimazo_Boss()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;
		
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_6.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/"));

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map1"));

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Model_Bone_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NonAnim/Bone_Sphere/Bin/Square.dat", PreTransformMatrix, true))))
		return E_FAIL;
#pragma endregion

#pragma region GameObject
	lstrcpy(m_szLoadingText, TEXT("객체원형를(을) 로딩 중 입니다."));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Test()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path(LEVEL_TEST, TEXT("../Bin/Resources/Models/Anim/"));

	Add_Models_On_Path_NonAnim(LEVEL_TEST, TEXT("../Bin/Resources/Models/NonAnim/Map/Map0"));

	Add_Models_On_Path_NonAnim(LEVEL_TEST, TEXT("../Bin/Resources/Models/NonAnim/Map/Map1"));

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_Bone_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NonAnim/Bone_Sphere/Bin/Square.dat", PreTransformMatrix, true))))
		return E_FAIL;
#pragma endregion

#pragma region GameObject
	lstrcpy(m_szLoadingText, TEXT("객체원형를(을) 로딩 중 입니다."));

	Add_GameObject_Particle_On_Path(TEXT("../../Client/Bin/DataFiles/Particle/"));

#pragma region Camera
	/* For.Prototype_GameObject_DebugCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_DebugCamera"), CDebugCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PlayerCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_PlayerCamera"), CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CCineCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CCineCamera"), CCineCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Player
	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SoketCollider */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_SoketCollider"),
		CSocketCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SoketEffect */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_SoketEffect"),
		CSocketEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Monster
	/* For.Prototype_GameObject_RushYakuza */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_RushYakuza"),
		CRushYakuza::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WPAYakuza */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_WPAYakuza"),
		CWPAYakuza::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Shakedown */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Shakedown"),
		CShakedown::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Kuze */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Kuze"),
		CKuze::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Yoneda */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Yoneda"),
		CYoneda::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Map
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Construction */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Construction"),
		CConstruction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Decal */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Decal"),
		CDecal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapCollider */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_MapCollider"),
		CMapCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SkyDome */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_SkyDome"),
		CSkyDome::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapColliderObj */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_MapColliderObj"),
		CMapColliderObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LightConstruction */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_LightConstruction"),
		CLightConstruction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LevelTrigger */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_LevelTrigger"),
		CLevelTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

// 경로를 넣어주면 디렉토리 이름을 읽어와서 bin존재 유무를 파악해 바이너리화 or bin파일 읽기를 자동으로 해준다
// 하지만 해당 함수를 사용하려면 넣어준 경로 안에 폴더명과 똑같은 이름의 fbx를 가지고 있어야 한다.
HRESULT CLoader::Add_Models_On_Path(_uint iLevel, const wstring& strPath, _bool bAnim)
{
	vector<wstring> vecDirectorys;
	m_pGameInstance->Get_DirectoryName(strPath, vecDirectorys);

	_matrix		PreTransformMatrix;

	for (auto& strDirlName : vecDirectorys)
	{
		wstring strFilePath = strPath + strDirlName + TEXT("/");

		string strDirectory = m_pGameInstance->WstringToString(strFilePath);
		string strBinPath = strDirectory + "Bin/";

		if (!fs::exists(strBinPath))
		{
			wstring strComponentName = TEXT("Prototype_Component_Model_") + strDirlName;
			wstring strFbxPath = strFilePath + strDirlName + TEXT(".fbx");
			string strTransPath = m_pGameInstance->WstringToString(strFbxPath);

			if (!bAnim)
			{
				PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
				if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strTransPath.c_str(), PreTransformMatrix, false))))
					return E_FAIL;
			}
			else
			{
				PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
				if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, strTransPath.c_str(), PreTransformMatrix, false))))
					return E_FAIL;
			}
		}
		else
		{
			for (const auto& entry : std::filesystem::directory_iterator(strBinPath))
			{
				string file_path = entry.path().string();
				string strFileName = m_pGameInstance->Get_FileName(file_path);
				wstring strComponentName = TEXT("Prototype_Component_Model_") + m_pGameInstance->StringToWstring(strFileName);

				if (!bAnim)
				{
					PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
					if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, file_path.c_str(), PreTransformMatrix, true))))
						return E_FAIL;
				}
				else
				{
					PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
					if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, file_path.c_str(), PreTransformMatrix, true))))
						return E_FAIL;
				}

			}
		}

	}

	return S_OK;
}

HRESULT CLoader::Add_GameObject_Particle_On_Path(const wstring& strPath)
{
	vector<wstring> vecDirectorys;
	m_pGameInstance->Get_DirectoryName(strPath, vecDirectorys);

	for (auto& strChannelName : vecDirectorys)
	{
		wstring strFilePath = strPath + strChannelName + TEXT("/");
		string strDirectory = m_pGameInstance->WstringToString(strFilePath);

		if (TEXT("Point") == strChannelName)
		{
			for (const auto& entry : fs::directory_iterator(strDirectory))
			{

				string FileName = entry.path().filename().string();
				string AllPath = strDirectory + FileName;

				string Tag;
				_int dotPos = FileName.find_last_of(".");
				Tag = FileName.substr(0, dotPos);


				/* For.Prototype_GameObject_Particle_Point */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(m_pGameInstance->StringToWstring(Tag),
					CParticle_Point::Create(m_pDevice, m_pContext, AllPath))))
					return E_FAIL;

			}
		}
		else if (TEXT("Trail") == strChannelName)
		{
			for (const auto& entry : fs::directory_iterator(strDirectory))
			{

				string FileName = entry.path().filename().string();
				string AllPath = strDirectory + FileName;

				string Tag;
				_int dotPos = FileName.find_last_of(".");
				Tag = FileName.substr(0, dotPos);


				/* For.Prototype_GameObject_Particle_Trail */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(m_pGameInstance->StringToWstring(Tag),
					CTRailEffect::Create(m_pDevice, m_pContext, AllPath))))
					return E_FAIL;

			}
		}
		else if (TEXT("Aura") == strChannelName)
		{
			for (const auto& entry : fs::directory_iterator(strDirectory))
			{

				string FileName = entry.path().filename().string();
				string AllPath = strDirectory + FileName;

				string Tag;
				_int dotPos = FileName.find_last_of(".");
				Tag = FileName.substr(0, dotPos);


				/* For.Prototype_GameObject_Particle_Aura */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(m_pGameInstance->StringToWstring(Tag),
					CAura::Create(m_pDevice, m_pContext, AllPath))))
					return E_FAIL;

			}
		}


	}
	return S_OK;
}

HRESULT CLoader::Add_Models_On_Path_NonAnim(_uint iLevel, const wstring& strPath)
{
	//vector<wstring> vecDirectorys;
	//m_pGameInstance->Get_DirectoryName(strPath, vecDirectorys);

	//for (int i = 0; i < vecDirectorys.size(); i++)
	//{
	//	wstring original = vecDirectorys[i];
	//	wstring to_remove = L"\\";
	//	int pos = original.find(to_remove);

	//	if (pos != wstring::npos) {
	//		original.erase(pos, to_remove.length());
	//	}

	//	vecDirectorys[i] = original;
	//}

	//_matrix		NonAnimPreTransformMatrix;

	//for (auto& strDirlName : vecDirectorys)
	//{
	//	vector<string> fbxFilesName;
	//	wstring		wstrFullPath = strPath + L"/" + strDirlName;

	//	// fbx 제외하고 fbx 파일 이름들 저장
	//	for (const auto& entry : fs::directory_iterator(wstrFullPath)) {
	//		if (entry.is_regular_file() && entry.path().extension() == L".fbx") {

	//			string strFileName = entry.path().filename().string();

	//			size_t lastDot = entry.path().filename().string().find_last_of(".");

	//			strFileName = entry.path().filename().string().substr(0, lastDot);

	//			fbxFilesName.push_back(strFileName);
	//		}
	//	}

	//	for (const auto& fbxNames : fbxFilesName)
	//	{
	//		wstring strFilePath = strPath + TEXT("/") + strDirlName + TEXT("/");

	//		string strDirectory = m_pGameInstance->WstringToString(strFilePath);
	//		string strBinPath = strDirectory + "Bin/" + fbxNames + ".dat";

	//		if (!fs::exists(strBinPath))
	//		{
	//			wstring strFbxName = m_pGameInstance->StringToWstring(fbxNames);
	//			wstring strComponentName = TEXT("Prototype_Component_Model_") + strFbxName;
	//			wstring strFbxPath = strFilePath + strFbxName + TEXT(".fbx");
	//			string strTransPath = m_pGameInstance->WstringToString(strFbxPath);


	//			NonAnimPreTransformMatrix = XMMatrixIdentity();
	//			if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
	//				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strTransPath.c_str(), NonAnimPreTransformMatrix, false))))
	//				return E_FAIL;
	//		}
	//		else
	//		{
	//			wstring strComponentName = TEXT("Prototype_Component_Model_") + m_pGameInstance->StringToWstring(fbxNames);

	//			NonAnimPreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//			if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
	//				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strBinPath.c_str(), NonAnimPreTransformMatrix, true))))
	//				return E_FAIL;
	//		}
	//	}
	//}

	//vecDirectorys.clear();

	vector<string> fbxFilesName;
	wstring		wstrFullPath = strPath;
	_matrix		NonAnimPreTransformMatrix;

	// fbx 제외하고 fbx 파일 이름들 저장
	for (const auto& entry : fs::directory_iterator(wstrFullPath)) {
		if (entry.is_regular_file() && entry.path().extension() == L".fbx") {

			string strFileName = entry.path().filename().string();

			size_t lastDot = entry.path().filename().string().find_last_of(".");

			strFileName = entry.path().filename().string().substr(0, lastDot);

			fbxFilesName.push_back(strFileName);
		}
	}

	for (const auto& fbxNames : fbxFilesName)
	{
		wstring strFilePath = strPath + TEXT("/");

		string strDirectory = m_pGameInstance->WstringToString(strFilePath);
		string strBinPath = strDirectory + "Bin/" + fbxNames + ".dat";

		if (!fs::exists(strBinPath))
		{
			wstring strFbxName = m_pGameInstance->StringToWstring(fbxNames);
			wstring strComponentName = TEXT("Prototype_Component_Model_") + strFbxName;
			wstring strFbxPath = strFilePath + strFbxName + TEXT(".fbx");
			string strTransPath = m_pGameInstance->WstringToString(strFbxPath);


			NonAnimPreTransformMatrix = XMMatrixIdentity();
			if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strTransPath.c_str(), NonAnimPreTransformMatrix, false))))
				return E_FAIL;
		}
		else
		{
			wstring strComponentName = TEXT("Prototype_Component_Model_") + m_pGameInstance->StringToWstring(fbxNames);

			NonAnimPreTransformMatrix = XMMatrixScaling(0.01002f, 0.01002f, 0.01002f);
			if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strBinPath.c_str(), NonAnimPreTransformMatrix, true))))
				return E_FAIL;
		}
	}
	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
