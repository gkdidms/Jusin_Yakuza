#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Client)
class CLandObject abstract :
    public CGameObject
{
protected:
    CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLandObject(const CLandObject& rhs);
    virtual ~CLandObject() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    const wstring& Get_ModelName() {
        return m_wstrModelName;
    }
        
protected:
    class CSystemManager* m_pSystemManager = { nullptr };

    wstring m_wstrModelName = TEXT("");

protected:
    virtual HRESULT Add_Components();
    virtual HRESULT Bind_ResourceData();

public:
    virtual void Free() override;
};
END
