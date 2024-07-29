#pragma once
#include "UIScene.h"
#include "Client_Defines.h"
BEGIN(Client)
class CUISkillHolligan :
    public CUIScene
{
protected:
    CUISkillHolligan();
    CUISkillHolligan(const CUISkillHolligan& rhs);
    virtual ~CUISkillHolligan() = default;
public:
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject);

    virtual HRESULT Show_Scene()override;//ui �ִ� �غ�(�ʱ�ȭ/���� ������ ����)
    virtual HRESULT Close_Scene()override;//ui �ִ� �غ�(�ʱ�ȭ/������ �ݴ������)

    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    virtual _bool Click_InterSect()override;//Ŭ�����浹
    virtual _bool Over_InterSect()override;//������ �浿
    virtual void Action() override;
    virtual void OverAction() override;


public:
    void Set_Skill(_int Index);

private:
    vector < CUI_Object*> m_pBall;
    _bool m_isSkill[8] = { true, false, };
public:
    static CUISkillHolligan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END