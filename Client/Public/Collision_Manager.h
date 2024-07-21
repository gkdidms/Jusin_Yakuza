#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CGameObject;
class CGameInstance;
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
    HRESULT Add_ImpulseResolution(class CLandObject* pObejct);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�
    HRESULT Add_AttackCollider(class CSocketCollider* pCollider, COLLIDER_TYPE eType);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�
    HRESULT Add_HitCollider(class CSocketCollider* pCollider, COLLIDER_TYPE eType);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�

    HRESULT Add_MapCollider(CCollider* pCollider);

    void Tick();

public:
    /*
    * ���� �浹ó���� ���� �Լ�

    * ���ڷ� �ִ� ��ü�� m_HitColliders ���� ���ҵ��� �浹�� ���Ѵ�.
    */
    void Enemy_Hit_Collision();
    void Player_Hit_Collision();

    _bool Map_Collision_Move(CCollider* pCollider, CTransform* pTransform); // �浹 �� position �ٽ� ���α�
    _bool Check_Map_Collision(CCollider* pCollider); // �浹 Ȯ�ο�

    class CLandObject* Get_Near_LandObject(class CLandObject* pObject, vector<CGameObject*>& pObjects);

private:
    // ĳ���Ͱ� ��ġ���ʰ� �о�� �Լ�
    // AABB������ �����Ǿ�����
    void ImpulseResolution();           // ���� ��Ƶ� ���Ϳ��� ���� �о�� �۾��� �Ѵ� (Tick���� ����)

    void ResolveCollision(BoundingSphere* sphere, BoundingOrientedBox* box, CTransform* pTransform);

private:
    vector<class CLandObject*> m_ImpulseResolutionObjects;

    /*
    * AttackColliders�� AttackŸ���� ���� �ݶ��̴����� ��Ƶ� ����
    * m_AttackColliders[PLAYER] ��� �÷��̾��� AttackŸ���� �ݶ��̴� ��, ���� �����ִ� �ֵ鸸 ��Ƶ�
    * m_AttackColliders[ENEMY] ��� ������ Attack Ÿ���� �ݶ��̴� �� ���� ƽ���� �����ִ� �ֵ鸸 ��Ƶ�
    * 
    * HitColliders�� HitŸ���� ���� �ݶ��̴����� ��Ƶ� ����
    * m_HitColliders[PLAYER] ��� �÷��̾��� Hit�ݶ��̴��� ��Ƶ� ���̸�, m_AttackColliders[ENEMY]�� �浹ó�� �Ѵ�.
    * m_HitColliders[ENEMY] ��� �÷��̾��� Hit�ݶ��̴��� ��Ƶ� ���̸�, m_AttackColliders[PLAYER]�� �浹ó�� �Ѵ�.
    */
    vector<class CSocketCollider*> m_AttackColliders[TYPE_END];
    vector<class CSocketCollider*> m_HitColliders[TYPE_END];


    vector<CCollider*> m_MapColliders;

    CGameInstance* m_pGameInstance = { nullptr };


    float           m_fIntersectDistance = { 0 };

private:
    void Impulse_Clear();
    void Battle_Clear();

public:
    virtual void Free();
};
END
