#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CAnim;
class CNavigation;
END

BEGIN(Client)
class CCarChase_Reactor abstract :
    public CLandObject
{
public:
    typedef struct tMapHighwayObjDesc : public CGameObject::GAMEOBJECT_DESC
    {
        XMMATRIX		vStartPos;
        wstring			wstrModelName;
        _int				iShaderPass;
        _int             iNaviNum;
        _int             iNaviRouteNum;
        _uint iStageDir;
        _uint iLineDir;
        
        _int iMonsterWeaponType[2];
    }HIGHWAY_IODESC;

protected:
    CCarChase_Reactor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCarChase_Reactor(const CCarChase_Reactor& rhs);
    virtual ~CCarChase_Reactor() = default;

public:
    virtual string Get_CurrentAnimationName();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

protected:
    CAnim* m_pAnimCom = { nullptr }; // 애니메이션만 따로 저장하고있는 애니메이션 컴포넌트
    CNavigation* m_pNavigationCom = { nullptr };

protected:
    _bool m_isAnimLoop = { false };
    _bool m_isStop = { false };
    _uint m_iState = { 0 };
    _float m_fChangeInterval = { 4.f };

    string  m_strAnimName = "";
    _uint   m_iAnim = { 0 };

    _bool   m_isColl = { false };
    
protected:
    _uint m_iStageDir = { DIR_END };
    _uint m_iLineDir = { DIR_END };

    _int     m_iNaviRouteNum = { 0 };
    _float m_fSpeed = { 40.f };


protected:
    virtual HRESULT Ready_Monster(_int* pMonsterTypes) = 0;
    virtual void Change_Animation();
    HRESULT Setup_Animation();

protected:
    void Move_Waypoint(const _float& fTimeDelta);

protected:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    virtual void Free();
};
END
