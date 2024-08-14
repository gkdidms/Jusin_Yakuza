#include "UIKaraoke_Select.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUIKaraoke_Select::CUIKaraoke_Select()
    :CUIScene{}
{
}

CUIKaraoke_Select::CUIKaraoke_Select(const CUIKaraoke_Select& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUIKaraoke_Select::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;
    return S_OK;
}

HRESULT CUIKaraoke_Select::Tick(const _float& fTimeDelta)
{
    if(FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIKaraoke_Select::Late_Tick(const _float& fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;
    return S_OK;
}

void CUIKaraoke_Select::Action()
{
}

void CUIKaraoke_Select::OverAction()
{
}

CUIKaraoke_Select* CUIKaraoke_Select::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CUIKaraoke_Select* pInstance = new CUIKaraoke_Select();
    if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CUIKaraoke_Select::Free()
{
    __super::Free();
}
