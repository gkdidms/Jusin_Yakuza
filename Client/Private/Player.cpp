#include "Player.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Componenets()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Priority_Tick(const _float& fTimeDelta)
{
}

void CPlayer::Tick(const _float& fTimeDelta)
{
}

void CPlayer::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_Componenets()
{
	return S_OK;
}

HRESULT CPlayer::Bind_ResourceData()
{
	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();
}
