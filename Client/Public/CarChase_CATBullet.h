#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNeoShader;
class CCollider;
END

BEGIN(Client)
class CCarChase_CATBullet :
    public CGameObject
{
public:
    typedef struct tBulletDesc {
        const _float4x4* pParentMatrix;
    }BULLET_DESC;
private:
    CCarChase_CATBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCarChase_CATBullet(const CCarChase_CATBullet& rhs);
    virtual ~CCarChase_CATBullet() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };
    CNeoShader* m_pMaterialCom = { nullptr };
    CCollider* m_pColliderCom = { nullptr };

    class CHighway_Kiryu* m_pTarget = { nullptr };

    const _float4x4* m_pParentMatrix = { nullptr };
    _float4x4 m_WorldMatrix = {};

private:
    _float m_fSpeed = { 10.f };

private:
    HRESULT Add_Components();
    HRESULT Bind_ResourceData();

public:
    static CCarChase_CATBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};

END