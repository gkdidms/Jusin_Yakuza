#pragma once

#include "GameObject.h"

#include "Client_Defines.h"
#include "VIBuffer_Instance_Point.h" 

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Point;
END

BEGIN(Client)
class CParticle_Point final:
    public CGameObject
{
public:
    typedef struct tPARTICLE_POINT_DESC :public CGameObject::GAMEOBJECT_DESC
    {
        _float4			vStartPos;
        CVIBuffer_Instance_Point::INSTANCE_POINT_DESC BufferInstance;
    }PARTICLE_POINT_DESC;
private:
    CParticle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CParticle_Point(const CParticle_Point& rhs);
    virtual ~CParticle_Point() = default;


public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Tick(const _float& fTimeDelta);
    virtual void Tick(const _float& fTimeDelta);
    virtual void Late_Tick(const _float& fTimeDelta);
    virtual HRESULT Render();


private:
    CShader* m_pShaderCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Instance_Point* m_pVIBufferCom = { nullptr };
    CVIBuffer_Instance_Point::INSTANCE_POINT_DESC* m_pBufferInstance;


private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();

public:
    static CParticle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
