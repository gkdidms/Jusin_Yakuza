#include "RoadYNG.h"

#include "GameInstance.h"


CRoadYNG::CRoadYNG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CRoadStanding{ pDevice, pContext }
{
}

CRoadYNG::CRoadYNG(const CRoadYNG& rhs)
	: CRoadStanding{ rhs }
{
}

HRESULT CRoadYNG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRoadYNG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CRoadYNG::Priority_Tick(const _float& fTimeDelta)
{
}

void CRoadYNG::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRoadYNG::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	__super::Late_Tick(fTimeDelta);
}

HRESULT CRoadYNG::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	//행동트리 저장
	//CAI_RoadNML::AI_ADVENTURE_DESC AIDesc{};
	//AIDesc.pAnimCom = m_pAnimCom;
	//AIDesc.pState = &m_iState;
	//AIDesc.pThis = this;
	//AIDesc.pAStartCom = m_pAStartCom;

	//m_pTree = dynamic_cast<CAI_RoadNML*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Passersby"), &AIDesc));
	//if (nullptr == m_pTree)
	//	return E_FAIL;

	return S_OK;
}

CRoadYNG* CRoadYNG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoadYNG* pInstance = new CRoadYNG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CRoadYNG::Clone(void* pArg)
{
	CRoadYNG* pInstance = new CRoadYNG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CRoadYNG::Free()
{
	__super::Free();
}
