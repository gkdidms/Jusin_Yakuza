#include "UILogo.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUILogo::CUILogo()
	:CUIScene{}
{
}

CUILogo::CUILogo(const CUILogo& rhs)
	:CUIScene{ rhs }
{
}


HRESULT CUILogo::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CUILogo::Tick(const _float& fTimeDelta)
{


	__super::Tick(fTimeDelta);



	return S_OK;
}

HRESULT CUILogo::Late_Tick(const _float& fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);


	//등장 애니메이션 끝난뒤 모든 행동 코드

	if (!m_isAnimFin)
		Check_AimFin();

	return S_OK;
}

void CUILogo::Action()
{
}

void CUILogo::OverAction()
{
}

CUILogo* CUILogo::Create(void* pArg)
{
	CUILogo* pInstance = new CUILogo();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUILogo::Free()
{
	__super::Free();
}
