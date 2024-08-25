#pragma once
#include "NPC.h"

/*
������ ���� ���°� ���Ѵ�.
Ű���� ��ٸ��� �ִ� ��� -> IDLE
Ű���� ��ȭ���� �� -> Talk
Ű���� �뷡�� �θ��� ���� �� -> Cheer
�ȴ� �������� �� ��� -> walk
*/
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
        WALK_ST,
        WALK_LP,
        WALK_EN,
        NISHIKI_STATE_END
    };

private:
    CNishiki(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNishiki(const CNishiki& rhs);
    virtual ~CNishiki() = default;

public:
    void Set_State(_uint iIndex) { m_iState = iIndex; }
    
public:
    _bool isWalkFinished() { return m_isFinished; }
    _bool isWalkStartEnd() { return m_isWalkStartEnd; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    _bool m_isFinished = { false };
    _bool m_isWalkStartEnd = { false };

    _uint m_iAnimIndex = { 0 };

    vector<_uint> m_RootBoneAnimList;

private:
    void Change_Animation(const _float fTimeDelta);
    virtual HRESULT Add_Components() override;

    void Ready_RootBoneAnimList();

private:
    void Move(const _float& fTimeDelta);

public:
    static CNishiki* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
