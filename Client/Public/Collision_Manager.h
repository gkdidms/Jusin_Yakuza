#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CCollision_Manager :
    public CBase
{
    DECLARE_SINGLETON(CCollision_Manager)

public:
    enum COLLIDER_TYPE
    {
        ATTACK, HIT, TYPE_END
    };

private:
    CCollision_Manager();
    virtual ~CCollision_Manager() = default;

public:
    HRESULT Initialize();
    HRESULT Add_ImpulseResolution(class CLandObject* pObejct);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수
    HRESULT Add_BattleCollider(class CLandObject* pObejct);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수

    void Tick();

private:
    // 캐릭터가 겹치지않고 밀어내는 함수
    // AABB끼리만 구현되어있음
    void ImpulseResolution();           // 따로 모아둔 벡터에서 서로 밀어내는 작업을 한다 (Tick에서 돌림)
    
    void BattleCollision();             // 데미지관련 충돌처리


private:
    vector<class CLandObject*> m_ImpulseResolutionObjects;
    vector<class CCollider*> m_BattleColliders[TYPE_END];

private:
    void Impulse_Clear();
    void Battle_Clear();

public:
    virtual void Free();
};
END
