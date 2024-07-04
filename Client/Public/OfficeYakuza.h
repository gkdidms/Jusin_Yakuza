#pragma once
#include "Monster.h"

BEGIN(Client)
class COfficeYakuza :
    public CMonster
{
private:
    COfficeYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    COfficeYakuza(const COfficeYakuza& rhs);
    virtual ~COfficeYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    class CAI_OfficeYakuza* m_pTree = { nullptr };

    _bool m_isAnimLoop = { false };

    _float          m_fPrevSpeed = { 0.f };
    _float4         m_vPrevMove;
    _float4         m_vPrevRotation;
    _float4x4       m_ModelWorldMatrix;

private:
    virtual HRESULT Add_Componenets() override;
    virtual HRESULT Bind_ResourceData() override;

private:
    void Synchronize_Root(const _float& fTimeDelta);
    void Change_Animation();

public:
    static COfficeYakuza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
