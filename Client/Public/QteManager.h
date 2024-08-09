#pragma once
#include "GameObject.h"

// 엔진 뭐 필요한게 없는듯?
BEGIN(Engine)
END
class CQteManager :
    public CGameObject
{
    enum QTE_TYPE {
        YONEDA, CARCHASE, QTE_TYPE_END
    };

private:
    CQteManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CQteManager(const CQteManager& rhs);
    virtual ~CQteManager() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    virtual HRESULT Add_Components();
    virtual HRESULT Bind_ResourceData();

private:
    _float  m_fTimeOut = { 2.f };
    _bool   m_bButton = { false };                  // false: Q, true: E

public:
    CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

