#include "Adv_Suit.h"

#include "GameInstance.h"

CAdv_Suit::CAdv_Suit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAdventure { pDevice, pContext }
{
}

CAdv_Suit::CAdv_Suit(const CAdv_Suit& rhs)
	: CAdventure { rhs }
{
}

HRESULT CAdv_Suit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAdv_Suit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CAdv_Suit::Priority_Tick(const _float& fTimeDelta)
{
}

void CAdv_Suit::Tick(const _float& fTimeDelta)
{
}

void CAdv_Suit::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	__super::Late_Tick(fTimeDelta);
}

CAdv_Suit* CAdv_Suit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAdv_Suit* pInstance = new CAdv_Suit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CAdv_Suit::Clone(void* pArg)
{
	CGameObject* pInstance = new CAdv_Suit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAdv_Suit::Free()
{
	__super::Free();
}
