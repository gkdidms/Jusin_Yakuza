#pragma once
#include "Monster.h"

BEGIN(Client)
class CYoneda :
    public CMonster
{
private:
    CYoneda(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CYoneda(const CYoneda& rhs);
    virtual ~CYoneda() = default;
    
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    class CAI_Yoneda* m_pTree = { nullptr };

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

private:
    void Change_Animation();

public:
    static CYoneda* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};
END
