#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)
class CHighway_Taxi :
    public CLandObject
{
public:
    typedef struct tHighWayTaxiDesc : public CLandObject::GAMEOBJECT_DESC {
        XMMATRIX		vStartPos;
        _int             iNaviNum;
        _int             iNaviRouteNum;
    } HIGHWAY_TEXI_DESC;

private:
    CHighway_Taxi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CHighway_Taxi(const CHighway_Taxi& rhs);
    virtual ~CHighway_Taxi() = default;

public:
    void Set_NavigationRouteIndex(_uint iLine);
    void Set_Dir(_uint iStageDir) { 
        m_iStageDir = iStageDir;
    }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    // CLandObject��(��) ���� ��ӵ�
    string Get_CurrentAnimationName() override;

protected:
    class CNavigation* m_pNavigationCom = { nullptr };

protected:
    _int     m_iNaviRouteNum = { 0 }; // ���Ͱ� �̵��ϴ� �׺���̼� ��ġ
    _uint m_iStageDir = { DIR_END }; // �������� ��ġ (��, ��, ��, ��)

protected:
    virtual void Change_Animation();
    void Move_Waypoint(const _float& fTimeDelta);

protected:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    static CHighway_Taxi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();


};
END
