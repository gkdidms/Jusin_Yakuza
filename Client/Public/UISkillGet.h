#pragma once
#include "UIScene.h"
#include "Client_Defines.h"
BEGIN(Client)
class CUISkillGet :
    public CUIScene
{
protected:
    CUISkillGet();
    CUISkillGet(const CUISkillGet& rhs);
    virtual ~CUISkillGet() = default;
public:

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
    static CUISkillGet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
