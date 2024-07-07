#pragma once
#include "Monster.h"

BEGIN(Client)
class CRushYakuza :
    public CMonster
{
private:
    CRushYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRushYakuza(const CRushYakuza& rhs);
    virtual ~CRushYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    class CAI_RushYakuza* m_pTree = { nullptr };

private:
    virtual HRESULT Add_Componenets() override;
    virtual HRESULT Bind_ResourceData() override;

private:
    void Change_Animation();

public:
    static CRushYakuza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
