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
    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

    virtual void Action() override;
    virtual void OverAction() override;

private:
    class CInventoryManager* m_pInvenctory = { nullptr };

public:
    static CUIInven* Create(void* pArg=nullptr);    
    virtual void Free();
};
END
