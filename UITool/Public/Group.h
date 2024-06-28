#pragma once
#include "UI_Object.h"

#include "UITool_Defines.h"

BEGIN(UITool)
class CGroup :
    public CUI_Object
{
private:
    CGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGroup(const CGroup& rhs);
    virtual ~CGroup() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    vector<CUI_Object*> m_PartObjects;

public:
    static CGroup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
