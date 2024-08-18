#pragma once
#include "NPC.h"

BEGIN(Client)
class CNishiki :
    public CNPC
{
public:
    enum NISHIKI_STATE
    {
        IDLE,
        TALK,
        CHEER,
        WALK,
        NISHIKI_STATE_END
    };

private:
    CNishiki(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNishiki(const CNishiki& rhs);
    virtual ~CNishiki() = default;

public:
    void Set_State(_uint iIndex) { m_iState = iIndex; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    virtual void Change_Animation();
    virtual HRESULT Add_Components() override;

private:
    void Move(const _float& fTimeDelta);

public:
    static CNishiki* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
