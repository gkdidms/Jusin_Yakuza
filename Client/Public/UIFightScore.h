#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIFightScore :
    public CUIScene
{
public:
    enum SCORE{BONUS0 ,BONUS1 , BONUS2 , BACK , SCORE_END};
protected:
    CUIFightScore();
    CUIFightScore(const CUIFightScore& rhs);
    virtual ~CUIFightScore() = default;

public:
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject) override;
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

    virtual _bool Click_InterSect()override;//클릭시충돌
    virtual _bool Over_InterSect()override;//오버시 충동
    virtual void Action() override;
    virtual void OverAction() override;

public:
    void Set_Money();

private:
    vector<CUI_Object*>m_Bonus;
    class CGroup* m_Money = { nullptr };
    class CGroup* m_AddMoney = { nullptr };
    _float m_iCurrentTime = { 0.f };
    _bool m_isEnd = { false };

    _float U = 0.625f;
    _float V = 0.2f;

public:
    static CUIFightScore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};

END