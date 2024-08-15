#include "UIKaraoke_Play.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUIKaraoke_Play::CUIKaraoke_Play()
    :CUIScene{}
{
}

CUIKaraoke_Play::CUIKaraoke_Play(const CUIKaraoke_Play& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUIKaraoke_Play::Show_Scene()
{
    if (FAILED(__super::Show_Scene()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIKaraoke_Play::Close_Scene()
{
    if (FAILED(__super::Close_Scene()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIKaraoke_Play::Add_UIData(CUI_Object* pUIObject)
{
    if (pUIObject->Get_Event())
    {
        if (m_pPlayUI.size() < 13)
            m_pPlayUI.push_back(pUIObject);
        else
            m_Lyrics=dynamic_cast<CGroup*>(pUIObject);
        return S_OK;
    }


    return S_OK;
}

HRESULT CUIKaraoke_Play::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIKaraoke_Play::Tick(const _float& fTimeDelta)
{

    for (auto& iter : m_pPlayUI)
        iter->Tick(fTimeDelta);
    m_Lyrics->Tick(fTimeDelta);
    return S_OK;
}

HRESULT CUIKaraoke_Play::Late_Tick(const _float& fTimeDelta)
{


    for (auto& iter : m_pPlayUI)
        iter->Late_Tick(fTimeDelta);
    m_Lyrics->Late_Tick(fTimeDelta);
    if (!m_isAnimFin)
        Check_AimFin();

    return S_OK;
}

_bool CUIKaraoke_Play::Click_InterSect()
{
    return _bool();
}

_bool CUIKaraoke_Play::Over_InterSect()
{
    return _bool();
}

void CUIKaraoke_Play::Action()
{
}

void CUIKaraoke_Play::OverAction()
{
}

CUIKaraoke_Play* CUIKaraoke_Play::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CUIKaraoke_Play* pInstance = new CUIKaraoke_Play();
    if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CUIKaraoke_Play::Free()
{
    __super::Free();
    for (auto& iter : m_pPlayUI)
        Safe_Release(iter);
    Safe_Release(m_Lyrics);
}
