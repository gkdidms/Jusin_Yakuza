#pragma once
#include "Monster.h"

/*
* Ư�� ��ų�� ����ϴ� ���Ͱ� �ƴ�
* �⺻���� ���� ��ų�� ����ϴ� ����
*/
BEGIN(Client)
class CDefaultYakuza :
    public CMonster
{
private:
    CDefaultYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDefaultYakuza(const CDefaultYakuza& rhs);
    virtual ~CDefaultYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    virtual HRESULT Add_Components() override;
    virtual void Change_Animation() override;

public:
    static CDefaultYakuza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
