#pragma once
#include "UIScene.h"
#include "Client_Defines.h"
BEGIN(Client)
class CUIInven :
    public CUIScene
{
public:
    typedef struct tIvenScenDesc :public CUIScene::SCENE_DESC {
       class CInventoryManager* pInventory;
    }IVENSCENE_DESC;
protected:
    CUIInven();
    CUIInven(const CUIInven& rhs);
    virtual ~CUIInven() = default;

public:

    virtual HRESULT Add_UIData(class CUI_Object* pUIObject) override;

    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

    virtual _bool Click_InterSect() override;
    virtual void Action() override;
    void Toggle_Action();
    virtual void OverAction() override;

private:
    class CInventoryManager* m_pInvenctory = { nullptr };
    vector <class CBtn*> m_Toggle;
    _uint m_iToggle = {0};
public:
    static CUIInven* Create(void* pArg=nullptr);    
    virtual void Free();
};
END
