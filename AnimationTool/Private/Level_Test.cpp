#include "Level_Test.h"
#include "GameInstance.h"

CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Test::Initialize()
{
	//if (FAILED(Ready_Object(TEXT("Layer_GameObejct"))))
	//	return E_FAIL;

	if (FAILED(Ready_ImGui(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Test::Tick(const _float& fTimeDelta)
{

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("테스트 레벨"));
#endif
}

HRESULT CLevel_Test::Ready_Object(const wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Test::Ready_ImGui(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("GameObject_Prototype_ImGuiManager"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

CLevel_Test* CLevel_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Test* pInstance = new CLevel_Test(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Test");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Test::Free()
{
	__super::Free();
}
