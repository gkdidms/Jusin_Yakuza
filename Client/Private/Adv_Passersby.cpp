#include "Adv_Passersby.h"

#include "GameInstance.h"

#include "AI_Passersby.h"

CAdv_Passersby::CAdv_Passersby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAdventure { pDevice, pContext }
{
}

CAdv_Passersby::CAdv_Passersby(const CAdv_Passersby& rhs)
	: CAdventure { rhs }
{
}

HRESULT CAdv_Passersby::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAdv_Passersby::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	ADVENTURE_IODESC* gameobjDesc = static_cast<ADVENTURE_IODESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
	m_wstrModelName = gameobjDesc->wstrModelName;
	m_iNaviRouteNum = gameobjDesc->iNaviRouteNum;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pNavigationCom->Set_Index(gameobjDesc->iNaviNum);
	m_pModelCom->Set_AnimationIndex(0);

	return S_OK;
}

void CAdv_Passersby::Priority_Tick(const _float& fTimeDelta)
{
}

void CAdv_Passersby::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CAdv_Passersby::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	__super::Late_Tick(fTimeDelta);
}

HRESULT CAdv_Passersby::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	//행동트리 저장
	CAI_Passersby::AI_ADVENTURE_DESC AIDesc{};
	AIDesc.pAnimCom= m_pAnimCom;
	AIDesc.pState = &m_iState;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_Passersby*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Passersby"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

CAdv_Passersby* CAdv_Passersby::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAdv_Passersby* pInstance = new CAdv_Passersby(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CAdv_Passersby::Clone(void* pArg)
{
	CGameObject* pInstance = new CAdv_Passersby(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAdv_Passersby::Free()
{
	__super::Free();
}
