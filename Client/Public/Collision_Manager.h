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
        FROM_PLAYER, FROM_ENEMY, TYPE_END
    };

private:
    CCollision_Manager();
    virtual ~CCollision_Manager() = default;

public:
    HRESULT Initialize();
    HRESULT Add_ImpulseResolution(class CLandObject* pObejct);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수
    HRESULT Add_BattleCollider(class CSocketCollider* pCollider, COLLIDER_TYPE eType);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수

    void Tick();

public:
    /*
    * 실제 충돌처리를 비교할 함수
    * 인자로 넣는 객체와 m_BattleColliders 벡터 원소들의 충돌을 비교한다.
    */
    _bool Collision_FromPlayer(class CSocketCollider* pAttackCollider);
    _bool Collision_FromEnemy(class CLandObject* pObejct);

private:
    // 캐릭터가 겹치지않고 밀어내는 함수
    // AABB끼리만 구현되어있음
    void ImpulseResolution();           // 따로 모아둔 벡터에서 서로 밀어내는 작업을 한다 (Tick에서 돌림)


private:
    vector<class CLandObject*> m_ImpulseResolutionObjects;

    /*
    * FROM 타입이 플레이어라면 플레이어의 ATTACK 콜라이더와 상호작용할 대상들을 저장한다
    * ex) 몬스터의 HIT 콜라이더(몸통, 다리 등), 아이템의 콜라이더
    * 플레이어가 아이템을 무기로 사용중이라면 Collision_FromPlayer() 함수의 인자에 아이템을 넣어준다.
    * 
    * FROM 타입이 몬스터라면 몬스터의 ATTACK 콜라이더와 상호작용할 대상들을 저장한다.
    * ex) 플레이어의 HIT 콜라이더 (몸통, 다리 등)
    */
    vector<class CSocketCollider*> m_BattleColliders[TYPE_END];

private:
    void Impulse_Clear();
    void Battle_Clear();

public:
    virtual void Free();
};
END
