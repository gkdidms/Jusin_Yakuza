#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CPlayer :
    public CLandObject
{
private:
    const _float ANIM_INTERVAL = 7.f;
    
private:
    CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() = default;
    
#ifdef _DEBUG
public:
    _bool isAnimStart() { return m_isAnimStart; }

public:
    void Set_AnimStart(_bool isAnim) { m_isAnimStart = isAnim; }
#endif // DEBUG


public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    void Synchronize_Root();

private:
    CShader*                m_pShaderCom = { nullptr };
    CModel*                 m_pModelCom = { nullptr };
    vector<class CSoketCollider*>      m_pColliders;
    class CCharacterData*   m_pData = { nullptr };

private:
    _uint       m_iAnimIndex = { 2 };
    _uint       m_iTemp = { 1 };

    _float4     m_vPrevMove;
    _float4x4   m_ModelWorldMatrix;

#ifdef _DEBUG
    _bool m_isAnimStart = { true };
#endif // _DEBUG

private:
    virtual HRESULT Add_Componenets() override;
    virtual HRESULT Bind_ResourceData() override;
    HRESULT Add_CharacterData();
    void Apply_ChracterData();

private:
    void Change_Animation(_uint iAnimIndex);

public:
    static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};
END
