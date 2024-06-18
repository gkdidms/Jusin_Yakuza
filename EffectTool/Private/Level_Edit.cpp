#include "Level_Edit.h"

CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Edit::Initialize()
{
	if (FAILED(Ready_Object(TEXT("Layer_EditObject"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Edit::Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("제작 레벨"));
#endif
}

HRESULT CLevel_Edit::Ready_Object(const wstring& strLayerTag)
{


	return S_OK;
}

CLevel_Edit* CLevel_Edit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Edit* pInstance = new CLevel_Edit(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Edit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Edit::Free()
{
	__super::Free();
}
