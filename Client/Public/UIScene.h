#pragma once
#include "Base.h"

#include "Client_Defines.h"
BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CUIScene :
    public CBase
{

protected:
    CUIScene();
    CUIScene(const CUIScene& rhs);
    virtual ~CUIScene() = default;

public:
    HRESULT Add_UIData(class CUI_Object* pUIObject);
    
    virtual HRESULT Show_Scene();
    virtual HRESULT Close_Scene();

    virtual HRESULT Initialize();
    virtual HRESULT Tick(const _float& fTimeDelta);
    virtual HRESULT Late_Tick(const _float& fTimeDelta);

    virtual void Check_AimFin();

    virtual _bool Click_InterSect();
    virtual void Action(_int EventNum);

public:
    _bool Get_isAnimFinish() { return m_isAnimFin; }
    _bool Get_isClose() { return m_isClose; }

    virtual void Reset() {
        m_isClose = false;  
    }
protected:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };

    vector<class CUI_Object*> m_UI;//모든 ui를 받는곧
    vector<class CBtn*> m_Button;//받은 ui 중 버튼만 모아 둠


    _bool m_isAnimFin = { false };
    _bool m_isClose = { false };

public:
    virtual void Free();
};

END