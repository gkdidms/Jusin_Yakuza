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

HRESULT CUIKaraoke_Select::Add_UIData(CUI_Object* pUIObject)
{

    if (pUIObject->Get_Event())
    {
        if (nullptr == m_Disc)
            m_Disc = pUIObject;
        else if (m_UIGrade.size() < 3)
            m_UIGrade.push_back(pUIObject);
        else if (nullptr == m_Select)
            m_Select = static_cast<CGroup*>(pUIObject);
        else if (m_UIName.size() < 3)
            m_UIName.push_back(pUIObject);
        else
            m_EventUI.push_back(pUIObject);

        return S_OK;
    }
    else
    {
        m_UI.push_back(pUIObject);
    }

    if (CUI_Object::TYPE_BTN == pUIObject->Get_TypeIndex())
    {
        Safe_AddRef(pUIObject);
        m_Button.push_back(dynamic_cast<CBtn*>(pUIObject));
    }


    return S_OK;
}


HRESULT CUIKaraoke_Select::Tick(const _float& fTimeDelta)
{
    m_Disc->Tick(fTimeDelta);
    if(FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    for (auto& iter : m_EventUI)
        iter->Tick(fTimeDelta);
    for (auto& iter : m_UIGrade)
        iter->Tick(fTimeDelta);
    for (auto& iter : m_UIName)
        iter->Tick(fTimeDelta);


    m_Select->Tick(fTimeDelta);

    return S_OK;
}

HRESULT CUIKaraoke_Select::Late_Tick(const _float& fTimeDelta)
{
    m_Disc->Late_Tick(fTimeDelta);
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    for (auto& iter : m_EventUI)
        iter->Late_Tick(fTimeDelta);
    for (auto& iter : m_UIGrade)
        iter->Late_Tick(fTimeDelta);
    for (auto& iter : m_UIName)
        iter->Late_Tick(fTimeDelta);

    m_Select->Late_Tick(fTimeDelta);

    if (!m_isAnimFin)
        Check_AimFin();

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
