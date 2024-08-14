#include "RoadWalker.h"

#include "GameInstance.h"

#include "AI_Adventure.h"

CRoadWalker::CRoadWalker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAdventure{pDevice, pContext }
{
}

CRoadWalker::CRoadWalker(const CRoadWalker& rhs)
	: CAdventure{ rhs }
{
}

HRESULT CRoadWalker::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRoadWalker::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CRoadWalker::Priority_Tick(const _float& fTimeDelta)
{
}

void CRoadWalker::Tick(const _float& fTimeDelta)
{
	m_pTree->Tick(fTimeDelta);

	Change_Animation();

	m_pModelCom->Play_Animation(fTimeDelta * m_fOffset, m_pAnimCom, m_isAnimLoop);

	_vector vDir = m_pNavigationCom->Compute_WayPointDir_Adv(m_pTransformCom->Get_State(CTransform::STATE_POSITION), fTimeDelta);
	m_pTransformCom->LookAt_For_LandObject(vDir, true);
	//m_pTransformCom->Go_Straight_CustumSpeed(m_fSpeed, fTimeDelta, m_pNavigationCom);

	Synchronize_Root(fTimeDelta);
}

void CRoadWalker::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CRoadWalker::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CRoadWalker::Change_Animation()
{
}

void CRoadWalker::Free()
{
	__super::Free();
}
