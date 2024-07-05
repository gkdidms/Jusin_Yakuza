#include "SocketEffect.h"
#include "GameInstance.h"
#include "SystemManager.h"

#include "Effect.h"

CSocketEffect::CSocketEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CSocketObject{ pDevice, pContext }
{
}

CSocketEffect::CSocketEffect(const CSocketEffect & rhs)
	: CSocketObject{ rhs }
{
}

HRESULT CSocketEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSocketEffect::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSocketEffect::Priority_Tick(const _float& fTimeDelta)
{
}

void CSocketEffect::Tick(const _float& fTimeDelta)
{
}

void CSocketEffect::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CSocketEffect::Render()
{

	return S_OK;
}

HRESULT CSocketEffect::Add_Components(void* pArg)
{
	if (nullptr != pArg)
	{
		SOKET_EFFECT_DESC* pDesc = static_cast<SOKET_EFFECT_DESC*>(pArg);

		CEffect::EFFECT_DESC Desc{};
		Desc.pWorldMatrix = &m_WorldMatrix;
		
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, pDesc->wstrEffectName, TEXT("Layer_Effect"), &Desc)))
		{
			auto pLayerObjects = m_pGameInstance->Get_GameObjects(LEVEL_TEST, TEXT("Layer_Effect"));
			m_pEffect = reinterpret_cast<CEffect*>(m_pGameInstance->Get_GameObject(LEVEL_TEST, TEXT("Layer_Effect"), pLayerObjects.size()));

			Safe_AddRef(m_pEffect);
		}
	}

	return S_OK;
}

CSocketEffect * CSocketEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSocketEffect*		pInstance = new CSocketEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSocketEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSocketEffect::Clone(void * pArg)
{
	CSocketEffect*		pInstance = new CSocketEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSocketEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSocketEffect::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}
