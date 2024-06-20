#pragma once
#include "BlendObject.h"

#include "Client_Defines.h"
#include "VIBuffer_Instance.h"

BEGIN(Engine)
class CVIBuffer_Instance;
END


BEGIN(Client)
class CEffect abstract:
    public CBlendObject
{
public:
    typedef struct tEFFECT_DESC :public CBlendObject::GAMEOBJECT_DESC
    {
        _uint eType;
        _float4			vStartPos;
        CVIBuffer_Instance::INSTANCE_DESC BufferInstance;   
    }EFFECT_DESC;
    enum TYPE { TYPE_POINT , TYPE_RECT ,TYPE_ANIM , TYPE_END};

protected:
    CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CEffect(const CEffect& rhs);
    virtual ~CEffect() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg)override;
    virtual void Priority_Tick(const _float& fTimeDelta)override;
    virtual void Tick(const _float& fTimeDelta)override;
    virtual void Late_Tick(const _float& fTimeDelta)override;
    virtual HRESULT Render()override;


    CVIBuffer_Instance::INSTANCE_DESC* Get_Instance() {
        return &m_BufferInstance;
    }
public:
    _uint Get_Type() const { return m_eType; }
protected:
    CVIBuffer_Instance::INSTANCE_DESC m_BufferInstance; 
    _uint m_eType = { TYPE_END };
public:
    virtual void Free() override;
};
END
