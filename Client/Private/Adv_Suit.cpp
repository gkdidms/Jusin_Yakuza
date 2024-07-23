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


	if (nullptr != pArg)
	{
		ADVENTURE_IODESC* gameobjDesc = (ADVENTURE_IODESC*)pArg;
		m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
		m_wstrModelName = gameobjDesc->wstrModelName;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		ADVENTURE_IODESC* gameobjDesc = (ADVENTURE_IODESC*)pArg;

		m_pNavigationCom->Set_Index(gameobjDesc->iNaviNum);
	}

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

HRESULT CAdv_Suit::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.5, 0.8, 0.5);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
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
