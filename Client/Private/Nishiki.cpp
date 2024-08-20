#include "Nishiki.h"

#include "GameInstance.h"

#include "Player.h"

CNishiki::CNishiki(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNPC{ pDevice, pContext }
{
}

CNishiki::CNishiki(const CNishiki& rhs)
	: CNPC{ rhs }
{
}

HRESULT CNishiki::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNishiki::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iObjectIndex = 101;

	if (m_iCurrentLevel == LEVEL_KARAOKE)
		m_iState = CHEER;
	else
		m_iState = IDLE;

	m_isAnimLoop = true;

	return S_OK;
}

void CNishiki::Priority_Tick(const _float& fTimeDelta)
{
}

void CNishiki::Tick(const _float& fTimeDelta)
{
	if (m_iCurrentLevel == LEVEL_NISHIKIWALK)
		Move(fTimeDelta);
	
	Change_Animation();

	if (m_iState == TALK)
	{
		//대화중일때 플레이어를 바라보게 하기
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0));
		m_pTransformCom->LookAt(pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
	}

	m_pModelCom->Play_Animation(fTimeDelta, m_isAnimLoop);

	//Synchronize_Root(fTimeDelta);
}

void CNishiki::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

void CNishiki::Change_Animation()
{
	_uint iAnim = { 0 };

	if (m_iState == IDLE)
		iAnim = 2;
	else if (m_iState == TALK)
		iAnim = 1;
	else if (m_iState == WALK)
		iAnim = 3;
	else if (m_iState == CHEER)
		iAnim = 0;

	m_pModelCom->Set_AnimationIndex(iAnim, m_fChangeInterval);
}

HRESULT CNishiki::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Nishiki"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Material_Nishiki"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	if (m_iCurrentLevel == LEVEL_NISHIKIWALK)
	{
		CNavigation::NAVIGATION_DESC Desc{};
		Desc.iCurrentLine = m_iNaviRouteNum;
		Desc.iCurrentRouteDir = DIR_F;
		Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), nullptr)))
			return E_FAIL;
	}

	return S_OK;
}

void CNishiki::Move(const _float& fTimeDelta)
{
	_bool isTurn = false;
	_int iDir = false;
	_bool isBack = false;
	_vector vDir = m_pNavigationCom->Compute_WayPointDir_Adv(m_pTransformCom->Get_State(CTransform::STATE_POSITION), fTimeDelta, &isTurn, &iDir, &isBack);

	m_pTransformCom->LookAt_For_LandObject(vDir);
}

CNishiki* CNishiki::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNishiki* pInstance = new CNishiki(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CNishiki::Clone(void* pArg)
{
	CNishiki* pInstance = new CNishiki(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNishiki::Free()
{
	__super::Free();
}
