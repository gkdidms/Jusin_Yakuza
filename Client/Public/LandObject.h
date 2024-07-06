#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
END

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
    virtual _bool Intersect(CLandObject* pTargetObject) { return false; }
    virtual void ImpulseResolution(CLandObject* pTargetObject) {};

public:
    const wstring& Get_ModelName() {
        return m_wstrModelName;
    }

    CCollider* Get_Collider() {
        return m_pColliderCom;
    }
 
protected:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CCollision_Manager* m_pCollisionManager = { nullptr };
    CCollider* m_pColliderCom = { nullptr };

    wstring m_wstrModelName = TEXT("");

protected:
    virtual HRESULT Add_Componenets();
    virtual HRESULT Bind_ResourceData();

public:
    virtual void Free() override;
};
END
