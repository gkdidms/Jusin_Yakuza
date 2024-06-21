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
        CVIBuffer_Instance::INSTANCE_DESC BufferInstance;   
        _uint eType;
        _float4			vStartPos;
        _float           fStartTime;
        _bool           bDir;
        wstring ParticleTag;
    }EFFECT_DESC;
    enum TYPE { TYPE_POINT , TYPE_RECT ,TYPE_ANIM , TYPE_END};
    enum ACTION { ACTION_SPREAD, ACTION_DROP, ACTION_END };
    static const _uint iAction[ACTION_END];

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
public:
    HRESULT Edit_Action(ACTION iEditAction);

    CVIBuffer_Instance::INSTANCE_DESC* Get_Instance() {
        return &m_BufferInstance;
    }
public:
    void Set_isDir(_bool bDir) { m_bDirInfluence = bDir; }
    void Set_StartPos(_float4 Pos) { m_vStartPos = Pos; }
    _uint Get_Type() const { return m_eType; }
    _bool Get_isDir() {  return m_bDirInfluence;    }
    wstring Get_Tag() { return m_ParticleTag; }
    _float* Get_pStartTime() { return &m_fStartTime; }
    _float4 Get_StartPos() { return m_vStartPos; }
    _uint Get_Action() { return m_iAction; }
protected:
    CVIBuffer_Instance::INSTANCE_DESC m_BufferInstance; 
    _uint m_eType = { TYPE_END };
    _bool m_bDirInfluence = { false };
    wstring m_ParticleTag = { TEXT("") };
    _float m_fStartTime = { 0.f };
    _float4 m_vStartPos = {};
    _float m_fCurTime = { 0.f };
    _uint							m_iAction = { 0 };
public:
    virtual void Free() override;
};
END
