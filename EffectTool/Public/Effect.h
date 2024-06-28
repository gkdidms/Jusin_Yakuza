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
        _float           fStartTime;

        wstring ParticleTag;
        _float4 vStartColor;
        _float4 vEndColor;

        _int iShaderPass;
    }EFFECT_DESC;

    enum TYPE { TYPE_POINT , TYPE_TRAIL, TYPE_END};
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

    virtual void* Get_Instance();
public:
    void Set_StartPos(_float4 Pos) { m_vStartPos = Pos; }
    _uint Get_Type() const { return m_eType; }
    wstring Get_Tag() { return m_ParticleTag; }
    _float* Get_pStartTime() { return &m_fStartTime; }
    _float4 Get_StartPos() { return m_vStartPos; }
    _uint Get_Action() { return m_iAction; }
    _float4 Get_SColor() { return m_vStartColor; }
    _float4 Get_EColor() { return m_vEndColor; }
    _int Get_ShaderPass() { return m_iShaderPass; }

protected:
    
    _uint          m_eType = { TYPE_END };
    wstring     m_ParticleTag = { TEXT("") };
    _float       m_fStartTime = { 0.f };
    _float4     m_vStartPos = {};

    _uint			m_iAction = { 0 };
    _float4     m_vStartColor = { 0.f , 0.f , 0.f , 0.f };
    _float4     m_vEndColor = { 0.f , 0.f , 0.f , 0.f };
    _int          m_iShaderPass = { 0 };


public:
    virtual void Free() override;
};
END
