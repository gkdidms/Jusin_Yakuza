#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CShader;
class CAnim;
class CNavigation;
END

BEGIN(Client)
class CAdventure abstract :
    public CLandObject
{
protected:
    CAdventure(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CAdventure(const CAdventure& rhs);
    virtual ~CAdventure() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    // 충돌함수
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) override;
    virtual void Animation_Event() override;
    virtual string Get_CurrentAnimationName() override;

    _bool Checked_Animation_Ratio(_float fRatio);

protected:
    CAnim* m_pAnimCom = { nullptr }; // 애니메이션만 따로 저장하고있는 애니메이션 컴포넌트
    CNavigation* m_pNavigationCom = { nullptr };
    class CAI_Monster* m_pTree = { nullptr };    

protected:
    _bool m_isAnimLoop = { false };
    _uint m_iState = { 0 };
    _float m_fChangeInterval = { 4.f };

    _float          m_fPrevSpeed;
    _float4         m_vPrevMove;
    _float4         m_vPrevRotation;
    _float4x4       m_ModelWorldMatrix;

protected:
    string m_strAnimName = "";
    _uint m_iAnim = { 0 };

protected:
    virtual void Change_Animation();
    void Synchronize_Root(const _float& fTimeDelta);

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    virtual void Free();
};
END
