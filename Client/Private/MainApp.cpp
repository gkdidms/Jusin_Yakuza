#include "../Default/framework.h"

#include "MainApp.h"
#include "UIManager.h"
#include "GameInstance.h"
#include "SystemManager.h"
#include "Collision_Manager.h"

#ifdef _DEBUG
#include "DebugManager.h"
#endif // _DEBUG

#include "Level_Loading.h"
#include "Background.h"
#include "FileTotalMgr.h"

#pragma region UI
#include "Image_Texture.h"
#include "Text.h"
#include "Group.h"
#include "Btn.h"
#include "UI_Effect.h"
#pragma endregion

CMainApp::CMainApp() :
#ifdef _DEBUG
	m_pDebugMananger{ CDebugManager::GetInstance() },
#endif // _DEBUG
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pFileTotalManager{ CFileTotalMgr::GetInstance() },
	m_pCollisionManager{ CCollision_Manager::GetInstance() },
	m_pUIManager{ CUIManager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pFileTotalManager);
	Safe_AddRef(m_pCollisionManager);
	Safe_AddRef(m_pUIManager);

#ifdef _DEBUG
	Safe_AddRef(m_pDebugMananger);
#endif // _DEBUG
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.hInst = g_hInst;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowed = true;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(m_pUIManager->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Font()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pDebugMananger->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;

	m_pGameInstance->Set_DebugView(true);
#endif // _DEBUG


	return S_OK;
}

void CMainApp::Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Tick(fTimeDelta);
	m_pCollisionManager->Tick();
	m_pUIManager->Tick(fTimeDelta);
	m_pUIManager->Late_Tick(fTimeDelta);

	//������ Ȯ�ο� ���߿� �ٽ� ����׷� �־����
	m_fTimeAcc += fTimeDelta;

#ifdef _DEBUG
	if (m_pGameInstance->Get_CurrentLevel() > 2)
	{
		if (m_pGameInstance->GetKeyState(DIK_1) == TAP)
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_OFFICE_1F))))
				return;
		}
		if (m_pGameInstance->GetKeyState(DIK_2) == TAP)
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_OFFICE_2F))))
				return;
		}
		if (m_pGameInstance->GetKeyState(DIK_3) == TAP)
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_OFFICE_BOSS))))
				return;
		}
		if (m_pGameInstance->GetKeyState(DIK_4) == TAP)
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO))))
				return;
		}
		if (m_pGameInstance->GetKeyState(DIK_5) == TAP)
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO_STAIRS))))
				return;
		}
		if (m_pGameInstance->GetKeyState(DIK_6) == TAP)
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO_LOBBY))))
				return;
		}
		if (m_pGameInstance->GetKeyState(DIK_7) == TAP)
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO_BOSS))))
				return;
		}
	}
	if (m_pGameInstance->GetKeyState(DIK_F6) == TAP)
	{
		m_pFileTotalManager->Load_Cinemachine(0, LEVEL_TEST);
	}
	if (m_pGameInstance->GetKeyState(DIK_F7) == TAP)
	{
		m_pSystemManager->Set_Camera(m_pSystemManager->Get_Camera() == CAMERA_DEBUG ? CAMERA_PLAYER : CAMERA_DEBUG);
	}
	if (m_pGameInstance->GetKeyState(DIK_F8) == TAP)
	{
		m_pGameInstance->Set_DebugView(!m_pGameInstance->isDebugView());
	}
	if (m_pGameInstance->GetKeyState(DIK_F10) == TAP)
	{
		m_isDebug = !m_isDebug;
		m_pDebugMananger->Set_Debug(m_isDebug);
	}
	if (m_pGameInstance->GetKeyState(DIK_F9) == TAP)
	{
		Render_Colsole(true);
	}

		
	if (m_isDebug) m_pDebugMananger->Tick();
#endif // _DEBUG
}

HRESULT CMainApp::Render()
{
	/* �׸���. */
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();

#ifdef _DEBUG
	//������ Ȯ�ο�.
	++m_iNumRender;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumRender);

		m_fTimeAcc = 0.f;
		m_iNumRender = 0;
	}

	m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));

	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("!!!�׽�Ʈ�� Ű�� ȭ�鿡 ��� �ۼ��Ұ� !!! "), _float2(500.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("!!!ó������->TEST , �̾��ϱ� ->OFFICE_1F!!! "), _float2(500.f, 20.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("!!!1~7 Office 1�� ~ ������ // ������ test level�̳� office1�� ���� ��"), _float2(500.f, 40.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
	_float fSize = 320.f;
	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("!!!ī�޶� ����Ű"), _float2(1000.f, fSize + 120.f), XMVectorSet(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("F6 : Scene Camera Test"), _float2(1000.f, fSize + 140.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("F7 : Camera Change"), _float2(1000.f, fSize + 160.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));

	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("!!!�� ��"), _float2(1000.f, fSize + 180.f), XMVectorSet(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("F8 : DebugView On/Off"), _float2(1000.f, fSize + 200.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("F9 : Console"), _float2(1000.f, fSize + 220.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("F10 : Debug Tool"), _float2(1000.f, fSize + 240.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("TAP : Camera Pos Fix"), _float2(1000.f, fSize + 260.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("Z : Collision Mgr Console Debug"), _float2(1000.f, fSize + 280.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));


	/* �÷��̾�/ ���� �� �׽�Ʈ Ű �ۼ� */

	if (m_isDebug) m_pDebugMananger->Render();
#endif // _DEBUG

	m_pGameInstance->Present();


	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_nanum24"), TEXT("../../Client/Bin/Resources/Fonts/nanum24.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../../Client/Bin/Resources/Fonts/nanum28.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_nanum30"), TEXT("../../Client/Bin/Resources/Fonts/nanum30.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_nanum36"), TEXT("../../Client/Bin/Resources/Fonts/nanum36.spritefont"))))
		return E_FAIL;
    return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Image_Texture */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Image_Texture"),
		CImage_Texture::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Text */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Text"),
		CText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Group */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Group"),
		CGroup::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Btn */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Btn"),
		CBtn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIEffect */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_UIEffect"),
		CUI_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	//������ ������ �ε�
	if (FAILED(Add_UI_On_Path(TEXT("../../Client/Bin/DataFiles/UIData/"))))
		return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxUI */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxUI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Loading */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default0.dds")))))
		return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Add_UI_On_Path(const wstring& strPath)
{
	vector<wstring> vecDirectorys;
	m_pGameInstance->Get_DirectoryName(strPath, vecDirectorys);

	CUIManager* m_pUIManager = CUIManager::GetInstance();

	for (auto& strChannelName : vecDirectorys)
	{
		wstring strFilePath = strPath + strChannelName + TEXT("/");
		string strDirectory = m_pGameInstance->WstringToString(strFilePath);


		wstring ProtoFrontName = strChannelName + TEXT("_");

		if (ProtoFrontName[0] == TEXT('1'))
			continue;
		for (const auto& entry : fs::directory_iterator(strDirectory))
		{

			string FileName = entry.path().filename().string();
			string AllPath = strDirectory + FileName;

			string Tag;
			_int dotPos = FileName.find_last_of(".");
			Tag = FileName.substr(0, dotPos);


			ifstream in(AllPath, ios::binary);

			if (!in.is_open()) {
				MSG_BOX("���� ����");
				return E_FAIL;
			}
			_uint Type;

			in.read((char*)&Type, sizeof(_uint));

			switch (Type)
			{
			case 0:
			{
				/* For.Prototype_GameObject_Image_Texture */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(ProtoFrontName + m_pGameInstance->StringToWstring(Tag),
					CImage_Texture::Create(m_pDevice, m_pContext, in))))
					return E_FAIL;

				if (FAILED(m_pUIManager->Add_Data(strChannelName, ProtoFrontName + m_pGameInstance->StringToWstring(Tag))))
					return E_FAIL;
			}
			break;

			case 1:
			{

				/* For.Prototype_GameObject_Btn */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(ProtoFrontName + m_pGameInstance->StringToWstring(Tag),
					CBtn::Create(m_pDevice, m_pContext, in))))
					return E_FAIL;

				if (FAILED(m_pUIManager->Add_Data(strChannelName, ProtoFrontName + m_pGameInstance->StringToWstring(Tag))))
					return E_FAIL;
			}
			break;

			case 2:
			{

				/* For.Prototype_GameObject_Text */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(ProtoFrontName + m_pGameInstance->StringToWstring(Tag),
					CText::Create(m_pDevice, m_pContext, in))))
					return E_FAIL;

				if (FAILED(m_pUIManager->Add_Data(strChannelName, ProtoFrontName + m_pGameInstance->StringToWstring(Tag))))
					return E_FAIL;
			}
			break;

			case 3:
			{


				/* For.Prototype_GameObject_Group */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(ProtoFrontName + m_pGameInstance->StringToWstring(Tag),
					CGroup::Create(m_pDevice, m_pContext, in))))
					return E_FAIL;

				if (FAILED(m_pUIManager->Add_Data(strChannelName, ProtoFrontName + m_pGameInstance->StringToWstring(Tag))))
					return E_FAIL;
			}
			break;

			case 4:
			{

				/* For.Prototype_GameObject_UIEffect */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(ProtoFrontName + m_pGameInstance->StringToWstring(Tag),
					CUI_Effect::Create(m_pDevice, m_pContext, in))))
					return E_FAIL;

				if (FAILED(m_pUIManager->Add_Data(strChannelName, ProtoFrontName + m_pGameInstance->StringToWstring(Tag))))
					return E_FAIL;
			}
			break;

			default:
				break;
			}
		}
	}

	return S_OK;
}

void CMainApp::Render_Colsole(_bool isOpen)
{
#ifdef _DEBUG
	if (::AllocConsole() == isOpen)
	{
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}
#endif // _DEBUG
}

CMainApp* CMainApp::Create()
{
    CMainApp* pInstance = new CMainApp();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : CMainApp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	/* ���۷��� ī��Ʈ�� 0���θ����. */
	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();

#ifdef _DEBUG
	Safe_Release(m_pDebugMananger);
	CDebugManager::Release_Debug();
#endif // _DEBUG

	Safe_Release(m_pSystemManager);
	CSystemManager::DestroyInstance();

	Safe_Release(m_pUIManager);
	CUIManager::DestroyInstance();

	Safe_Release(m_pFileTotalManager);
	CFileTotalMgr::DestroyInstance();

	Safe_Release(m_pCollisionManager);
	CCollision_Manager::DestroyInstance();

}
