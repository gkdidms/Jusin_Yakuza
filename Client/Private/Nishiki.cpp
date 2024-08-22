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
	//니시키야마가 다 걸으면 멈추고 멈추는 순간 레벨 이동한다.
	if (m_iCurrentLevel == LEVEL_NISHIKIWALK)
	{
		if (m_iState == WALK_EN && m_pModelCom->Get_AnimFinished(m_iAnimIndex))
			m_isFinished = true;
	}

	if (m_iCurrentLevel == LEVEL_NISHIKIWALK && m_iState != WALK_EN)
	{
		Move(fTimeDelta);
	}

	Change_Animation(fTimeDelta);

	if (m_iState == TALK)
	{
		//대화중일때 플레이어를 바라보게 하기
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0));
		m_pTransformCom->LookAt(pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
	}

	Synchronize_Root(fTimeDelta);
}

void CNishiki::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

void CNishiki::Change_Animation(const _float fTimeDelta)
{
	m_fOffset = { 1.f };
	m_isAnimLoop = true;

	if (m_iState == IDLE)
		m_iAnimIndex = 2;
	else if (m_iState == TALK)
		m_iAnimIndex = 1;
	else if (m_iState == WALK_ST)
	{
		m_iAnimIndex = 5;
		m_isAnimLoop = false;
	}
	else if (m_iState == WALK_LP)
	{
		m_iAnimIndex = 4;
		m_fOffset = 0.8f;
	}
	else if (m_iState == WALK_EN)
	{
		m_iAnimIndex = 3;
		m_isAnimLoop = false;
	}
	else if (m_iState == CHEER)
		m_iAnimIndex = 0;

	CModel::ANIMATION_DESC Desc{ m_iAnimIndex, m_isAnimLoop};
	m_pModelCom->Set_AnimLoop(m_iAnimIndex, m_isAnimLoop);
	m_pModelCom->Set_AnimationIndex(m_iAnimIndex, m_fChangeInterval);
	if(m_iState == CHEER)
		m_pModelCom->Play_Animation(fTimeDelta * m_fOffset, Desc, false);
	else
		m_pModelCom->Play_Animation(fTimeDelta * m_fOffset, Desc);
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
	if (m_iState == WALK_ST && m_pModelCom->Get_AnimFinished(m_iAnimIndex))
	{
		m_isWalkStartEnd = true;
		m_iState = WALK_LP;
	}
	if (m_iState != WALK_LP)
		m_iState = WALK_ST;

	_bool isFinished = false;
	_vector vDir = m_pNavigationCom->Compute_NishikiDir(m_pTransformCom->Get_State(CTransform::STATE_POSITION), fTimeDelta, &isFinished);

	if (isFinished)
	{
		//끝이났다면?
		m_iState = WALK_EN;
	}
	else
		m_pTransformCom->LookAt_For_LandObject(vDir, true);

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
