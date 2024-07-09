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
        PLAYER, ENEMY, TYPE_END
    };

private:
    CCollision_Manager();
    virtual ~CCollision_Manager() = default;

public:
    HRESULT Initialize();
    HRESULT Add_ImpulseResolution(class CLandObject* pObejct);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수
    HRESULT Add_AttackCollider(class CSocketCollider* pCollider, COLLIDER_TYPE eType);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수
    HRESULT Add_HitCollider(class CSocketCollider* pCollider, COLLIDER_TYPE eType);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수

    HRESULT Add_MapCollider(CCollider* pCollider);

    void Tick();

public:
    /*
    * 실제 충돌처리를 비교할 함수
    * 인자로 넣는 객체와 m_HitColliders 벡터 원소들의 충돌을 비교한다.
    */
    void Enemy_Hit_Collision();
    void Player_Hit_Collision();

    _bool Map_Collision(CCollider* pCollider);

private:
    // 캐릭터가 겹치지않고 밀어내는 함수
    // AABB끼리만 구현되어있음
    void ImpulseResolution();           // 따로 모아둔 벡터에서 서로 밀어내는 작업을 한다 (Tick에서 돌림)

    void ResolveCollision(BoundingSphere* sphere, BoundingOrientedBox* box);

private:
    vector<class CLandObject*> m_ImpulseResolutionObjects;

    /*
    * AttackColliders는 Attack타입의 소켓 콜라이더들을 모아둘 벡터
    * m_AttackColliders[PLAYER] 라면 플레이어의 Attack타입의 콜라이더 중, 현재 켜져있는 애들만 모아둠
    * m_AttackColliders[ENEMY] 라면 적들의 Attack 타입의 콜라이더 중 현재 틱에서 켜져있는 애들만 모아둠
    * 
    * HitColliders는 Hit타입의 소켓 콜라이더들을 모아둘 벡터
    * m_HitColliders[PLAYER] 라면 플레이어의 Hit콜라이더를 모아둘 것이며, m_AttackColliders[ENEMY]와 충돌처리 한다.
    * m_HitColliders[ENEMY] 라면 플레이어의 Hit콜라이더를 모아둘 것이며, m_AttackColliders[PLAYER]와 충돌처리 한다.
    */
    vector<class CSocketCollider*> m_AttackColliders[TYPE_END];
    vector<class CSocketCollider*> m_HitColliders[TYPE_END];

    vector<CCollider*> m_MapColliders;

private:
    void Impulse_Clear();
    void Battle_Clear();

public:
    virtual void Free();
};
END
