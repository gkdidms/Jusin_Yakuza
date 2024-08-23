#pragma once
#include "Trigger.h"

/*
���������� ��״��� ��ũ �׼��� ���´�.
��״� Ʈ���ſ� ID ���� �ʿ��� -> ���� �ʿ�
Į / �ƴ��� ����
*/

BEGIN(Client)
class CYonedaTrigger :
    public CTrigger
{
private:
    CYonedaTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CYonedaTrigger(const CYonedaTrigger& rhs);
    virtual ~CYonedaTrigger() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    virtual bool				Move_Scene(int& iLevelNum);

private:
    HRESULT Create_Yoneda();

private:
    _bool m_isColl = { false };
    _uint m_iCollCount = { 0 };

private:
    virtual HRESULT Add_Components(void* pArg) override;

public:
    static CYonedaTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END