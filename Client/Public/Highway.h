#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CAnim;
class CNavigation;
END

BEGIN(Client)
class CHighway abstract :
    public CLandObject
{
public:
    typedef struct tMapHighwayObjDesc : public CGameObject::GAMEOBJECT_DESC
    {
        XMMATRIX		vStartPos;
        wstring			wstrModelName;
        int				iShaderPass;
        int             iNaviNum;
        int             iNaviRouteNum;
    }HIGHWAY_IODESC;

protected:
    CHighway(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CHighway(const CHighway& rhs);
    virtual ~CHighway() = default;

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
    CAnim* m_pAnimCom = { nullptr }; // �ִϸ��̼Ǹ� ���� �����ϰ��ִ� �ִϸ��̼� ������Ʈ
    CNavigation* m_pNavigationCom = { nullptr };
    class CAI_Adventure* m_pTree = { nullptr };

protected:
    _bool m_isAnimLoop = { false };
    _uint m_iState = { 0 };
    _float m_fChangeInterval = { 4.f };

    _float          m_fPrevSpeed;
    _float4         m_vPrevMove;
    _float4         m_vPrevRotation;
    _float4x4       m_ModelWorldMatrix;

protected:
    string  m_strAnimName = "";
    _uint   m_iAnim = { 0 };

    _bool   m_isColl = { false };

    int     m_iNaviRouteNum = { 0 };

protected:
    virtual void Change_Animation();
    HRESULT Setup_Animation();
    void Synchronize_Root(const _float& fTimeDelta);

protected:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    virtual void Free();
};
END
